#include "allocator_tests.h"

#include "file_work/file_allocator_internals.h"

#include <assert.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>

static bool isHeadersEqual(const struct BlockHeader * const Header1, const struct BlockHeader * const Header2) {
    return Header1->IsOccupied == Header2->IsOccupied &&
           Header1->FullSize == Header2->FullSize &&
           Header1->DataSize == Header2->DataSize &&
           Header1->NextBlockOffset.HasValue == Header2->NextBlockOffset.HasValue &&
           Header1->NextBlockOffset.Offset == Header2->NextBlockOffset.Offset &&
           Header1->PrevBlockOffset.HasValue == Header2->PrevBlockOffset.HasValue &&
           Header1->PrevBlockOffset.Offset == Header2->PrevBlockOffset.Offset;
}

void testSimpleAllocationWithFree() {
    fprintf(stderr, "testSimpleAllocationWithFree started\n");
    struct FileAllocator* const Allocator = initFileAllocator("test.bin");
    struct BlockHeader *Headers;
    debugReadAllHeaders(Allocator, &Headers);
    const size_t HeadersNumberAtInit = debugGetHeadersNumber(Allocator);
    fprintf(stderr, "Headers number after init: %lu. 1 expected\n", HeadersNumberAtInit);
    assert(HeadersNumberAtInit == 1);
    assert(Headers[0].IsOccupied == false);
    free(Headers);
    fprintf(stderr, "Allocating three small blocks\n");
    struct OptionalFullAddr Addr1 = allocate(Allocator, 100);
    struct OptionalFullAddr Addr2 = allocate(Allocator, 100);
    struct OptionalFullAddr Addr3 = allocate(Allocator, 100);
    debugReadAllHeaders(Allocator, &Headers);
    bool ThreeHeadersOccupied = true;
    bool SizeIsEnough = true;
    const size_t HeadersNumberAfterAlloc = debugGetHeadersNumber(Allocator);
    fprintf(stderr, "Number of headers after allocation of 3 regions: %lu. 4 expected\n", HeadersNumberAfterAlloc);
    fprintf(stderr, "Check three headers are occupied and have enough size\n");
    for (size_t i = 0; i < HeadersNumberAfterAlloc - 1; ++i) {
        struct BlockHeader Header = Headers[i];
        ThreeHeadersOccupied &= Header.IsOccupied;
        SizeIsEnough &= Header.DataSize >= 100;
    }
    assert(ThreeHeadersOccupied);
    assert(SizeIsEnough);
    fprintf(stderr, "Test last header is free\n");
    assert(!Headers[HeadersNumberAfterAlloc - 1].IsOccupied);
    free(Headers);
    fprintf(stderr, "Deallocating 3 regions\n");
    deallocate(Allocator, Addr3);
    deallocate(Allocator, Addr2);
    deallocate(Allocator, Addr1);
    const size_t HeadersNumberAfterDeallocate = debugGetHeadersNumber(Allocator);
    debugReadAllHeaders(Allocator, &Headers);
    fprintf(stderr, "Check all blocks are free\n");
    for (size_t i = 0; i < HeadersNumberAfterDeallocate; ++i) {
        assert(!Headers[i].IsOccupied);
    }
    free(Headers);
    shutdownFileAllocator(Allocator);
    fprintf(stderr, "testSimpleAllocationWithFree finished\n\n");
}

void testFileReopen() {
    fprintf(stderr, "testFileReopen started\n");
    fprintf(stderr, "Allocating three blocks and closing file\n");
    struct FileAllocator* Allocator = initFileAllocator("test.bin");
    struct OptionalFullAddr Addr1 =  allocate(Allocator, 512);
    struct OptionalFullAddr Addr2 =  allocate(Allocator, 512);
    struct OptionalFullAddr Addr3 =  allocate(Allocator, 512);
    struct BlockHeader *HeadersBefore;
    debugReadAllHeaders(Allocator, &HeadersBefore);
    debugPrintFile(Allocator);
    shutdownFileAllocator(Allocator);
    fprintf(stderr, "File closed\n");

    fprintf(stderr, "Reopening file\n");
    Allocator = initFileAllocator("test.bin");
    struct BlockHeader *HeadersAfter;
    debugReadAllHeaders(Allocator, &HeadersAfter);
    debugPrintFile(Allocator);
    for (size_t i = 0; i < debugGetHeadersNumber(Allocator); i++) {
        fprintf(stderr, "Check block %lu\n", i);
        assert(isHeadersEqual(&HeadersBefore[i], &HeadersAfter[i]));
    }
    free(HeadersBefore);
    free(HeadersAfter);
    deallocate(Allocator, Addr1);
    deallocate(Allocator, Addr2);
    deallocate(Allocator, Addr3);
    shutdownFileAllocator(Allocator);
    fprintf(stderr, "testFileReopen finished\n\n");
}

void testFileExtension() {
    fprintf(stderr, "testFileExtension started\n");
    struct FileAllocator* const Allocator = initFileAllocator("test.bin");
    fprintf(stderr, "Allocating 1 small block\n");
    struct OptionalFullAddr Addr1 = allocate(Allocator, 512);
    fprintf(stderr, "Allocating 1 big block\n");
    struct OptionalFullAddr Addr2 = allocate(Allocator, INITIAL_FILE_SIZE);
    debugPrintFile(Allocator);
    struct BlockHeader * Headers;
    debugReadAllHeaders(Allocator, &Headers);
    const size_t HeadersNumber = debugGetHeadersNumber(Allocator);
    fprintf(stderr, "Number of blocks: %lu. 3 expected\n", HeadersNumber);
    assert(HeadersNumber == 3);
    fprintf(stderr, "Check first two are occupied\n");
    assert(Headers[0].IsOccupied);
    assert(Headers[1].IsOccupied);
    fprintf(stderr, "Check sizes are enough\n");
    assert(Headers[0].DataSize >= 512);
    assert(Headers[1].DataSize >= INITIAL_FILE_SIZE);
    deallocate(Allocator, Addr1);
    deallocate(Allocator, Addr2);
    fprintf(stderr, "After deallocating blocks\n");
    debugPrintFile(Allocator);
    shutdownFileAllocator(Allocator);
    free(Headers);
    fprintf(stderr, "testFileExtension finished\n\n");
}

void testFileExtensionReopen() {
    fprintf(stderr, "testFileExtensionReopen started\n");
    struct FileAllocator* Allocator = initFileAllocator("test.bin");
    fprintf(stderr, "Allocating 1 small block\n");
    struct OptionalFullAddr Addr1 = allocate(Allocator, 512);
    fprintf(stderr, "Allocating 1 big block\n");
    struct OptionalFullAddr Addr2 = allocate(Allocator, 16384);
    debugPrintFile(Allocator);
    struct BlockHeader * HeadersBefore;
    debugReadAllHeaders(Allocator, &HeadersBefore);
    fprintf(stderr, "Closing file\n");
    shutdownFileAllocator(Allocator);

    fprintf(stderr, "Reopening file\n");
    Allocator = initFileAllocator("test.bin");
    struct BlockHeader *HeadersAfter;
    debugReadAllHeaders(Allocator, &HeadersAfter);
    debugPrintFile(Allocator);
    for (size_t i = 0; i < debugGetHeadersNumber(Allocator); i++) {
        fprintf(stderr, "Check block %lu\n", i);
        assert(isHeadersEqual(&HeadersBefore[i], &HeadersAfter[i]));
    }
    free(HeadersBefore);
    free(HeadersAfter);
    deallocate(Allocator, Addr1);
    deallocate(Allocator, Addr2);
    shutdownFileAllocator(Allocator);
    fprintf(stderr, "testFileExtensionReopen finished\n\n");
}

void checkFetchAndStore() {
    fprintf(stderr, "checkFetchAndStore started\n");
    struct FileAllocator* const Allocator = initFileAllocator("test.bin");
    fprintf(stderr, "Allocating 1 small block\n");
    struct OptionalFullAddr Addr1 = allocate(Allocator, 512);
    fprintf(stderr, "storing data: 0xDEADBEEF\n");
    uint64_t StoredData = 0xDEADBEEF;
    storeData(Allocator, Addr1, sizeof(StoredData), &StoredData);
    fprintf(stderr, "fetching data\n");
    uint64_t FetchedData;
    fetchData(Allocator, Addr1, sizeof(FetchedData), &FetchedData);
    fprintf(stderr, "Stored: %lx, Fetched: %lx\n", StoredData, FetchedData);
    assert(FetchedData == StoredData);
    deallocate(Allocator, Addr1);
    shutdownFileAllocator(Allocator);
    fprintf(stderr, "checkFetchAndStore finished\n\n");
}

void checkFetchAndStoreWithReopen() {
    fprintf(stderr, "checkFetchAndStoreWithReopen started\n");
    struct FileAllocator* Allocator = initFileAllocator("test.bin");
    fprintf(stderr, "Allocating 1 small block\n");
    struct OptionalFullAddr Addr1 = allocate(Allocator, 512);
    fprintf(stderr, "storing data: 0xDEADBEEF\n");
    uint64_t StoredData = 0xDEADBEEF;
    storeData(Allocator, Addr1, sizeof(StoredData), &StoredData);
    fprintf(stderr, "Closing File\n");
    shutdownFileAllocator(Allocator);

    fprintf(stderr, "Reopening file\n");
    Allocator = initFileAllocator("test.bin");
    fprintf(stderr, "fetching data\n");
    uint64_t FetchedData;
    fetchData(Allocator, Addr1, sizeof(FetchedData), &FetchedData);
    fprintf(stderr, "Stored: %lx, Fetched: %lx\n", StoredData, FetchedData);
    assert(FetchedData == StoredData);
    deallocate(Allocator, Addr1);
    shutdownFileAllocator(Allocator);
    fprintf(stderr, "checkFetchAndStoreWithReopen finished\n\n");
}