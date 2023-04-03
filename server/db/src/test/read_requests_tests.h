#pragma once

void testReadEmptyScheme();

void testReadEmptySchemeWithNodes();

void testReadFourAttrScheme();

void testReadNodes();

void testReadNodesAfterDelete();

void testReadLinks();

void testReadLinksAfterDelete();

void testReadLinksAfterNodeDelete();

void testReadNodesAndLinksWithReopen();

#define READ_REQUESTS_TESTS_NUMBER 9

static void (*AllReadRequestsTests[READ_REQUESTS_TESTS_NUMBER])() = {
    testReadEmptyScheme,
    testReadEmptySchemeWithNodes,
    testReadFourAttrScheme,
    testReadNodes,
    testReadNodesAfterDelete,
    testReadLinks,
    testReadLinksAfterDelete,
    testReadLinksAfterNodeDelete,
    testReadNodesAndLinksWithReopen};
