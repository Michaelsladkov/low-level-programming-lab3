#pragma once

#include <malloc.h>

#include "file_work/file_allocator.h"
#include "logical_structures.h"

struct StorageController {
    struct FileAllocator *Allocator;
    struct GraphStorage Storage;
    struct OptionalFullAddr StorageAddr;
};

size_t increaseSchemeNumber(struct StorageController *Controller);
size_t decreaseSchemeNumber(struct StorageController *Controller);
size_t increaseNodeNumber(struct StorageController *Controller);
size_t increaseNodeLinkNumber(struct StorageController *Controller);
void updateLastScheme(struct StorageController *const Controller,
                     struct OptionalFullAddr LastSchemeAddr);
void updateFirstScheme(struct StorageController *const Controller,
                      struct OptionalFullAddr FirstSchemeAddr);