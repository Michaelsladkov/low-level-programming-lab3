#pragma once

#include "file_work/file_allocator.h"

void testSimpleAllocationWithFree();
void testFileReopen();
void testFileExtension();
void testFileExtensionReopen();
void checkFetchAndStore();
void checkFetchAndStoreWithReopen();

#define ALLOCATOR_TEST_NUMBER 6
static void (*AllAllocatorTests[ALLOCATOR_TEST_NUMBER])() = {
    testSimpleAllocationWithFree, testFileReopen,     testFileExtension,
    testFileExtensionReopen,      checkFetchAndStore, checkFetchAndStoreWithReopen};
