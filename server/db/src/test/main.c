#include "file_work/file_allocator.h"
#include "allocator_tests.h"
#include "basic_operations_tests.h"
#include "read_requests_tests.h"
#include "update_requests_tests.h"

int main() {
    for (int i = 0; i < ALLOCATOR_TEST_NUMBER; ++i) {
        AllAllocatorTests[i]();
    }
    for (int i = 0; i < BASIC_OPERATION_TESTS_NUMBER; ++i) {
        AllBasicOperationsTests[i]();
    }
    for (int i = 0; i < READ_REQUESTS_TESTS_NUMBER; ++i) {
        AllReadRequestsTests[i]();
    }
    for (int i = 0; i < UPDATE_REQUESTS_TESTS_NUMBER; ++i) {
        AllUpdateRequestsTests[i]();
    }
    return 0;
}