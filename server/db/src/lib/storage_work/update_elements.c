#include "file_work/file_allocator.h"
#include "find_element.h"
#include "graph_storage_interface.h"
#include "logical_structures.h"
#include "request.h"
#include "string_operations.h"

static void updateSingleNode(const struct StorageController *const Controller,
                             const struct OptionalFullAddr NodeAddr,
                             size_t UpdatedAttributesNumber,
                             struct ExternalAttribute *NewAttributes, struct Scheme *Scheme) {
    const size_t AttributesSize = sizeof(struct Attribute) * Scheme->AttributeNumber;
    struct Attribute *Attributes = malloc(AttributesSize);
    struct Node ToUpdate;
    fetchData(Controller->Allocator, NodeAddr, sizeof(ToUpdate), &ToUpdate);
    fetchData(Controller->Allocator, ToUpdate.Attributes, AttributesSize, Attributes);
    for (size_t i = 0; i < UpdatedAttributesNumber; ++i) {
        const size_t AttrId = NewAttributes[i].Id;
        if (Attributes[AttrId].Type == INT) {
            Attributes[AttrId].Value.IntValue = NewAttributes[i].Value.IntValue;
            continue;
        }
        if (Attributes[AttrId].Type == FLOAT) {
            Attributes[AttrId].Value.FloatValue = NewAttributes[i].Value.FloatValue;
            continue;
        }
        if (Attributes[AttrId].Type == BOOL) {
            Attributes[AttrId].Value.BoolValue = NewAttributes[i].Value.BoolValue;
            continue;
        }
        if (Attributes[AttrId].Type == STRING && NewAttributes[i].Type == STRING) {
            struct MyString NewString =
                createString(Controller, NewAttributes[i].Value.StringAddr);
            deleteString(Controller, Attributes[AttrId].Value.StringValue);
            Attributes[AttrId].Value.StringValue = NewString;
        }
    }
    storeData(Controller->Allocator, ToUpdate.Attributes, AttributesSize, Attributes);
    free(Attributes);
}

size_t updateNode(const struct StorageController *const Controller,
                  const struct UpdateNodeRequest *const Request) {
    const struct OptionalFullAddr SchemeAddr =
        findSchemeAddrBySchemeId(Controller, Request->SchemeIdType, Request->SchemeId);
    struct Scheme Scheme;
    fetchData(Controller->Allocator, SchemeAddr, sizeof(Scheme), &Scheme);
    if (Request->ById) {
        const struct OptionalFullAddr NodeAddr =
            findNodeAddrById(Controller, SchemeAddr, Request->Id);
        if (NodeAddr.HasValue) {
            updateSingleNode(Controller, NodeAddr, Request->UpdatedAttributesNumber,
                             Request->Attributes, &Scheme);
            return 1;
        }
        return 0;
    }
    struct OptionalFullAddr *NodesToUpdate;
    size_t NodesToUpdateCnt = findNodesByFilters(
        Controller, SchemeAddr, Request->AttributesFilterChain, &NodesToUpdate);
    for (size_t i = 0; i < NodesToUpdateCnt; ++i) {
        const struct OptionalFullAddr NodeAddr = NodesToUpdate[i];
        updateSingleNode(Controller, NodeAddr, Request->UpdatedAttributesNumber,
                         Request->Attributes, &Scheme);
    }
    free(NodesToUpdate);
    return NodesToUpdateCnt;
}

size_t updateNodeLink(const struct StorageController *const Controller,
                      const struct UpdateNodeLinkRequest *const Request) {
    if (!Request->UpdateType && !Request->UpdateWeight) {
        return 0;
    }
    const struct OptionalFullAddr NodeLinkAddr =
        findNodeLinkAddrById(Controller, Request->Id);
    struct NodeLink ToUpdate;
    fetchData(Controller->Allocator, NodeLinkAddr, sizeof(ToUpdate), &ToUpdate);
    ToUpdate.Type = Request->UpdateType ? Request->Type : ToUpdate.Type;
    ToUpdate.Weight = Request->UpdateWeight ? Request->Weight : ToUpdate.Weight;
    storeData(Controller->Allocator, NodeLinkAddr, sizeof(ToUpdate), &ToUpdate);
    return 1;
}
