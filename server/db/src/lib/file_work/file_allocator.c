#include <errno.h>
#include <inttypes.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>

#include "file_allocator_internals.h"

struct FileAllocator {
    FILE *File;
    size_t FileSize;
};

#define FileSignature 0x67683C2D14C1B291
#define FIRST_BLOCK_OFFSET sizeof(uint64_t)

static void blockInit(const struct FileAllocator *const Allocator, size_t Offset,
                      size_t FullSize, struct OptionalOffset NextBlockOffset,
                      struct OptionalOffset PrevBlockOffset) {
    struct BlockHeader Header = {.IsOccupied = false,
                                 .FullSize = FullSize,
                                 .DataSize = FullSize - sizeof(struct BlockHeader),
                                 .NextBlockOffset = NextBlockOffset,
                                 .PrevBlockOffset = PrevBlockOffset};
    fseek(Allocator->File, Offset, SEEK_SET);
    fwrite(&Header, sizeof(Header), 1, Allocator->File);
    fseek(Allocator->File, Offset + FullSize - 1, SEEK_SET);
    fputc(0, Allocator->File);
}

static void initEmptyFile(const struct FileAllocator *const Allocator) {
    uint64_t Signature = FileSignature;
    fwrite(&Signature, sizeof(Signature), 1,
           Allocator->File); // Write Signature
    fseek(Allocator->File, sizeof(Signature), SEEK_SET);
    blockInit(Allocator, sizeof(Signature), INITIAL_FILE_SIZE - sizeof(Signature), NULL_OFFSET,
              NULL_OFFSET); // Initialize first block
    size_t *ZeroData = memset(malloc(INITIAL_FILE_SIZE), 0, INITIAL_FILE_SIZE);
    size_t zeroes_written = fwrite(
        ZeroData, sizeof(size_t),
        (INITIAL_FILE_SIZE - sizeof(Signature) - sizeof(struct BlockHeader)) / sizeof(size_t),
        Allocator->File);
    free(ZeroData);
}

static void extendFile(struct FileAllocator *const Allocator,
                       struct OptionalOffset LastBlockOffset) {
    struct BlockHeader OldLastBlock;
    fseek(Allocator->File, LastBlockOffset.Offset, SEEK_SET);
    fread(&OldLastBlock, sizeof(OldLastBlock), 1, Allocator->File);
    const size_t EndOfLastBlock = LastBlockOffset.Offset + OldLastBlock.FullSize;
    blockInit(Allocator, EndOfLastBlock, Allocator->FileSize, NULL_OFFSET, LastBlockOffset);
    OldLastBlock.NextBlockOffset = getOptionalOffset(EndOfLastBlock);
    fseek(Allocator->File, LastBlockOffset.Offset, SEEK_SET);
    fwrite(&OldLastBlock, sizeof(OldLastBlock), 1, Allocator->File);
    Allocator->FileSize *= 2;
}

static void mergeWhilePossible(const struct FileAllocator *const Allocator,
                               const size_t Offset) {
    struct BlockHeader Header;
    fseek(Allocator->File, Offset, SEEK_SET);
    fread(&Header, sizeof(Header), 1, Allocator->File);
    if (Header.IsOccupied) {
        return;
    }
    if (Header.NextBlockOffset.HasValue) {
        struct BlockHeader NextHeader;
        fseek(Allocator->File, Header.NextBlockOffset.Offset, SEEK_SET);
        fread(&NextHeader, sizeof(NextHeader), 1, Allocator->File);
        if (!NextHeader.IsOccupied) {
            blockInit(Allocator, Offset, Header.FullSize + NextHeader.FullSize,
                      NextHeader.NextBlockOffset, Header.PrevBlockOffset);
            mergeWhilePossible(Allocator, Offset);
        }
    }
    fseek(Allocator->File, Offset, SEEK_SET);
}

static void mergeAllPossible(const struct FileAllocator *const Allocator) {
    mergeWhilePossible(Allocator, FIRST_BLOCK_OFFSET);
    fseek(Allocator->File, FIRST_BLOCK_OFFSET, SEEK_SET);
    struct BlockHeader Header;
    fread(&Header, sizeof(struct BlockHeader), 1, Allocator->File);
    while (Header.NextBlockOffset.HasValue) {
        mergeWhilePossible(Allocator, Header.NextBlockOffset.Offset);
        fseek(Allocator->File, Header.NextBlockOffset.Offset, SEEK_SET);
        fread(&Header, sizeof(struct BlockHeader), 1, Allocator->File);
    }
}

static size_t getHeadersNumber(const struct FileAllocator *const Allocator) {
    size_t HeadersNumber = 0;
    struct BlockHeader Header;
    fseek(Allocator->File, FIRST_BLOCK_OFFSET, SEEK_SET);
    fread(&Header, sizeof(Header), 1, Allocator->File);
    while (Header.NextBlockOffset.HasValue) {
        HeadersNumber++;
        fseek(Allocator->File, Header.NextBlockOffset.Offset, SEEK_SET);
        fread(&Header, sizeof(Header), 1, Allocator->File);
    }
    HeadersNumber++;
    return HeadersNumber;
}

static void readAllHeaders(const struct FileAllocator *const Allocator,
                           struct BlockHeader **Headers) {
    const size_t HeadersNumber = getHeadersNumber(Allocator);
    *Headers = malloc(HeadersNumber * sizeof(struct BlockHeader));
    fseek(Allocator->File, FIRST_BLOCK_OFFSET, SEEK_SET);
    for (size_t i = 0; i < HeadersNumber; i++) {
        fread(&(*Headers)[i], sizeof(struct BlockHeader), 1, Allocator->File);
        fseek(Allocator->File, (*Headers)[i].NextBlockOffset.Offset, SEEK_SET);
    }
}

struct FileAllocator *initFileAllocator(char *fileName) {
    struct FileAllocator *const Allocator = malloc(sizeof(struct FileAllocator));
    Allocator->File = fopen(fileName, "r+b");
    if (Allocator->File == NULL) {
        Allocator->File = fopen(fileName, "w+b");
        if (Allocator->File == NULL) {
            return NULL;
        }
    }
    uint64_t Signature;
    int ReadResult = fread(&Signature, sizeof(Signature), 1, Allocator->File);
    if (ReadResult != 1) {
        Allocator->FileSize = ftell(Allocator->File);
        initEmptyFile(Allocator);
    } else if (Signature != FileSignature) {
        return NULL;
    }
    // We need to know the size of the file
    fseek(Allocator->File, 0, SEEK_END);          // Go to the end of the file
    Allocator->FileSize = ftell(Allocator->File); // Get the current position in the file
    mergeAllPossible(Allocator);
    return Allocator;
}

void shutdownFileAllocator(struct FileAllocator *Allocator) {
    fclose(Allocator->File);
    free(Allocator);
}

static bool blockSplittable(struct BlockHeader *Header, const size_t DataSize) {
    return !Header->IsOccupied &&
           Header->FullSize >= DataSize + 2 * sizeof(struct BlockHeader) + BLOCK_MIN_CAPACITY;
}

static void splitIfTooBig(const struct FileAllocator *const Allocator, const size_t BlockOffset,
                          const size_t DataSize) {
    struct BlockHeader OldHeader;
    fseek(Allocator->File, BlockOffset, SEEK_SET);
    fread(&OldHeader, sizeof(OldHeader), 1, Allocator->File);
    if (blockSplittable(&OldHeader, DataSize)) {
        blockInit(Allocator, BlockOffset, DataSize + sizeof(struct BlockHeader),
                  getOptionalOffset(BlockOffset + DataSize + sizeof(struct BlockHeader)),
                  OldHeader.PrevBlockOffset);
        blockInit(Allocator, BlockOffset + DataSize + sizeof(struct BlockHeader),
                  OldHeader.FullSize - DataSize - sizeof(struct BlockHeader),
                  OldHeader.NextBlockOffset, getOptionalOffset(BlockOffset));
    }
}

struct SearchResult {
    size_t Offset;
    bool Found;
};

// Returns the offset of the first block that is not occupied and has enough
// space for the data If no such block is found, returns the offset of the last
// block
static struct SearchResult findBlock(const struct FileAllocator *const Allocator,
                                     const size_t DataSize) {
    struct SearchResult Result = {0, false};
    struct BlockHeader Header;
    size_t CurrentOffset = FIRST_BLOCK_OFFSET;
    fseek(Allocator->File, CurrentOffset, SEEK_SET);
    do {
        fread(&Header, sizeof(Header), 1, Allocator->File);
        if (!Header.IsOccupied) {
            mergeWhilePossible(Allocator, ftell(Allocator->File) - sizeof(Header));
            fread(&Header, sizeof(Header), 1, Allocator->File);
        }
        if (!Header.IsOccupied && Header.DataSize >= DataSize) {
            Result.Offset = ftell(Allocator->File) - sizeof(struct BlockHeader);
            Result.Found = true;
            break;
        }
        if (Header.NextBlockOffset.HasValue) {
            CurrentOffset = Header.NextBlockOffset.Offset;
        }
        fseek(Allocator->File, CurrentOffset, SEEK_SET);
    } while (Header.NextBlockOffset.HasValue);
    if (!Result.Found) {
        Result.Offset = CurrentOffset;
    }
    return Result;
}

static size_t getRealDataSize(size_t DataSize) {
    size_t FullBlocks = DataSize / BLOCK_MIN_CAPACITY;
    FullBlocks += ((DataSize % BLOCK_MIN_CAPACITY)) == 0 ? 0 : 1;
    return FullBlocks * BLOCK_MIN_CAPACITY;
}

struct OptionalFullAddr allocate(struct FileAllocator *const Allocator, const size_t DataSize) {
    if (DataSize == 0) {
        return NULL_FULL_ADDR;
    }
    const size_t RealDataSize = getRealDataSize(DataSize);
    struct SearchResult SearchResult = findBlock(Allocator, RealDataSize);
    while (!SearchResult.Found) {
        extendFile(Allocator, getOptionalOffset(SearchResult.Offset));
        SearchResult = findBlock(Allocator, RealDataSize);
    }
    splitIfTooBig(Allocator, SearchResult.Offset, RealDataSize);
    struct BlockHeader Header;
    fseek(Allocator->File, SearchResult.Offset, SEEK_SET);
    fread(&Header, sizeof(Header), 1, Allocator->File);
    Header.IsOccupied = true;
    fseek(Allocator->File, SearchResult.Offset, SEEK_SET);
    fwrite(&Header, sizeof(Header), 1, Allocator->File);
    return getOptionalFullAddr(SearchResult.Offset, sizeof(Header));
}

void deallocate(const struct FileAllocator *const Allocator,
                const struct OptionalFullAddr Addr) {
    if (!Addr.HasValue) {
        return;
    }
    struct BlockHeader Header;
    const size_t BlockOffset = Addr.BlockOffset;
    fseek(Allocator->File, BlockOffset, SEEK_SET);
    fread(&Header, sizeof(Header), 1, Allocator->File);
    Header.IsOccupied = false;
    fseek(Allocator->File, BlockOffset, SEEK_SET);
    fwrite(&Header, sizeof(Header), 1, Allocator->File);
    mergeWhilePossible(Allocator, BlockOffset);
}

static void printHeaderInfo(const struct FileAllocator *const Allocator,
                            struct BlockHeader *Header) {
    fprintf(stderr,
            "Block at %lu, size %lu, data size %lu, occupied %d, next %lu, prev "
            "%lu\n",
            ftell(Allocator->File) - sizeof(struct BlockHeader), Header->FullSize,
            Header->DataSize, Header->IsOccupied, Header->NextBlockOffset.Offset,
            Header->PrevBlockOffset.Offset);
}

void debugPrintFile(const struct FileAllocator *const Allocator) {
    struct BlockHeader Header;
    fseek(Allocator->File, FIRST_BLOCK_OFFSET, SEEK_SET);
    fread(&Header, sizeof(Header), 1, Allocator->File);
    while (Header.NextBlockOffset.HasValue) {
        printHeaderInfo(Allocator, &Header);
        fseek(Allocator->File, Header.NextBlockOffset.Offset, SEEK_SET);
        fread(&Header, sizeof(Header), 1, Allocator->File);
    }
    printHeaderInfo(Allocator, &Header);
}

struct OptionalFullAddr getFirstBlockData(const struct FileAllocator *const Allocator) {
    struct BlockHeader Header;
    fseek(Allocator->File, FIRST_BLOCK_OFFSET, SEEK_SET);
    fread(&Header, sizeof(Header), 1, Allocator->File);
    if (!Header.IsOccupied) {
        return NULL_FULL_ADDR;
    }
    return getOptionalFullAddr(FIRST_BLOCK_OFFSET, sizeof(struct BlockHeader));
}

size_t debugGetHeadersNumber(const struct FileAllocator *const Allocator) {
    return getHeadersNumber(Allocator);
}

void debugReadAllHeaders(const struct FileAllocator *const Allocator,
                         struct BlockHeader **Headers) {
    readAllHeaders(Allocator, Headers);
}

int fetchData(const struct FileAllocator *const Allocator, const struct OptionalFullAddr Addr,
              const size_t Size, void *const Buffer) {
    fseek(Allocator->File, Addr.BlockOffset + Addr.DataOffset, SEEK_SET);
    return fread(Buffer, Size, 1, Allocator->File);
}

int storeData(const struct FileAllocator *const Allocator, const struct OptionalFullAddr Addr,
              const size_t Size, const void *const Buffer) {
    if (!Addr.HasValue) {
        return -1;
    }
    fseek(Allocator->File, Addr.BlockOffset + Addr.DataOffset, SEEK_SET);
    return fwrite(Buffer, 1, Size, Allocator->File);
}

size_t getFileSize(const struct FileAllocator *const Allocator) {
    return Allocator->FileSize;
}
