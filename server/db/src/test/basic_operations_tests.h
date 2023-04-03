#pragma once

void testStorageInit();

void testEmptySchemeCreationAndDelete();

void testSchemeAndNodeCreation();

void testTwoSchemesCreation();

void testTwoSchemesCreationWithReopen();

void testThreeNodesWithComplicatedData();

void testTwoLinkedNodes();

void testForceNodeVacuum();

void testForceLinkVacuum();

#define BASIC_OPERATION_TESTS_NUMBER 9

static void (*AllBasicOperationsTests[BASIC_OPERATION_TESTS_NUMBER])() = {
    testStorageInit,
    testEmptySchemeCreationAndDelete,
    testSchemeAndNodeCreation,
    testTwoSchemesCreation,
    testTwoSchemesCreationWithReopen,
    testThreeNodesWithComplicatedData,
    testTwoLinkedNodes,
    testForceNodeVacuum,
    testForceLinkVacuum};
