#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "common_types.h"

struct FileAllocator;

struct OptionalOffset {
    bool HasValue;
    size_t Offset;
};

#define NULL_OFFSET                                                                            \
    (struct OptionalOffset) { false, 0 }

static inline struct OptionalOffset getOptionalOffset(size_t Offset) {
    return (struct OptionalOffset){true, Offset};
}

struct FileAllocator *initFileAllocator(char *FileName);
void shutdownFileAllocator(struct FileAllocator *Allocator);
void dropFileAllocator(struct FileAllocator *Allocator);
struct OptionalFullAddr allocate(struct FileAllocator *const Allocator, size_t Size);
struct OptionalFullAddr getFirstBlockData(const struct FileAllocator *const Allocator);
void deallocate(const struct FileAllocator *const Allocator, struct OptionalFullAddr Addr);
int fetchData(const struct FileAllocator *const Allocator, const struct OptionalFullAddr Addr,
              const size_t Size, void *const Buffer);
int storeData(const struct FileAllocator *const Allocator, const struct OptionalFullAddr Addr,
              const size_t Size, const void *const Buffer);
