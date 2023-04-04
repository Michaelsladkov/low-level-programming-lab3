#include <string.h>

#include "common_types.h"
#include "external_representations.h"
#include "file_work/file_allocator.h"
#include "find_element.h"
#include "graph_storage_interface.h"
#include "logical_structures.h"
#include "request_result.h"

struct NodeResultSet {
    const struct StorageController *Controller;
    struct OptionalFullAddr SchemeAddr;
    size_t Cnt;
    size_t Index;
    struct OptionalFullAddr *NodeAddrs;
};

struct NodeLinkResultSet {
    const struct StorageController *Controller;
    size_t Cnt;
    size_t Index;
    struct OptionalFullAddr *LinkAddrs;
};

struct SchemeResultSet {
    const struct StorageController *Controller;
    size_t Cnt;
    size_t Index;
    struct OptionalFullAddr *SchemeAddrs;
};

static void readMyString(const struct StorageController *const Constroller,
                         const struct MyString String,
                         char* Result) {
    if (String.Length < SMALL_STRING_LIMIT) {
        strcpy(Result, String.Data.InlinedData);
    } else {
        fetchData(Constroller->Allocator, String.Data.DataPtr, String.Length, Result);
    }
    Result[String.Length] = 0;
}

static void getExternalNodeLink(const struct StorageController *const Controller,
                                const struct OptionalFullAddr NodeLinkAddr,
                                struct ExternalNodeLink **Result) {
    struct NodeLink NodeLink;
    fetchData(Controller->Allocator, NodeLinkAddr, sizeof(NodeLink), &NodeLink);
    *Result = malloc(sizeof(struct ExternalNodeLink) + NodeLink.Name.Length + 1);
    (*Result)->Id = NodeLink.Id;
    (*Result)->Type = NodeLink.Type;
    (*Result)->Weight = NodeLink.Weight;
    (*Result)->LeftNodeId = NodeLink.LeftNodeId;
    (*Result)->RightNodeId = NodeLink.RightNodeId;
    (*Result)->Name = (char*)*Result + sizeof(struct ExternalNodeLink);
    readMyString(Controller, NodeLink.Name, (*Result)->Name);
}

static size_t getNodeStringsSize(const struct Attribute *const Attributes,
                                 const size_t AttributeNumber) {
    size_t Result = 0;
    for (size_t i = 0; i < AttributeNumber; ++i) {
        if (Attributes[i].Type == STRING) {
            Result += Attributes[i].Value.StringValue.Length + 1;
        }
    }
    return Result;
}

static void getExternalNode(const struct StorageController *const Controller,
                            const struct OptionalFullAddr NodeAddr,
                            const struct OptionalFullAddr SchemeAddr,
                            struct ExternalNode **Result) {
    struct Node Node;
    fetchData(Controller->Allocator, NodeAddr, sizeof(Node), &Node);
    struct Scheme Scheme;
    fetchData(Controller->Allocator, SchemeAddr, sizeof(Scheme), &Scheme);
    const size_t AttributesSize = sizeof(struct Attribute) * Scheme.AttributeNumber;
    struct Attribute *Attributes = malloc(AttributesSize);
    fetchData(Controller->Allocator, Node.Attributes, AttributesSize, Attributes);
    size_t ResultSize = sizeof(struct ExternalNode) +
                        sizeof(struct ExternalAttribute) * Scheme.AttributeNumber +
                        getNodeStringsSize(Attributes, Scheme.AttributeNumber);
    *Result = malloc(ResultSize);
    struct ExternalAttribute *ExternalAttributes =
        (struct ExternalAttribute *)((char *)*Result + sizeof(struct ExternalNode));
    char *Strings =
        (char *)ExternalAttributes + Scheme.AttributeNumber * sizeof(struct ExternalAttribute);
    for (size_t i = 0; i < Scheme.AttributeNumber; ++i) {
        ExternalAttributes[i].Id = Attributes[i].AttributeId;
        ExternalAttributes[i].Type = Attributes[i].Type;
        if (Attributes[i].Type == BOOL) {
            ExternalAttributes[i].Value.BoolValue = Attributes[i].Value.BoolValue;
        } else if (Attributes[i].Type == INT) {
            ExternalAttributes[i].Value.IntValue = Attributes[i].Value.IntValue;
        }
        if (Attributes[i].Type == FLOAT) {
            ExternalAttributes[i].Value.FloatValue = Attributes[i].Value.FloatValue;
        } else if (Attributes[i].Type == STRING) {
            struct MyString String = Attributes[i].Value.StringValue;
            if (String.Length > SMALL_STRING_LIMIT) {
                fetchData(Controller->Allocator, String.Data.DataPtr, String.Length, Strings);
            } else {
                memcpy(Strings, String.Data.InlinedData, String.Length);
            }
            Strings[String.Length] = 0;
            ExternalAttributes[i].Value.StringAddr = Strings;
            Strings = Strings + String.Length + 1;
        }
    }
    (**Result).Attributes = ExternalAttributes;
    (**Result).AttributesNumber = Scheme.AttributeNumber;
    (**Result).Id = Node.Id;
    (**Result).SchemeId = Node.SchemeId;
    free(Attributes);
}

static size_t getSchemeStringsSize(const struct StorageController *const Controller,
                                  const struct AttributeDescription *const Descriptions,
                                  const struct Scheme *const Scheme) {
    struct MyString String = Scheme->Name;
    size_t Result = 0;
    Result += String.Length + 1;
    for (size_t i = 0; i < Scheme->AttributeNumber; ++i) {
        Result += Descriptions[i].Name.Length + 1;
    }
    return Result;
}

static void getExternalScheme(const struct StorageController *const Controller,
                             const struct OptionalFullAddr Addr,
                             struct ExternalScheme **Result) {
    struct Scheme Scheme;
    fetchData(Controller->Allocator, Addr, sizeof(Scheme), &Scheme);
    const size_t AttributesDescriptionsSize =
        sizeof(struct AttributeDescription) * Scheme.AttributeNumber;
    struct AttributeDescription *AttributesDescriptions = malloc(AttributesDescriptionsSize);
    fetchData(Controller->Allocator, Scheme.AttributesDecription, AttributesDescriptionsSize,
              AttributesDescriptions);
    size_t ExternalSchemeSize =
        sizeof(struct ExternalScheme) +
        Scheme.AttributeNumber * sizeof(struct ExternalAttributeDescription) +
        getSchemeStringsSize(Controller, AttributesDescriptions, &Scheme);
    *Result = malloc(ExternalSchemeSize);
    struct ExternalAttributeDescription *ExternalDescriptions =
        (struct ExternalAttributeDescription *)((char *)*Result + sizeof(struct ExternalScheme));
    char *Strings = (char *)ExternalDescriptions +
                    Scheme.AttributeNumber * sizeof(struct ExternalAttributeDescription);
    (*Result)->Id = Scheme.Id;
    (*Result)->NodesNumber = Scheme.NodeNumber;
    (*Result)->AttributesDescriptionNumber = Scheme.AttributeNumber;
    (*Result)->AttributesDescription = ExternalDescriptions;
    (*Result)->Name = Strings;
    struct MyString SchemeName = Scheme.Name;
    if (SchemeName.Length > SMALL_STRING_LIMIT) {
        fetchData(Controller->Allocator, SchemeName.Data.DataPtr, SchemeName.Length, Strings);
    } else {
        memcpy(Strings, SchemeName.Data.InlinedData, SchemeName.Length);
    }
    Strings[SchemeName.Length] = 0;
    Strings += SchemeName.Length + 1;
    for (size_t i = 0; i < Scheme.AttributeNumber; ++i) {
        ExternalDescriptions[i].AttributeId = AttributesDescriptions[i].AttributeId;
        ExternalDescriptions[i].Type = AttributesDescriptions[i].Type;
        ExternalDescriptions[i].Next =
            i == Scheme.AttributeNumber - 1 ? NULL : ExternalDescriptions + i + 1;
        struct MyString AttrbuteName = AttributesDescriptions[i].Name;
        if (AttrbuteName.Length > SMALL_STRING_LIMIT) {
            fetchData(Controller->Allocator, AttrbuteName.Data.DataPtr, AttrbuteName.Length,
                      Strings);
        } else {
            memcpy(Strings, AttrbuteName.Data.InlinedData, AttrbuteName.Length);
        }
        Strings[AttrbuteName.Length] = 0;
        ExternalDescriptions[i].Name = Strings;
        Strings += AttrbuteName.Length + 1;
    }
    free(AttributesDescriptions);
}

struct NodeResultSet *readNode(const struct StorageController *const Controller,
                               const struct ReadNodeRequest *const Request) {
    struct OptionalFullAddr *Nodes;
    struct OptionalFullAddr SchemeAddr =
        findSchemeAddrBySchemeId(Controller, Request->SchemeIdType, Request->SchemeId);
    struct NodeResultSet *Result = malloc(sizeof(struct NodeResultSet));
    Result->Cnt =
        findNodesByFilters(Controller, SchemeAddr, Request->AttributesFilterChain, &Nodes);
    Result->Index = 0;
    Result->NodeAddrs = Nodes;
    Result->Controller = Controller;
    Result->SchemeAddr = SchemeAddr;
    return Result;
}

struct NodeLinkResultSet *readNodeLink(const struct StorageController *const Controller,
                                       const struct ReadNodeLinkRequest *const Request) {
    struct OptionalFullAddr *NodeLinks;
    struct NodeLinkResultSet *Result = malloc(sizeof(struct NodeLinkResultSet));
    if (Request->Type != BY_STRING_FILTER) {
        Result->Cnt =
            findNodeLinksByIdAndType(Controller, Request->Type, Request->Id, &NodeLinks);
    }
    else {
        //TODO
    }
    Result->Controller = Controller;
    Result->Index = 0;
    Result->LinkAddrs = NodeLinks;
    return Result;
}

struct SchemeResultSet *readScheme(const struct StorageController *const Controller,
                                 const struct ReadSchemeRequest *const Request) {
    struct SchemeResultSet *Result = malloc(sizeof(struct SchemeResultSet));
    struct OptionalFullAddr *SchemeAddr = malloc(sizeof(struct OptionalFullAddr));
    *SchemeAddr = findSchemeAddrByName(Controller, Request->Name);
    Result->SchemeAddrs = SchemeAddr;
    Result->Controller = Controller;
    Result->Cnt = SchemeAddr->HasValue ? 1 : 0;
    Result->Index = 0;
    return Result;
}

bool readResultNode(struct NodeResultSet *ResultSet, struct ExternalNode **Node) {
    if (nodeResultSetIsEmpty(ResultSet))
        return false;
    getExternalNode(ResultSet->Controller, ResultSet->NodeAddrs[ResultSet->Index],
                    ResultSet->SchemeAddr, Node);
    return true;
}

bool moveToNextNode(struct NodeResultSet *ResultSet) {
    if (hasNextNode(ResultSet)) {
        ResultSet->Index += 1;
        return true;
    }
    return false;
}

bool hasNextNode(struct NodeResultSet *ResultSet) {
    return ResultSet->Cnt - ResultSet->Index > 1;
}

bool moveToPreviousNode(struct NodeResultSet *ResultSet) {
    if (hasPreviousNode(ResultSet)) {
        ResultSet->Index -= 1;
        return true;
    }
    return false;
}

bool hasPreviousNode(struct NodeResultSet *ResultSet) { return ResultSet->Index > 0; }

bool nodeResultSetIsEmpty(struct NodeResultSet *ResultSet) { return ResultSet->Cnt == 0; }

void deleteNodeResultSet(struct NodeResultSet **ReultSet) {
    free((**ReultSet).NodeAddrs);
    free(*ReultSet);
    *ReultSet = NULL;
}

bool readResultNodeLink(struct NodeLinkResultSet *ResultSet,
                        struct ExternalNodeLink **NodeLink) {
    if (nodeLinkResultSetIsEmpty(ResultSet))
        return false;
    getExternalNodeLink(ResultSet->Controller, ResultSet->LinkAddrs[ResultSet->Index],
                        NodeLink);
    return true;
}

bool moveToNextNodeLink(struct NodeLinkResultSet *ResultSet) {
    if (hasNextNodeLink(ResultSet)) {
        ResultSet->Index += 1;
        return true;
    }
    return false;
}

bool hasNextNodeLink(struct NodeLinkResultSet *ResultSet) {
    return ResultSet->Cnt - ResultSet->Index > 1;
}

bool moveToPreviousNodeLink(struct NodeLinkResultSet *ResultSet) {
    if (hasPreviousNodeLink(ResultSet)) {
        ResultSet->Index -= 1;
        return true;
    }
    return false;
}

bool hasPreviousNodeLink(struct NodeLinkResultSet *ResultSet) { return ResultSet->Index > 0; }

void deleteNodeLinkResultSet(struct NodeLinkResultSet **ResultSet) {
    free((**ResultSet).LinkAddrs);
    free(*ResultSet);
    *ResultSet = NULL;
}

bool nodeLinkResultSetIsEmpty(struct NodeLinkResultSet *ResultSet) {
    return ResultSet->Cnt == 0;
}

bool readResultScheme(struct SchemeResultSet *ResultSet, struct ExternalScheme **Scheme) {
    if (schemeResultSetIsEmpty(ResultSet))
        return false;
    getExternalScheme(ResultSet->Controller, ResultSet->SchemeAddrs[ResultSet->Index], Scheme);
    return true;
}

bool moveToNextScheme(struct SchemeResultSet *ResultSet) {
    if (hasNextScheme(ResultSet)) {
        ResultSet->Index += 1;
        return true;
    }
    return false;
}

bool hasNextScheme(struct SchemeResultSet *ResultSet) {
    return ResultSet->Cnt - ResultSet->Index > 1;
}

bool moveToPreviousScheme(struct SchemeResultSet *ResultSet) {
    if (hasPreviousScheme(ResultSet)) {
        ResultSet->Index -= 1;
        return true;
    }
    return false;
}

bool hasPreviousScheme(struct SchemeResultSet *ResultSet) { return ResultSet->Index > 0; }

bool schemeResultSetIsEmpty(struct SchemeResultSet *ResultSet) { return ResultSet->Cnt == 0; }

void deleteSchemeResultSet(struct SchemeResultSet **ResultSet) {
    free((**ResultSet).SchemeAddrs);
    free(*ResultSet);
    *ResultSet = NULL;
}

void deleteExternalNode(struct ExternalNode **Node) {
    if (Node == NULL || *Node == NULL)
        return;
    free(*Node);
    *Node = NULL;
}

void deleteExternalNodeLink(struct ExternalNodeLink **Link) {
    if (Link == NULL || *Link == NULL)
        return;
    free(*Link);
    *Link = NULL;
}

void deleteExternalScheme(struct ExternalScheme **Scheme) {
    if (Scheme == NULL || *Scheme == NULL)
        return;
    free(*Scheme);
    *Scheme = NULL;
}

size_t nodeResultSetGetSize(struct NodeResultSet *ResultSet) { return ResultSet->Cnt; }

size_t nodeLinkResultSetGetSize(struct NodeLinkResultSet *ResultSet) { return ResultSet->Cnt; }

size_t schemeResultSetGetSize(struct SchemeResultSet *ResultSet) { return ResultSet->Cnt; }
