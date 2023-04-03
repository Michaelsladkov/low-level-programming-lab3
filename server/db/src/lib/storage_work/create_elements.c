#include <string.h>

#include "common_types.h"
#include "external_representations.h"
#include "file_work/file_allocator.h"
#include "find_element.h"
#include "graph_storage_interface.h"
#include "params.h"
#include "request.h"
#include "storage_controller.h"
#include "string_operations.h"

struct MyString createString(struct StorageController *const Controller,
                             const char *const String) {
    size_t StringLength = strlen(String) + 1;
    struct MyString MyString;
    MyString.Length = StringLength;
    if (StringLength < SMALL_STRING_LIMIT) {
        memcpy(MyString.Data.InlinedData, String, StringLength);
        MyString.Data.InlinedData[StringLength] = 0;
    } else {
        struct OptionalFullAddr StringAddr = allocate(Controller->Allocator, StringLength);
        storeData(Controller->Allocator, StringAddr, sizeof(struct MyString), &MyString);
        MyString.Data.DataPtr = StringAddr;
        size_t CharsWritten =
            storeData(Controller->Allocator, MyString.Data.DataPtr, StringLength + 1, String);
        if (CharsWritten < StringLength) {
            perror("Error while writing string");
        }
    }
    return MyString;
}

static size_t getAttributeDescriptionNumber(const struct CreateSchemeRequest *const Request) {
    size_t AttributeDescriptionNumber = 0;
    const struct ExternalAttributeDescription *CurrentAttributeDescription =
        (const struct ExternalAttributeDescription *)Request->AttributesDescription;
    while (CurrentAttributeDescription != NULL) {
        AttributeDescriptionNumber++;
        CurrentAttributeDescription =
            (const struct ExternalAttributeDescription *)CurrentAttributeDescription->Next;
    }
    return AttributeDescriptionNumber;
}

static struct AttributeDescription
attributeDescriptionFromExternal(struct StorageController *const Controller,
                                 const struct ExternalAttributeDescription *const External) {
    struct AttributeDescription Description;
    Description.Type = External->Type;
    Description.Name = createString(Controller, External->Name);
    Description.AttributeId = External->AttributeId;
    Description.Next = NULL_FULL_ADDR;
    return Description;
}

static void writeSchemeAttributesDescription(struct StorageController *const Controller,
                                            const struct OptionalFullAddr FirstAttributeAddr,
                                            const struct CreateSchemeRequest *const Request) {
    const struct ExternalAttributeDescription *CurrentExternal =
        (const struct ExternalAttributeDescription *)Request->AttributesDescription;
    struct OptionalFullAddr CurrentAttributeAddr = FirstAttributeAddr;
    do {
        struct AttributeDescription CurrentAttributeDescription =
            attributeDescriptionFromExternal(Controller, CurrentExternal);
        if (CurrentExternal->Next != NULL) {
            CurrentAttributeDescription.Next = getOptionalFullAddr(
                CurrentAttributeAddr.BlockOffset,
                CurrentAttributeAddr.DataOffset + sizeof(struct AttributeDescription));
        }
        storeData(Controller->Allocator, CurrentAttributeAddr,
                  sizeof(struct AttributeDescription), &CurrentAttributeDescription);
        CurrentAttributeAddr = CurrentAttributeDescription.Next;
        CurrentExternal = (const struct ExternalAttributeDescription *)CurrentExternal->Next;
    } while (CurrentExternal != NULL);
}

size_t createScheme(struct StorageController *const Controller,
                   const struct CreateSchemeRequest *const Request) {
    size_t Id = Controller->Storage.NextSchemeId;
    const size_t AttributeDescriptionNumber = getAttributeDescriptionNumber(Request);
    const size_t NodeSize =
        sizeof(struct Node) + sizeof(struct Attribute) * AttributeDescriptionNumber;
    const size_t SchemeSize =
        sizeof(struct Scheme) + sizeof(struct AttributeDescription) * AttributeDescriptionNumber;
    const size_t BlockNodesSize = SCHEME_NODES_PER_BLOCK * NodeSize;
    const size_t SchemeAndDataSize = BlockNodesSize + SchemeSize;
    struct OptionalFullAddr SchemeAddr = allocate(Controller->Allocator, SchemeAndDataSize);
    struct Scheme *Scheme = malloc(SchemeSize);
    Scheme->Id = Id;
    Scheme->NodeNumber = 0;
    Scheme->AttributeNumber = AttributeDescriptionNumber;
    Scheme->AttributesDecription =
        getOptionalFullAddr(SchemeAddr.BlockOffset, SchemeAddr.DataOffset + sizeof(struct Scheme));
    if (Request->AttributesDescription != NULL) {
        writeSchemeAttributesDescription(Controller, Scheme->AttributesDecription, Request);
    } else {
        Scheme->AttributesDecription = NULL_FULL_ADDR;
    }
    Scheme->Name = createString(Controller, Request->Name);
    Scheme->Nodes = getOptionalFullAddr(SchemeAddr.BlockOffset, SchemeAddr.DataOffset + SchemeSize);
    Scheme->Nodes.HasValue = false;
    Scheme->LastNode = Scheme->Nodes;
    Scheme->Next = NULL_FULL_ADDR;
    Scheme->Previous = Controller->Storage.LastScheme;
    Scheme->NodesPlaceable = SCHEME_NODES_PER_BLOCK;
    Scheme->PlacedNodes = 0;
    Scheme->LazyDeletedNodeNumber = 0;
    storeData(Controller->Allocator, SchemeAddr, sizeof(struct Scheme), Scheme);
    increaseSchemeNumber(Controller);
    if (!Controller->Storage.Schemes.HasValue) {
        updateFirstScheme(Controller, SchemeAddr);
    }
    updateLastScheme(Controller, SchemeAddr);
    size_t SchemeId = Scheme->Id;
    free(Scheme);
    return SchemeId;
}

static struct OptionalFullAddr getNewNodeAddr(struct StorageController *const Controller,
                                              struct Scheme *const Scheme) {
    if (Scheme->NodesPlaceable > 0) {
        if (!isOptionalFullAddrsEq(Scheme->LastNode, NULL_FULL_ADDR) &&
            !Scheme->LastNode.HasValue) {
            Scheme->LastNode.HasValue = true;
            return Scheme->LastNode;
        }
        if (Scheme->LastNode.HasValue) {
            struct Node LastNode;
            fetchData(Controller->Allocator, Scheme->LastNode, sizeof(LastNode), &LastNode);
            if (LastNode.Next.HasValue) {
                return LastNode.Next;
            }
        }
        return NULL_FULL_ADDR;
    }
    const size_t NodesBlockSize =
        (sizeof(struct Node) + sizeof(struct Attribute) * Scheme->AttributeNumber) *
        SCHEME_NODES_PER_BLOCK;
    struct Node LastNode;
    fetchData(Controller->Allocator, Scheme->LastNode, sizeof(LastNode), &LastNode);
    const struct OptionalFullAddr NewBlockAddr =
        allocate(Controller->Allocator, NodesBlockSize);
    Scheme->NodesPlaceable = SCHEME_NODES_PER_BLOCK;
    LastNode.Next = NewBlockAddr;
    storeData(Controller->Allocator, Scheme->LastNode, sizeof(LastNode), &LastNode);
    return NewBlockAddr;
}

static size_t createNodeBySchemeAddr(struct StorageController *const Controller,
                                    const struct OptionalFullAddr Addr,
                                    struct ExternalAttribute *Attributes) {
    struct Scheme Scheme;
    fetchData(Controller->Allocator, Addr, sizeof(Scheme), &Scheme);
    struct Node NewNode;
    const struct OptionalFullAddr NewNodeAddr = getNewNodeAddr(Controller, &Scheme);
    const size_t AttributesSize = sizeof(struct Attribute) * Scheme.AttributeNumber;
    struct Attribute *AttributesToStore = malloc(AttributesSize);
    const struct OptionalFullAddr AttributesAddr = getOptionalFullAddr(
        NewNodeAddr.BlockOffset, NewNodeAddr.DataOffset + sizeof(struct Node));
    struct AttributeDescription *SchemeAttributesDescription =
        malloc(sizeof(struct AttributeDescription) * Scheme.AttributeNumber);
    fetchData(Controller->Allocator, Scheme.AttributesDecription,
              sizeof(struct AttributeDescription) * Scheme.AttributeNumber,
              SchemeAttributesDescription);
    for (size_t i = 0; i < Scheme.AttributeNumber; ++i) {
        size_t AttributeId = Attributes[i].Id;
        if (Attributes[i].Type != SchemeAttributesDescription[AttributeId].Type) {
            return 0;
        }
    }
    for (size_t i = 0; i < Scheme.AttributeNumber; ++i) {
        struct Attribute Attribute;
        Attribute.AttributeId = Attributes[i].Id;
        if (i != Scheme.AttributeNumber - 1) {
            Attribute.Next =
                getOptionalFullAddr(AttributesAddr.BlockOffset,
                                    AttributesAddr.DataOffset + (i + 1) * sizeof(Attribute));
        } else {
            Attribute.Next = NULL_FULL_ADDR;
        }
        Attribute.Type = Attributes[i].Type;
        if (Attribute.Type == INT) {
            Attribute.Value.IntValue = Attributes[i].Value.IntValue;
        }
        if (Attribute.Type == FLOAT) {
            Attribute.Value.FloatValue = Attributes[i].Value.FloatValue;
        }
        if (Attribute.Type == BOOL) {
            Attribute.Value.BoolValue = Attributes[i].Value.BoolValue;
        }
        if (Attribute.Type == STRING) {
            Attribute.Value.StringValue =
                createString(Controller, Attributes[i].Value.StringAddr);
        }
        AttributesToStore[i] = Attribute;
    }
    storeData(Controller->Allocator, AttributesAddr, AttributesSize, AttributesToStore);
    Scheme.NodesPlaceable -= 1;
    Scheme.PlacedNodes += 1;
    NewNode.Id = Controller->Storage.NextNodeId;
    NewNode.SchemeId = Scheme.Id;
    NewNode.Attributes = AttributesAddr;
    if (Scheme.NodesPlaceable > 0) {
        const size_t FullNodeSize =
            sizeof(struct Node) + Scheme.AttributeNumber * sizeof(struct Attribute);
        NewNode.Next =
            getOptionalFullAddr(NewNodeAddr.BlockOffset, NewNodeAddr.DataOffset + FullNodeSize);
    } else {
        NewNode.Next = NULL_FULL_ADDR;
    }
    NewNode.Deleted = false;
    increaseNodeNumber(Controller);
    Scheme.NodeNumber += 1;
    if (!Scheme.Nodes.HasValue) {
        Scheme.Nodes = Scheme.LastNode = NewNodeAddr;
        storeData(Controller->Allocator, Addr, sizeof(Scheme), &Scheme);
        NewNode.Previous = NULL_FULL_ADDR;
    } else {
        struct Node OldLastNode;
        fetchData(Controller->Allocator, Scheme.LastNode, sizeof(OldLastNode), &OldLastNode);
        OldLastNode.Next = NewNodeAddr;
        storeData(Controller->Allocator, Scheme.LastNode, sizeof(OldLastNode), &OldLastNode);
        NewNode.Previous = Scheme.LastNode;
        Scheme.LastNode = NewNodeAddr;
    }
    storeData(Controller->Allocator, Addr, sizeof(Scheme), &Scheme);
    storeData(Controller->Allocator, NewNodeAddr, sizeof(NewNode), &NewNode);

    free(AttributesToStore);
    free(SchemeAttributesDescription);
    return NewNode.Id;
}

size_t createNode(struct StorageController *const Controller,
                  const struct CreateNodeRequest *const Request) {
    struct OptionalFullAddr SchemeAddr =
        findSchemeAddrBySchemeId(Controller, Request->SchemeIdType, Request->SchemeId);
    return createNodeBySchemeAddr(Controller, SchemeAddr, Request->Attributes);
}

static struct OptionalFullAddr getNewLinkAddr(struct StorageController *const Controller,
                                              struct GraphStorage *const Storage) {
    if (Storage->LinksPlaceable > 0) {
        if (!isOptionalFullAddrsEq(Storage->LastLink, NULL_FULL_ADDR) &&
            !Storage->LastLink.HasValue) {
            Storage->LastLink.HasValue = true;
            return Storage->LastLink;
        }
        if (Storage->LastLink.HasValue) {
            struct NodeLink LastLink;
            fetchData(Controller->Allocator, Storage->LastLink, sizeof(LastLink), &LastLink);
            if (LastLink.Next.HasValue) {
                return LastLink.Next;
            }
        }
        return NULL_FULL_ADDR;
    }
    const size_t LinkBlockSize = sizeof(struct NodeLink) * LINKS_PER_BLOCK;
    struct NodeLink LastLink;
    fetchData(Controller->Allocator, Storage->LastLink, sizeof(LastLink), &LastLink);
    const struct OptionalFullAddr NewBlockAddr = allocate(Controller->Allocator, LinkBlockSize);
    Storage->LinksPlaceable = LINKS_PER_BLOCK;
    LastLink.Next = NewBlockAddr;
    storeData(Controller->Allocator, Storage->LastLink, sizeof(LastLink), &LastLink);
    return NewBlockAddr;
}

size_t createNodeLink(struct StorageController *const Controller,
                            const struct CreateNodeLinkRequest *const Request) {
    struct GraphStorage Storage;
    fetchData(Controller->Allocator, Controller->StorageAddr, sizeof(Storage), &Storage);
    struct OptionalFullAddr NewLinkAddr = getNewLinkAddr(Controller, &Storage);
    struct NodeLink NewLink;
    Storage.LinksPlaceable -= 1;
    Storage.PlacedLinks += 1;
    NewLink.Id = Controller->Storage.NextNodeLinkId;
    NewLink.Type = Request->Type;
    if (Storage.LinksPlaceable > 0) {
        NewLink.Next = getOptionalFullAddr(NewLinkAddr.BlockOffset,
                                           NewLinkAddr.DataOffset + sizeof(NewLink));
    } else {
        NewLink.Next = NULL_FULL_ADDR;
    }
    if (!Storage.Links.HasValue) {
        Storage.Links = Storage.LastLink = NewLinkAddr;
        Controller->Storage = Storage;
        storeData(Controller->Allocator, Controller->StorageAddr, sizeof(Storage), &Storage);
        NewLink.Previous = NULL_FULL_ADDR;
    } else {
        struct NodeLink OldLastLink;
        fetchData(Controller->Allocator, Storage.LastLink, sizeof(OldLastLink), &OldLastLink);
        OldLastLink.Next = NewLinkAddr;
        storeData(Controller->Allocator, Storage.LastLink, sizeof(OldLastLink), &OldLastLink);
        NewLink.Previous = Storage.LastLink;
        Storage.LastLink = NewLinkAddr;
        storeData(Controller->Allocator, Controller->StorageAddr, sizeof(Storage), &Storage);
    }
    NewLink.LeftNodeId = Request->LeftNodeId;
    NewLink.RightNodeId = Request->RightNodeId;
    NewLink.Weight = Request->Weight;
    NewLink.Name = createString(Controller, Request->Name);
    NewLink.Deleted = false;
    if (Storage.LastLink.HasValue) {
        struct NodeLink OldLast;
        fetchData(Controller->Allocator, Storage.LastLink, sizeof(OldLast), &OldLast);
        OldLast.Next = NewLinkAddr;
        storeData(Controller->Allocator, Storage.LastLink, sizeof(OldLast), &OldLast);
    }
    storeData(Controller->Allocator, NewLinkAddr, sizeof(NewLink), &NewLink);
    Storage.LastLink = NewLinkAddr;
    Storage.LinkNumber += 1;
    if (!Storage.Links.HasValue) {
        Storage.Links = NewLinkAddr;
    }
    Controller->Storage = Storage;
    storeData(Controller->Allocator, Controller->StorageAddr, sizeof(Storage), &Storage);
    increaseNodeLinkNumber(Controller);
    return NewLink.Id;
}
