#ifndef MSLADKOV_LLP_LAB1_FIND_ELEMENT_H
#define MSLADKOV_LLP_LAB1_FIND_ELEMENT_H

#include "common_types.h"
#include "request.h"
#include "storage_controller.h"

struct OptionalFullAddr findSchemeAddrById(const struct StorageController *Controller,
                                          size_t Id);
struct OptionalFullAddr findSchemeAddrByName(const struct StorageController *Controller,
                                            const char *Name);
struct OptionalFullAddr findSchemeAddrBySchemeId(const struct StorageController *const Controller,
                                               const enum SchemeIdType IdType,
                                               const union SchemeId SchemeId);
size_t findNodesByFilters(const struct StorageController *const Controller,
                          const struct OptionalFullAddr SchemeAddr,
                          const struct AttributeFilter *AttributeFilterChain,
                          struct OptionalFullAddr **Result);
struct OptionalFullAddr findNodeAddrById(const struct StorageController *const Controller,
                                         const struct OptionalFullAddr SchemeAddr, size_t Id);
struct OptionalFullAddr findNodeLinkAddrById(const struct StorageController *const Controller,
                                             size_t Id);
size_t findNodeLinksByIdAndType(const struct StorageController *Controller,
                                const enum NodeLinkRequestType Type, const size_t Id,
                                struct OptionalFullAddr **Result);

#endif // MSLADKOV_LLP_LAB1_FIND_ELEMENT_H
