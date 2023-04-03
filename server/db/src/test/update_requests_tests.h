#pragma once

void testUpdateOneNode();

void testUpdateNodeWithString();

void testUpdateSetOfNodes();

void testUpdateLink();

#define UPDATE_REQUESTS_TESTS_NUMBER 4

static void (*AllUpdateRequestsTests[UPDATE_REQUESTS_TESTS_NUMBER])() = {
    testUpdateOneNode, testUpdateNodeWithString, testUpdateSetOfNodes, testUpdateLink};