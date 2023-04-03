#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include "file_allocator.h"
#include "params.h"

struct BlockHeader {
    bool IsOccupied;
    size_t FullSize;
    size_t DataSize;
    struct OptionalOffset NextBlockOffset;
    struct OptionalOffset PrevBlockOffset;
};

struct Block {
    struct BlockHeader header;
    char data[];
};

void debugPrintFile(const struct FileAllocator *const Allocator);

/**
 * @brief Returns array of all block headers allocated in the file
 *
 * @return struct BlockHeader*
 *
 * @note The array is allocated on the heap and must be freed by the caller
 */
void debugReadAllHeaders(const struct FileAllocator *const Allocator,
                         struct BlockHeader **headers);

/**
 * @brief Returns number of blocks allocator works with
 */
size_t debugGetHeadersNumber(const struct FileAllocator *const Allocator);
size_t getFileSize(const struct FileAllocator *const Allocator);