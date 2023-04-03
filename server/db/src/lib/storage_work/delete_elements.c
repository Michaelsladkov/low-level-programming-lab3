#include <string.h>

#include "common_types.h"
#include "file_work/file_allocator.h"
#include "find_element.h"
#include "graph_storage_interface.h"
#include "request.h"
#include "storage_controller.h"
#include "string_operations.h"

void deleteString(const struct StorageController *const Controller,
                  const struct MyString String) {
    if (String.Length > SMALL_STRING_LIMIT) {
        deallocate(Controller->Allocator, String.Data.DataPtr);
    }
}

static void supressLinksEnd(struct StorageController *const Controller,
                            struct OptionalFullAddr Addr,
                            struct NodeLink *ToDelete) {
    struct NodeLink LinkC = *ToDelete;
    struct OptionalFullAddr CurrAddr = Addr;
    while (LinkC.Deleted) {
        struct OptionalFullAddr PAddr = LinkC.Previous;
        if (isOptionalFullAddrsEq(PAddr, NULL_FULL_ADDR)) {
            Controller->Storage.Links = NULL_FULL_ADDR;
            Controller->Storage.LastLink = CurrAddr;
            Controller->Storage.LastLink.HasValue = false;
            break;
        }
        Controller->Storage.PlacedLinks -= 1;
        Controller->Storage.LazyDeletedLinkNumber -= 1;
        Controller->Storage.LinksPlaceable += 1;
        struct NodeLink LinkP;
        fetchData(Controller->Allocator, PAddr, sizeof(LinkP), &LinkP);
        if (inDifferentBlocks(CurrAddr, PAddr)) {
            deallocate(Controller->Allocator, CurrAddr);
            Controller->Storage.LinksPlaceable = 0;
            LinkP.Next = NULL_FULL_ADDR;
            storeData(Controller->Allocator, PAddr, sizeof(LinkP), &LinkP);
        }
        Controller->Storage.LastLink = PAddr;
        LinkC = LinkP;
        CurrAddr = PAddr;
    }
}

static void vacuumateLinks(struct StorageController *const Controller) {
    struct GraphStorage Storage;
    fetchData(Controller->Allocator, Controller->StorageAddr, sizeof(Storage), &Storage);
    struct OptionalFullAddr SpaceAddr = Storage.Links;
    if (!SpaceAddr.HasValue) {
        return;
    }
    while (Storage.LazyDeletedLinkNumber != 0) {
        struct NodeLink Space;
        fetchData(Controller->Allocator, SpaceAddr, sizeof(Space), &Space);
        while (!Space.Deleted && Space.Next.HasValue &&
               !isOptionalFullAddrsEq(SpaceAddr, Storage.LastLink)) {
            SpaceAddr = Space.Next;
            fetchData(Controller->Allocator, SpaceAddr, sizeof(Space), &Space);
        }
        if (!Space.Deleted) {
            break;
        }
        struct OptionalFullAddr LoadAddr = SpaceAddr;
        struct NodeLink Load;
        fetchData(Controller->Allocator, LoadAddr, sizeof(struct NodeLink), &Load);
        while (Load.Deleted && Load.Next.HasValue &&
               !isOptionalFullAddrsEq(LoadAddr, Storage.LastLink)) {
            LoadAddr = Load.Next;
            fetchData(Controller->Allocator, LoadAddr, sizeof(struct NodeLink), &Load);
        }
        if (Load.Deleted && isOptionalFullAddrsEq(Storage.LastLink, LoadAddr)) {
            supressLinksEnd(Controller, LoadAddr, &Load);
            break;
        }
        fetchData(Controller->Allocator, LoadAddr, sizeof(Load), &Load);
        Load.Deleted = true;
        storeData(Controller->Allocator, LoadAddr, sizeof(Load), &Load);
        Load.Next = Space.Next;
        Load.Previous = Space.Previous;
        Load.Deleted = false;
        storeData(Controller->Allocator, SpaceAddr, sizeof(Load), &Load);
        SpaceAddr = Space.Next;
    }
    storeData(Controller->Allocator, Controller->StorageAddr, sizeof(Storage), &Controller->Storage);
}

static void deleteSingleNodeLink(struct StorageController *const Controller,
                                 const struct OptionalFullAddr Addr) {
    struct GraphStorage Storage;
    fetchData(Controller->Allocator, Controller->StorageAddr, sizeof(Storage), &Storage);
    struct NodeLink ToDelete;
    fetchData(Controller->Allocator, Addr, sizeof(ToDelete), &ToDelete);
    Storage.LinkNumber -= 1;
    deleteString(Controller, ToDelete.Name);
    ToDelete.Deleted = true;
    Storage.LazyDeletedLinkNumber += 1;
    storeData(Controller->Allocator, Addr, sizeof(ToDelete), &ToDelete);
    Controller->Storage = Storage;
    if (isOptionalFullAddrsEq(Storage.LastLink, Addr)) {
        if (isOptionalFullAddrsEq(ToDelete.Previous, NULL_FULL_ADDR)) {
            Storage.Links = NULL_FULL_ADDR;
        } else {
            Storage.LastLink = ToDelete.Previous;
        }
        Controller->Storage = Storage;
        supressLinksEnd(Controller, Addr, &ToDelete);
    }
    storeData(Controller->Allocator, Controller->StorageAddr, sizeof(Storage), &(Controller->Storage));
}

static size_t deleteNodeLinksByNodeId(struct StorageController *const Controller,
                                      const size_t NodeId, bool CheckLeft, bool CheckRight) {
    struct GraphStorage Storage;
    fetchData(Controller->Allocator, Controller->StorageAddr, sizeof(Storage), &Storage);
    struct OptionalFullAddr NodeLinkAddr = Storage.Links;
    size_t deleted = 0;
    while (NodeLinkAddr.HasValue) {
        struct NodeLink Link;
        fetchData(Controller->Allocator, NodeLinkAddr, sizeof(Link), &Link);
        if (!Link.Deleted) {
            struct OptionalFullAddr OldAddr = NodeLinkAddr;
            if ((CheckLeft && Link.LeftNodeId == NodeId) ||
                (CheckRight && Link.RightNodeId == NodeId)) {
                deleteSingleNodeLink(Controller, OldAddr);
                deleted++;
                if (isOptionalFullAddrsEq(OldAddr, Storage.LastLink)) {
                    break;
                }
                fetchData(Controller->Allocator, NodeLinkAddr, sizeof(Link), &Link);
                fetchData(Controller->Allocator, Controller->StorageAddr, sizeof(Storage), &Storage);
            } else if (isOptionalFullAddrsEq(OldAddr, Storage.LastLink)) {
                break;
            }
        }
        NodeLinkAddr = Link.Next;
    }
    if (Storage.LazyDeletedLinkNumber > Storage.PlacedLinks / 2) {
        vacuumateLinks(Controller);
    }
    return deleted;
}

static void supressNodeEnd(const struct StorageController *const Controller,
                           struct OptionalFullAddr Addr, struct Scheme *Scheme,
                           struct Node *ToDelete) {
    struct Node NodeC = *ToDelete;
    struct OptionalFullAddr CAddr = Addr;
    while (NodeC.Deleted) {
        struct OptionalFullAddr PAddr = NodeC.Previous;
        if (isOptionalFullAddrsEq(PAddr, NULL_FULL_ADDR)) {
            Scheme->Nodes = NULL_FULL_ADDR;
            Scheme->LastNode = CAddr;
            Scheme->LastNode.HasValue = false;
            break;
        }
        Scheme->PlacedNodes -= 1;
        Scheme->LazyDeletedNodeNumber -= 1;
        Scheme->NodesPlaceable += 1;
        struct Node NodeP;
        fetchData(Controller->Allocator, PAddr, sizeof(NodeP), &NodeP);
        if (inDifferentBlocks(CAddr, PAddr)) {
            deallocate(Controller->Allocator, CAddr);
            Scheme->NodesPlaceable = 0;
            NodeP.Next = NULL_FULL_ADDR;
            storeData(Controller->Allocator, PAddr, sizeof(NodeP), &NodeP);
        }
        Scheme->LastNode = PAddr;
        NodeC = NodeP;
        CAddr = PAddr;
    }
}

static void vacuumateNodes(const struct StorageController *const Controller,
                           const struct OptionalFullAddr SchemeAddr) {
    struct Scheme Scheme;
    fetchData(Controller->Allocator, SchemeAddr, sizeof(Scheme), &Scheme);
    struct OptionalFullAddr SpaceAddr = Scheme.Nodes;
    if (!SpaceAddr.HasValue) {
        return;
    }
    size_t NodeSize = sizeof(struct Node) + sizeof(struct Attribute) * Scheme.AttributeNumber;
    while (Scheme.LazyDeletedNodeNumber != 0) {
        struct Node Space;
        fetchData(Controller->Allocator, SpaceAddr, sizeof(Space), &Space);
        while (!Space.Deleted && Space.Next.HasValue &&
               !isOptionalFullAddrsEq(SpaceAddr, Scheme.LastNode)) {
            SpaceAddr = Space.Next;
            fetchData(Controller->Allocator, SpaceAddr, sizeof(Space), &Space);
        }
        if (!Space.Deleted) {
            break;
        }
        struct OptionalFullAddr LoadAddr = SpaceAddr;
        struct Node *Load = malloc(NodeSize);
        fetchData(Controller->Allocator, LoadAddr, sizeof(struct Node), Load);
        while (Load->Deleted && Load->Next.HasValue &&
               !isOptionalFullAddrsEq(LoadAddr, Scheme.LastNode)) {
            LoadAddr = Load->Next;
            fetchData(Controller->Allocator, LoadAddr, sizeof(struct Node), Load);
        }
        if (Load->Deleted && isOptionalFullAddrsEq(Scheme.LastNode, LoadAddr)) {
            supressNodeEnd(Controller, LoadAddr, &Scheme, Load);
            free(Load);
            break;
        }
        fetchData(Controller->Allocator, LoadAddr, NodeSize, Load);
        Load->Deleted = true;
        storeData(Controller->Allocator, LoadAddr, NodeSize, Load);
        Load->Next = Space.Next;
        Load->Previous = Space.Previous;
        Load->Deleted = false;
        storeData(Controller->Allocator, SpaceAddr, NodeSize, Load);
        SpaceAddr = Space.Next;
        free(Load);
    }
    storeData(Controller->Allocator, SchemeAddr, sizeof(Scheme), &Scheme);
}

static size_t deleteSingleNode(const struct StorageController *const Controller,
                               const struct OptionalFullAddr Addr,
                               const struct OptionalFullAddr SchemeAddr) {
    struct Node ToDelete;
    struct Scheme Scheme;
    fetchData(Controller->Allocator, Addr, sizeof(ToDelete), &ToDelete);
    deleteNodeLinksByNodeId(Controller, ToDelete.Id, true, true);
    fetchData(Controller->Allocator, SchemeAddr, sizeof(Scheme), &Scheme);
    Scheme.NodeNumber -= 1;
    size_t AttributesSize = sizeof(struct Attribute) * Scheme.AttributeNumber;
    struct Attribute *Attributes = malloc(AttributesSize);
    fetchData(Controller->Allocator, ToDelete.Attributes, AttributesSize, Attributes);
    for (size_t i = 0; i < Scheme.AttributeNumber; ++i) {
        if (Attributes[i].Type == STRING) {
            deleteString(Controller, Attributes[i].Value.StringValue);
        }
    }
    free(Attributes);
    ToDelete.Deleted = true;
    Scheme.LazyDeletedNodeNumber += 1;
    storeData(Controller->Allocator, Addr, sizeof(ToDelete), &ToDelete);
    if (isOptionalFullAddrsEq(Scheme.LastNode, Addr)) {
        if (isOptionalFullAddrsEq(ToDelete.Previous, NULL_FULL_ADDR)) {
            Scheme.Nodes = NULL_FULL_ADDR;
        } else {
            Scheme.LastNode = ToDelete.Previous;
        }
        supressNodeEnd(Controller, Addr, &Scheme, &ToDelete);
    }
    storeData(Controller->Allocator, SchemeAddr, sizeof(Scheme), &Scheme);
    return 1;
}

static void deleteAllNodes(const struct StorageController *const Controller,
                           const struct OptionalFullAddr SchemeAddr) {
    struct Scheme Scheme;
    fetchData(Controller->Allocator, SchemeAddr, sizeof(Scheme), &Scheme);
    const struct OptionalFullAddr StartAddr = Scheme.Nodes;
    if (!StartAddr.HasValue) {
        return;
    }
    struct OptionalFullAddr NodeAddr = StartAddr;
    while (NodeAddr.HasValue) {
        struct Node CurrentNode;
        fetchData(Controller->Allocator, NodeAddr, sizeof(CurrentNode), &CurrentNode);
        struct OptionalFullAddr OldAddr = NodeAddr;
        deleteSingleNode(Controller, OldAddr, SchemeAddr);
        if (isOptionalFullAddrsEq(Scheme.LastNode, NodeAddr))
            break;
        NodeAddr = CurrentNode.Next;
    }
}

size_t deleteScheme(struct StorageController *const Controller,
                   const struct DeleteSchemeRequest *const Request) {
    struct OptionalFullAddr SchemeAddr = findSchemeAddrByName(Controller, Request->Name);
    struct Scheme ToDelete;
    struct Scheme BeforeDeleted;
    struct Scheme AfterDeleted;
    fetchData(Controller->Allocator, SchemeAddr, sizeof(ToDelete), &ToDelete);
    if (ToDelete.Previous.HasValue) {
        fetchData(Controller->Allocator, ToDelete.Previous, sizeof(BeforeDeleted),
                  &BeforeDeleted);
        BeforeDeleted.Next = ToDelete.Next;
        storeData(Controller->Allocator, ToDelete.Previous, sizeof(BeforeDeleted),
                  &BeforeDeleted);
    }
    if (ToDelete.Next.HasValue) {
        fetchData(Controller->Allocator, ToDelete.Next, sizeof(AfterDeleted), &AfterDeleted);
        AfterDeleted.Previous = ToDelete.Previous;
        storeData(Controller->Allocator, ToDelete.Next, sizeof(AfterDeleted), &AfterDeleted);
    }
    if (ToDelete.AttributesDecription.HasValue) {
        struct AttributeDescription *SchemeAttributeDescriptions =
            malloc(ToDelete.AttributeNumber * sizeof(struct AttributeDescription));
        fetchData(Controller->Allocator, ToDelete.AttributesDecription,
                  ToDelete.AttributeNumber * sizeof(struct AttributeDescription),
                  SchemeAttributeDescriptions);
        for (size_t i = 0; i < ToDelete.AttributeNumber; ++i) {
            deleteString(Controller, SchemeAttributeDescriptions[i].Name);
        }
        free(SchemeAttributeDescriptions);
    }
    deleteAllNodes(Controller, SchemeAddr);
    deleteString(Controller, ToDelete.Name);
    deallocate(Controller->Allocator, SchemeAddr);
    decreaseSchemeNumber(Controller);
    if (isOptionalFullAddrsEq(SchemeAddr, Controller->Storage.Schemes)) {
        updateFirstScheme(Controller, ToDelete.Next);
    }
    if (isOptionalFullAddrsEq(SchemeAddr, Controller->Storage.LastScheme)) {
        updateLastScheme(Controller, ToDelete.Previous);
    }
    return 1;
}

size_t deleteNode(const struct StorageController *const Controller,
                  const struct DeleteNodeRequest *const Request) {
    struct OptionalFullAddr SchemeAddr =
        findSchemeAddrBySchemeId(Controller, Request->SchemeIdType, Request->SchemeId);
    if (Request->ById) {
        struct OptionalFullAddr NodeAddr = findNodeAddrById(Controller, SchemeAddr, Request->Id);
        deleteSingleNode(Controller, NodeAddr, SchemeAddr);
        return 1;
    }
    struct OptionalFullAddr *NodesToDelete;
    const size_t NodesCnt = findNodesByFilters(Controller, SchemeAddr,
                                               Request->AttributesFilterChain, &NodesToDelete);
    for (size_t i = NodesCnt; i != 0; --i) {
        deleteSingleNode(Controller, NodesToDelete[i - 1], SchemeAddr);
    }
    free(NodesToDelete);
    struct Scheme Scheme;
    fetchData(Controller->Allocator, SchemeAddr, sizeof(Scheme), &Scheme);
    if (Scheme.LazyDeletedNodeNumber > Scheme.PlacedNodes / 2 + 1) {
        vacuumateNodes(Controller, SchemeAddr);
    }
    return NodesCnt;
}

size_t deleteNodeLink(const struct StorageController *const Controller,
                      const struct DeleteNodeLinkRequest *const Request) {
    size_t ret = 0;
    if (Request->Type == BY_LEFT_NODE_ID) {
        ret = deleteNodeLinksByNodeId(Controller, Request->Id, true, false);
    }
    if (Request->Type == BY_RIGHT_NODE_ID) {
        ret = deleteNodeLinksByNodeId(Controller, Request->Id, false, true);
    }
    if (Request->Type == BY_ID) {
        struct OptionalFullAddr NodeLinkAddr =
            findNodeLinkAddrById(Controller, Request->Id);
        deleteSingleNodeLink(Controller, NodeLinkAddr);
        ret = 1;
        struct GraphStorage Storage;
        fetchData(Controller->Allocator, Controller->StorageAddr, sizeof(Storage), &Storage);
        if (Storage.LazyDeletedLinkNumber > Storage.PlacedLinks / 2) {
            vacuumateLinks(Controller);
        }
    }
    if (Request->Type == BY_STRING_FILTER) {}
    return ret;
}