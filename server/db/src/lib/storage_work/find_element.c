#include "find_element.h"

#include "common_types.h"
#include "logical_structures.h"
#include "malloc.h"
#include "params.h"
#include "request.h"
#include "storage_controller.h"

#include <string.h>

struct OptionalFullAddr findSchemeAddrById(const struct StorageController *const Controller,
                                          const size_t Id) {
    struct OptionalFullAddr SchemeAddr = Controller->Storage.Schemes;
    while (SchemeAddr.HasValue) {
        struct Scheme Scheme;
        fetchData(Controller->Allocator, SchemeAddr, sizeof(struct Scheme), &Scheme);
        if (Scheme.Id == Id) {
            return SchemeAddr;
        }
        SchemeAddr = Scheme.Next;
    }
    return NULL_FULL_ADDR;
}

struct OptionalFullAddr findSchemeAddrByName(const struct StorageController *const Controller,
                                            const char *const Name) {
    struct OptionalFullAddr SchemeAddr = Controller->Storage.Schemes;
    struct OptionalFullAddr Result = NULL_FULL_ADDR;
    while (SchemeAddr.HasValue) {
        struct Scheme Scheme;
        fetchData(Controller->Allocator, SchemeAddr, sizeof(struct Scheme), &Scheme);
        struct MyString SchemeName = Scheme.Name;
        char *SchemeNameStr = malloc(SchemeName.Length + 1);
        if (SchemeName.Length > SMALL_STRING_LIMIT) {
            fetchData(Controller->Allocator, SchemeName.Data.DataPtr, SchemeName.Length,
                      SchemeNameStr);
        } else {
            memcpy(SchemeNameStr, SchemeName.Data.InlinedData, SchemeName.Length);
        }
        SchemeNameStr[SchemeName.Length] = 0;
        if (strcmp(SchemeNameStr, Name) == 0) {
            Result = SchemeAddr;
        }
        free(SchemeNameStr);
        if (Result.HasValue)
            return Result;
        SchemeAddr = Scheme.Next;
    }
    return NULL_FULL_ADDR;
}

struct OptionalFullAddr findSchemeAddrBySchemeId(const struct StorageController *const Controller,
                                               const enum SchemeIdType IdType,
                                               const union SchemeId SchemeId) {
    if (IdType == Scheme_ID) {
        return findSchemeAddrById(Controller, SchemeId.SchemeId);
    }
    if (IdType == Scheme_NAME) {
        return findSchemeAddrByName(Controller, SchemeId.SchemeName);
    }
    return NULL_FULL_ADDR;
}

static bool matchIntFilter(const struct IntFilter *const Filter, int32_t Value) {
    if (Filter->HasMin && Filter->Min > Value) {
        return false;
    }
    if (Filter->HasMax && Filter->Max < Value) {
        return false;
    }
    return true;
}

static bool matchFloatFilter(const struct FloatFilter *const Filter, float Value) {
    if (Filter->HasMin && Filter->Min > Value) {
        return false;
    }
    if (Filter->HasMax && Filter->Max < Value) {
        return false;
    }
    return true;
}

static bool matchStringFilter(const struct StorageController* const Controller, const struct StringFilter *const Filter, struct MyString Value) {
    bool result = true;
    char* Str = malloc(Value.Length + 1);
    if (Value.Length < SMALL_STRING_LIMIT) {
        strcpy(Str, Value.Data.InlinedData);
    } else {
        fetchData(Controller->Allocator, Value.Data.DataPtr, Value.Length, Str);
    }
    Str[Value.Length] = 0;
    if (Filter->Type == STRING_EQUAL) {
        if (strcmp(Str, Filter->Data.StringEqual) != 0) result = false;
    }
    if (Filter->Type == STRLEN_RANGE) {
        result = matchIntFilter(&(Filter->Data.StrlenRange), Value.Length);
    }
    if (Filter->Type == CONTAINS) {
        if (!strstr(Str, Filter->Data.StringEqual)) result = false;
    }
    free(Str);
    return result;
}

static bool matchFilterAndAttributeType(enum FILTER_TYPE FT, enum ATTRIBUTE_TYPE AT) {
    if (AT == INT)
        return FT == INT_FILTER;
    if (AT == FLOAT)
        return FT == FLOAT_FILTER;
    if (AT == BOOL)
        return FT == BOOL_FILTER;
    if (AT == STRING)
        return FT == STRING_FILTER;
    return false;
}

static bool checkNodeMatchesFilter(const struct StorageController *const Controller,
                                   const struct OptionalFullAddr NodeAddr,
                                   const struct Scheme *const Scheme,
                                   const struct OptionalFullAddr SchemeAddr,
                                   const struct AttributeFilter *const FilterChain) {
    if (FilterChain == NULL) {
        return true;
    }
    struct Node ToCheck;
    fetchData(Controller->Allocator, NodeAddr, sizeof(ToCheck), &ToCheck);
    const size_t AttributesSize = sizeof(struct Attribute) * Scheme->AttributeNumber;
    struct Attribute *Attributes = malloc(AttributesSize);
    fetchData(Controller->Allocator, ToCheck.Attributes, AttributesSize, Attributes);
    const struct AttributeFilter *Filter = FilterChain;
    bool result = true;
    while (Filter != NULL) {
        if (Filter->Type == LINK_FILTER) {
            if (Filter->Data.Link.Relation == HAS_LINK_TO) {
                struct OptionalFullAddr *LinkAddrs;
                bool HasLink = false;
                size_t NeighboursCnt = findNodeLinksByIdAndType(
                    Controller, BY_LEFT_NODE_ID, ToCheck.Id, &LinkAddrs);
                for (size_t i = 0; i < NeighboursCnt; ++i) {
                    struct NodeLink Link;
                    fetchData(Controller->Allocator, LinkAddrs[i], sizeof(Link), &Link);
                    if (Link.RightNodeId == Filter->Data.Link.NodeId) {
                        if (matchFloatFilter(&(Filter->Data.Link.WeightFilter), Link.Weight)) {
                            HasLink = true;
                            break;
                        }
                    }
                }
                free(LinkAddrs);
                if (!HasLink) {
                    NeighboursCnt = findNodeLinksByIdAndType(
                        Controller, BY_RIGHT_NODE_ID, ToCheck.Id, &LinkAddrs);
                    for (size_t i = 0; i < NeighboursCnt; ++i) {
                        struct NodeLink Link;
                        fetchData(Controller->Allocator, LinkAddrs[i], sizeof(Link), &Link);
                        if (Link.LeftNodeId == Filter->Data.Link.NodeId &&
                            Link.Type == UNIDIRECTIONAL) {
                            if (matchFloatFilter(&(Filter->Data.Link.WeightFilter),
                                                 Link.Weight)) {
                                HasLink = true;
                                break;
                            }
                        }
                    }
                    free(LinkAddrs);
                }
                if (!HasLink) {
                    result = false;
                    break;
                }
            }
            if (Filter->Data.Link.Relation == HAS_LINK_FROM) {
                struct OptionalFullAddr *LinkAddrs;
                bool HasLink = false;
                size_t NeighboursCnt = findNodeLinksByIdAndType(
                    Controller, BY_RIGHT_NODE_ID, ToCheck.Id, &LinkAddrs);
                for (size_t i = 0; i < NeighboursCnt; ++i) {
                    struct NodeLink Link;
                    fetchData(Controller->Allocator, LinkAddrs[i], sizeof(Link), &Link);
                    if (Link.LeftNodeId == Filter->Data.Link.NodeId) {
                        if (matchFloatFilter(&(Filter->Data.Link.WeightFilter), Link.Weight)) {
                            HasLink = true;
                            break;
                        }
                    }
                }
                free(LinkAddrs);
                if (!HasLink) {
                    NeighboursCnt = findNodeLinksByIdAndType(
                        Controller, BY_LEFT_NODE_ID, ToCheck.Id, &LinkAddrs);
                    for (size_t i = 0; i < NeighboursCnt; ++i) {
                        struct NodeLink Link;
                        fetchData(Controller->Allocator, LinkAddrs[i], sizeof(Link), &Link);
                        if (Link.RightNodeId == Filter->Data.Link.NodeId &&
                            Link.Type == UNIDIRECTIONAL) {
                            if (matchFloatFilter(&(Filter->Data.Link.WeightFilter),
                                                 Link.Weight)) {
                                HasLink = true;
                                break;
                            }
                        }
                    }
                    free(LinkAddrs);
                }
                if (!HasLink) {
                    result = false;
                    break;
                }
            }
            Filter = Filter->Next;
            continue;
        }
        if (Scheme->AttributeNumber == 0) {
            return false;
        }
        for (size_t i = 0; i < Scheme->AttributeNumber; ++i) {
            if (Attributes[i].AttributeId == Filter->AttributeId &&
                matchFilterAndAttributeType(Filter->Type, Attributes[i].Type)) {
                if (Filter->Type == BOOL_FILTER) {
                    if (Attributes[i].Value.BoolValue != Filter->Data.Bool.Value) {
                        result = false;
                        break;
                    }
                    continue;
                }
                if (Filter->Type == INT_FILTER) {
                    if (!matchIntFilter(&(Filter->Data.Int), Attributes[i].Value.IntValue)) {
                        result = false;
                        break;
                    }
                    continue;
                }
                if (Filter->Type == FLOAT_FILTER) {
                    if (!matchFloatFilter(&(Filter->Data.Float),
                                          Attributes[i].Value.FloatValue)) {
                        result = false;
                        break;
                    }
                    continue;
                }
                if (Filter->Type == STRING_FILTER) {
                    struct MyString AttributeString = Attributes[i].Value.StringValue;
                    if (Filter->Data.String.Type == STRLEN_RANGE) {
                        const size_t AttributeStringLength = AttributeString.Length;
                        if (!matchIntFilter(&(Filter->Data.String.Data.StrlenRange),
                                            AttributeStringLength)) {
                            result = false;
                            break;
                        }
                        continue;
                    }
                    if (Filter->Data.String.Type == STRING_EQUAL) {
                        char *AttributeStringStr = malloc(AttributeString.Length + 1);
                        if (AttributeString.Length > SMALL_STRING_LIMIT) {
                            fetchData(Controller->Allocator, AttributeString.Data.DataPtr,
                                      AttributeString.Length + 1, AttributeStringStr);
                        } else {
                            memcpy(AttributeStringStr, AttributeString.Data.InlinedData,
                                   AttributeString.Length + 1);
                        }
                        if (strcmp(AttributeStringStr, Filter->Data.String.Data.StringEqual) !=
                            0) {
                            result = false;
                        }
                        free(AttributeStringStr);
                        if (!result)
                            break;
                        continue;
                    }
                }
            }
        }
        if (!result) {
            break;
        }
        Filter = Filter->Next;
    }
    free(Attributes);
    return result;
}

size_t findNodesByFilters(const struct StorageController *const Controller,
                          const struct OptionalFullAddr SchemeAddr,
                          const struct AttributeFilter *AttributeFilterChain,
                          struct OptionalFullAddr **Result) {
    struct Scheme Scheme;
    fetchData(Controller->Allocator, SchemeAddr, sizeof(Scheme), &Scheme);
    size_t GoodNodesCnt = 0;
    struct OptionalFullAddr NodeAddr = Scheme.Nodes;
    *Result = malloc(sizeof(struct OptionalFullAddr) * SCHEME_NODES_PER_BLOCK);
    size_t ResultCapacity = SCHEME_NODES_PER_BLOCK;
    while (NodeAddr.HasValue) {
        struct Node ToCheck;
        fetchData(Controller->Allocator, NodeAddr, sizeof(ToCheck), &ToCheck);
        if (!ToCheck.Deleted && checkNodeMatchesFilter(Controller, NodeAddr, &Scheme, SchemeAddr,
                                                       AttributeFilterChain)) {
            (*Result)[GoodNodesCnt] = NodeAddr;
            GoodNodesCnt++;
        }
        if (GoodNodesCnt + 3 > ResultCapacity) {
            *Result = realloc(*Result, ResultCapacity * 2 * sizeof(struct OptionalFullAddr));
            ResultCapacity = ResultCapacity * 2;
        }
        if (!isOptionalFullAddrsEq(NodeAddr, Scheme.LastNode)) {
            NodeAddr = ToCheck.Next;
        } else
            break;
    }
    return GoodNodesCnt;
}

static bool checkNodeLinkMatchRequest(const struct NodeLink *const Link,
                                      const enum NodeLinkRequestType Type, const size_t Id) {
    return Type == ALL || (Type == BY_ID && Link->Id == Id) ||
           (Type == BY_LEFT_NODE_ID && Link->LeftNodeId == Id) ||
           (Type == BY_RIGHT_NODE_ID && Link->RightNodeId == Id);
}

size_t findNodeLinksByIdAndType(const struct StorageController *Controller,
                                const enum NodeLinkRequestType Type, const size_t Id,
                                struct OptionalFullAddr **Result) {
    struct GraphStorage Storage;
    fetchData(Controller->Allocator, Controller->StorageAddr, sizeof(Storage), &Storage);
    size_t Cnt = 0;
    struct OptionalFullAddr NodeLinkAddr = Storage.Links;
    while (NodeLinkAddr.HasValue) {
        struct NodeLink Link;
        fetchData(Controller->Allocator, NodeLinkAddr, sizeof(Link), &Link);
        if (!Link.Deleted && checkNodeLinkMatchRequest(&Link, Type, Id)) {
            Cnt++;
        }
        if (isOptionalFullAddrsEq(NodeLinkAddr, Storage.LastLink))
            break;
        NodeLinkAddr = Link.Next;
    }
    *Result = malloc(sizeof(struct OptionalFullAddr) * Cnt);
    NodeLinkAddr = Storage.Links;
    size_t Index = 0;
    while (NodeLinkAddr.HasValue) {
        struct NodeLink Link;
        fetchData(Controller->Allocator, NodeLinkAddr, sizeof(Link), &Link);
        if (!Link.Deleted && checkNodeLinkMatchRequest(&Link, Type, Id)) {
            (*Result)[Index] = NodeLinkAddr;
            Index++;
        }
        if (isOptionalFullAddrsEq(NodeLinkAddr, Storage.LastLink))
            break;
        NodeLinkAddr = Link.Next;
    }
    return Cnt;
}

size_t findNodeLinksByName(const struct StorageController *Controller,
                           const struct StringFilter Filter,
                           struct OptionalFullAddr **Result) {
    struct GraphStorage Storage;
    fetchData(Controller->Allocator, Controller->StorageAddr, sizeof(Storage), &Storage);
    size_t Cnt = 0;
    struct OptionalFullAddr NodeLinkAddr = Storage.Links;
    while (NodeLinkAddr.HasValue) {
        struct NodeLink Link;
        fetchData(Controller->Allocator, NodeLinkAddr, sizeof(Link), &Link);
        if (!Link.Deleted && matchStringFilter(Controller, &Filter, Link.Name)) {
            Cnt++;
        }
        if (isOptionalFullAddrsEq(NodeLinkAddr, Storage.LastLink))
            break;
        NodeLinkAddr = Link.Next;
    }
    *Result = malloc(sizeof(struct OptionalFullAddr) * Cnt);
    NodeLinkAddr = Storage.Links;
    size_t Index = 0;
    while (NodeLinkAddr.HasValue) {
        struct NodeLink Link;
        fetchData(Controller->Allocator, NodeLinkAddr, sizeof(Link), &Link);
        if (!Link.Deleted && matchStringFilter(Controller, &Filter, Link.Name)) {
            (*Result)[Index] = NodeLinkAddr;
            Index++;
        }
        if (isOptionalFullAddrsEq(NodeLinkAddr, Storage.LastLink))
            break;
        NodeLinkAddr = Link.Next;
    }
    return Cnt;
}

struct OptionalFullAddr findNodeLinkAddrById(const struct StorageController *const Controller,
                                             const size_t Id) {
    struct GraphStorage Storage;
    fetchData(Controller->Allocator, Controller->StorageAddr, sizeof(Storage), &Storage);
    struct OptionalFullAddr NodeLinkAddr = Storage.Links;
    while (NodeLinkAddr.HasValue) {
        struct NodeLink Link;
        fetchData(Controller->Allocator, NodeLinkAddr, sizeof(Link), &Link);
        if (Link.Id == Id && !Link.Deleted) {
            return NodeLinkAddr;
        }
        NodeLinkAddr = Link.Next;
    }
    return NULL_FULL_ADDR;
}

struct OptionalFullAddr findNodeAddrById(const struct StorageController *const Controller,
                                         const struct OptionalFullAddr SchemeAddr, size_t Id) {
    struct Scheme Scheme;
    fetchData(Controller->Allocator, SchemeAddr, sizeof(Scheme), &Scheme);
    struct OptionalFullAddr NodeAddr = Scheme.Nodes;
    while (NodeAddr.HasValue) {
        struct Node Node;
        fetchData(Controller->Allocator, NodeAddr, sizeof(Node), &Node);
        if (Node.Id == Id && !Node.Deleted) {
            return NodeAddr;
        }
        if (!isOptionalFullAddrsEq(NodeAddr, Scheme.LastNode)) {
            NodeAddr = Node.Next;
        } else
            break;
    }
    return NULL_FULL_ADDR;
}