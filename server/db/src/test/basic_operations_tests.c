#include "basic_operations_tests.h"

#include "storage_work/graph_storage_interface.h"
#include "storage_work/storage_controller.h"
#include "file_work/file_allocator_internals.h"
#include "params.h"

#include <assert.h>
#include <stdio.h>

void testStorageInit() {
    fprintf(stderr, "testStorageInit started\n");
    fprintf(stderr, "Creating storage\n");
    struct StorageController* Controller = beginWork("test.bin");
    assert(Controller != NULL);
    endWork(Controller);
    fprintf(stderr, "testStorageInit finished\n\n");
}

void testEmptySchemeCreationAndDelete() {
    fprintf(stderr, "testEmptySchemeCreationAndDelete started\n");
    fprintf(stderr, "Creating storage\n");
    struct StorageController* Controller = beginWork("test.bin");
    assert(Controller != NULL);
    struct CreateSchemeRequest CreateRequest;
    CreateRequest.AttributesDescription = NULL;
    CreateRequest.Name = "TestEmptyScheme";
    debugPrintFile(Controller->Allocator);
    fprintf(stderr, "Creating Scheme\n");
    size_t Created = createScheme(Controller, &CreateRequest);
    fprintf(stderr, "Created Schemes number %zu, 1 expected\n", Created);
    assert(Created != 0);
    assert(Controller->Storage.SchemeNumber == 1);
    assert(Controller->Storage.LastScheme.HasValue);
    assert(Controller->Storage.Schemes.HasValue);
    debugPrintFile(Controller->Allocator);
    struct DeleteSchemeRequest DeleteRequest;
    DeleteRequest.Name = "TestEmptyScheme";
    size_t Deleted = deleteScheme(Controller, &DeleteRequest);
    fprintf(stderr, "Deleted Schemes number %zu, 1 expected\n", Deleted);
    assert(Deleted == 1);
    assert(Controller->Storage.SchemeNumber == 0);
    assert(!Controller->Storage.LastScheme.HasValue);
    assert(!Controller->Storage.Schemes.HasValue);
    debugPrintFile(Controller->Allocator);
    endWork(Controller);
    fprintf(stderr, "testEmptySchemeCreationAndDelete finished\n\n");
}

void testSchemeAndNodeCreation() {
    fprintf(stderr, "testSchemeAndNodeCreation started\n");
    fprintf(stderr, "Creating storage\n");
    struct StorageController* Controller = beginWork("test.bin");
    assert(Controller != NULL);
    debugPrintFile(Controller->Allocator);
    struct CreateSchemeRequest CreateSchemeRequest;
    CreateSchemeRequest.Name = "ALPHA";
    struct ExternalAttributeDescription Description;
    Description.Name = "STR";
    Description.Next = NULL;
    Description.Type = STRING;
    Description.AttributeId = 0;
    CreateSchemeRequest.AttributesDescription = &Description;
    fprintf(stderr, "Creating Scheme\n");
    size_t Created = createScheme(Controller, &CreateSchemeRequest);
    fprintf(stderr, "Created Schemes number %zu, 1 expected\n", Created);
    assert(Created != 0);
    debugPrintFile(Controller->Allocator);
    struct CreateNodeRequest CreateNodeRequest;
    CreateNodeRequest.SchemeIdType = Scheme_NAME;
    CreateNodeRequest.SchemeId.SchemeName = "ALPHA";
    struct ExternalAttribute attribute;
    attribute.Type = STRING;
    attribute.Id = 0;
    attribute.Value.StringAddr = "aboba";
    CreateNodeRequest.Attributes = &attribute;
    fprintf(stderr, "Creating Node\n");
    Created = createNode(Controller, &CreateNodeRequest);
    fprintf(stderr, "Created Nodes number %zu, 1 expected\n", Created);
    assert(Created != 0);
    debugPrintFile(Controller->Allocator);
    struct DeleteSchemeRequest DeleteRequest;
    DeleteRequest.Name = "ALPHA";
    size_t Deleted = deleteScheme(Controller, &DeleteRequest);
    assert(Deleted != 0);
    debugPrintFile(Controller->Allocator);
    endWork(Controller);
    fprintf(stderr, "testSchemeAndNodeCreation finished\n");
}

void testTwoSchemesCreation() {
    fprintf(stderr, "testTwoSchemesCreation started\n");
    fprintf(stderr, "Creating storage\n");
    struct StorageController* Controller = beginWork("test.bin");
    assert(Controller != NULL);
    debugPrintFile(Controller->Allocator);
    struct CreateSchemeRequest Request1, Request2;
    Request1.AttributesDescription = NULL;
    Request1.Name = "TestEmptyScheme1";
    Request2.AttributesDescription = NULL;
    Request2.Name = "TestEmptyScheme2";
    fprintf(stderr, "Creating Scheme\n");
    size_t Created = createScheme(Controller, &Request1);
    fprintf(stderr, "Creating Scheme\n");
    assert(Created != 0);
    Created = createScheme(Controller, &Request2);
    assert(Created != 0);
    size_t TotalSchemes = Controller->Storage.SchemeNumber;
    fprintf(stderr, "Created Schemes number %zu, 2 expected\n", TotalSchemes);
    assert(TotalSchemes == 2);
    struct DeleteSchemeRequest DeleteRequest;
    size_t Deleted = 0;
    fprintf(stderr, "Deleting Schemes\n");
    DeleteRequest.Name = "TestEmptyScheme1";
    Deleted += deleteScheme(Controller, &DeleteRequest);
    DeleteRequest.Name = "TestEmptyScheme2";
    Deleted += deleteScheme(Controller, &DeleteRequest);
    assert(Deleted == 2);
    endWork(Controller);
    fprintf(stderr, "testTwoSchemesCreation finished\n\n");
}

void testTwoSchemesCreationWithReopen() {
    fprintf(stderr, "testTwoSchemesCreationWithReopen started\n");
    fprintf(stderr, "Creating storage\n");
    struct StorageController* Controller = beginWork("test.bin");
    assert(Controller != NULL);
    debugPrintFile(Controller->Allocator);
    struct CreateSchemeRequest CreateRequest;
    CreateRequest.AttributesDescription = NULL;
    CreateRequest.Name = "TestEmptyScheme1";
    fprintf(stderr, "Creating Scheme\n");
    size_t Created = createScheme(Controller, &CreateRequest);
    fprintf(stderr, "Creating Scheme\n");
    assert(Created != 0);
    CreateRequest.Name = "TestEmptyScheme2";
    Created = createScheme(Controller, &CreateRequest);
    assert(Created != 0);
    size_t TotalSchemes = Controller->Storage.SchemeNumber;
    fprintf(stderr, "Created Schemes number %zu, 2 expected\n", TotalSchemes);
    assert(TotalSchemes == 2);
    endWork(Controller);
    fprintf(stderr, "Storage destroyed\n");

    fprintf(stderr, "Opening storage again\n");
    Controller = beginWork("test.bin");
    assert(Controller != NULL);
    fprintf(stderr, "Checkinng Scheme number in reopened storage");
    TotalSchemes = Controller->Storage.SchemeNumber;
    fprintf(stderr, "Total Schemes number %zu, 2 expected\n", TotalSchemes);
    assert(TotalSchemes == 2);
    struct DeleteSchemeRequest DeleteRequest;
    size_t Deleted = 0;
    fprintf(stderr, "Deleting Schemes\n");
    DeleteRequest.Name = "TestEmptyScheme1";
    Deleted += deleteScheme(Controller, &DeleteRequest);
    DeleteRequest.Name = "TestEmptyScheme2";
    Deleted += deleteScheme(Controller, &DeleteRequest);
    assert(Deleted == 2);
    debugPrintFile(Controller->Allocator);
    endWork(Controller);
    fprintf(stderr, "testTwoSchemesCreationWithReopen finished\n\n");
}

void testThreeNodesWithComplicatedData() {
    fprintf(stderr, "testThreeNodesWithComplicatedDataAndReopen started\n");
    struct StorageController *Controller = beginWork("test.bin");
    assert(Controller != NULL);
    struct CreateSchemeRequest CreateSchemeRequest;
    CreateSchemeRequest.Name = "MyScheme";
    struct ExternalAttributeDescription AttributesDescription[4];
    AttributesDescription[0].Type = STRING;
    AttributesDescription[0].Name = "NodeName";
    AttributesDescription[0].AttributeId = 0;
    AttributesDescription[0].Next = &AttributesDescription[1];
    AttributesDescription[1].Type = INT;
    AttributesDescription[1].Name = "NodeInt";
    AttributesDescription[1].AttributeId = 1;
    AttributesDescription[1].Next = &AttributesDescription[2];
    AttributesDescription[2].Type = FLOAT;
    AttributesDescription[2].Name = "NodeFloat";
    AttributesDescription[2].AttributeId = 2;
    AttributesDescription[2].Next = &AttributesDescription[3];
    AttributesDescription[3].Type = BOOL;
    AttributesDescription[3].Name = "NodeBool";
    AttributesDescription[3].AttributeId = 3;
    AttributesDescription[3].Next = NULL;
    CreateSchemeRequest.AttributesDescription = AttributesDescription;
    assert(createScheme(Controller, &CreateSchemeRequest) != 0);
    debugPrintFile(Controller->Allocator);
    struct CreateNodeRequest CreateNodeRequest;
    CreateNodeRequest.SchemeIdType = Scheme_NAME;
    CreateNodeRequest.SchemeId.SchemeName = "MyScheme";
    struct ExternalAttribute Attributes[4];
    Attributes[0].Type = STRING;
    Attributes[0].Id = 0;
    Attributes[0].Value.StringAddr = "Node 1";
    Attributes[1].Type = INT;
    Attributes[1].Id = 1;
    Attributes[1].Value.IntValue = 42;
    Attributes[2].Type = FLOAT;
    Attributes[2].Id = 2;
    Attributes[2].Value.FloatValue = 3.14;
    Attributes[3].Type = BOOL;
    Attributes[3].Id = 3;
    Attributes[3].Value.BoolValue = true;
    CreateNodeRequest.Attributes = Attributes;
    assert(createNode(Controller, &CreateNodeRequest) != 0);
    fprintf(stderr, "Node added\n");
    Attributes[0].Value.StringAddr = "Node 2";
    Attributes[1].Value.IntValue = 228;
    assert(createNode(Controller, &CreateNodeRequest) != 0);
    fprintf(stderr, "Node added\n");
    Attributes[0].Value.StringAddr = "Node 3";
    Attributes[1].Value.IntValue = 1488;
    Attributes[2].Value.FloatValue = 2.783;
    Attributes[3].Value.BoolValue = false;
    assert(createNode(Controller, &CreateNodeRequest) != 0);
    fprintf(stderr, "Node added\n");
    debugPrintFile(Controller->Allocator);
    struct DeleteSchemeRequest DeleteRequest;
    DeleteRequest.Name = "MyScheme";
    fprintf(stderr, "Deleting Scheme\n");
    assert(deleteScheme(Controller, &DeleteRequest) != 0);
    debugPrintFile(Controller->Allocator);
    endWork(Controller);
    fprintf(stderr, "testThreeNodesWithComplicatedDataAndReopen finished\n\n");
}

void testTwoLinkedNodes() {
    fprintf(stderr, "testTwoLinkedNodes started\n");
    struct StorageController *Controller = beginWork("test.bin");
    assert(Controller != NULL);
    struct CreateSchemeRequest CreateSchemeRequest;
    CreateSchemeRequest.Name = "MyScheme";
    struct ExternalAttributeDescription AttributesDescription[2];
    AttributesDescription[0].Type = STRING;
    AttributesDescription[0].Name = "NodeName";
    AttributesDescription[0].AttributeId = 0;
    AttributesDescription[0].Next = &AttributesDescription[1];
    AttributesDescription[1].Type = INT;
    AttributesDescription[1].Name = "NodeInt";
    AttributesDescription[1].AttributeId = 1;
    AttributesDescription[1].Next = NULL;
    CreateSchemeRequest.AttributesDescription = AttributesDescription;
    assert(createScheme(Controller, &CreateSchemeRequest) != 0);
    debugPrintFile(Controller->Allocator);
    struct CreateNodeRequest CreateNodeRequest;
    CreateNodeRequest.SchemeIdType = Scheme_NAME;
    CreateNodeRequest.SchemeId.SchemeName = "MyScheme";
    struct ExternalAttribute Attributes[2];
    CreateNodeRequest.Attributes = Attributes;
    Attributes[0].Id = 0;
    Attributes[0].Type = STRING;
    Attributes[0].Value.StringAddr = "Node 1";
    Attributes[1].Id = 1;
    Attributes[1].Type = INT;
    Attributes[1].Value.IntValue = 42;
    size_t Node1Id = createNode(Controller, &CreateNodeRequest);
    assert(Node1Id != 0);
    Attributes[0].Value.StringAddr = "Node 2";
    Attributes[1].Value.IntValue = 43;
    size_t Node2Id = createNode(Controller, &CreateNodeRequest);
    assert(Node2Id != 0);
    fprintf(stderr, "Nodes created\n");
    debugPrintFile(Controller->Allocator);
    struct CreateNodeLinkRequest CreateNodeLinkRequest;
    CreateNodeLinkRequest.Type = UNIDIRECTIONAL;
    CreateNodeLinkRequest.Weight = 1;
    CreateNodeLinkRequest.RightNodeId = Node1Id;
    CreateNodeLinkRequest.LeftNodeId = Node2Id;
    CreateNodeLinkRequest.Name = "Link";
    assert(createNodeLink(Controller, &CreateNodeLinkRequest) != 0);
    fprintf(stderr, "NodeLink created\n");
    debugPrintFile(Controller->Allocator);
    struct DeleteSchemeRequest DeleteRequest;
    DeleteRequest.Name = "MyScheme";
    fprintf(stderr, "Deleting Scheme\n");
    assert(deleteScheme(Controller, &DeleteRequest) != 0);
    debugPrintFile(Controller->Allocator);
    endWork(Controller);
    fprintf(stderr, "testTwoLinkedNodes finished\n\n");
}

void testForceNodeVacuum() {
    fprintf(stderr, "testForceNodeVacuum started\n");

    struct StorageController *Controllerr = beginWork("test.bin");
    assert(Controllerr != NULL);
    struct CreateSchemeRequest CreateSchemeRequest;
    CreateSchemeRequest.Name = "MyScheme";
    struct ExternalAttributeDescription AttributesDescription[2];
    AttributesDescription[0].Type = INT;
    AttributesDescription[0].Name = "NodeInt";
    AttributesDescription[0].AttributeId = 0;
    AttributesDescription[0].Next = &AttributesDescription[1];
    AttributesDescription[1].Type = STRING;
    AttributesDescription[1].Name = "NodeName";
    AttributesDescription[1].AttributeId = 1;
    AttributesDescription[1].Next = NULL;
    CreateSchemeRequest.AttributesDescription = AttributesDescription;
    const size_t SchemeId = createScheme(Controllerr, &CreateSchemeRequest);
    struct CreateNodeRequest CreateNodeRequest;
    CreateNodeRequest.SchemeIdType = Scheme_NAME;
    CreateNodeRequest.SchemeId.SchemeName = "MyScheme";
    struct ExternalAttribute Attributes[2];
    CreateNodeRequest.Attributes = Attributes;
    Attributes[1].Id = 1;
    Attributes[1].Type = STRING;
    Attributes[1].Value.StringAddr = "Node";
    Attributes[0].Id = 0;
    Attributes[0].Type = INT;

    for(size_t i = 0; i < SCHEME_NODES_PER_BLOCK * 5; ++i) {
        Attributes[0].Value.IntValue = i;
        createNode(Controllerr, &CreateNodeRequest);
    }
    fprintf(stderr, "After creating nodes\n");
    debugPrintFile(Controllerr->Allocator);
    assert(debugGetHeadersNumber(Controllerr->Allocator) == 7);
    struct AttributeFilter CentralNodesFilter = {
        .AttributeId = 0,
        .Type = INT_FILTER,
        .Next = NULL,
        .Data.Int.HasMin = true,
        .Data.Int.HasMax = true,
        .Data.Int.Min = 2,
        .Data.Int.Max = SCHEME_NODES_PER_BLOCK * 5 - 2
    };
    struct DeleteNodeRequest DNR = {
        .AttributesFilterChain = &CentralNodesFilter,
        .ById = false,
        .SchemeIdType = Scheme_ID,
        .SchemeId.SchemeId = SchemeId
    };
    fprintf(stderr, "deleting nodes\n");
    size_t DeletedNodes = deleteNode(Controllerr, &DNR);
    fprintf(stderr, "deleted %zu nodes\n", DeletedNodes);
    fprintf(stderr, "After deleting nodes\n");
    debugPrintFile(Controllerr->Allocator);
    assert(debugGetHeadersNumber(Controllerr->Allocator) == 3);

    struct DeleteSchemeRequest DeleteRequest;
    DeleteRequest.Name = "MyScheme";
    fprintf(stderr, "Deleting Scheme\n");
    assert(deleteScheme(Controllerr, &DeleteRequest) != 0);
    debugPrintFile(Controllerr->Allocator);
    assert(debugGetHeadersNumber(Controllerr->Allocator) == 2);
    endWork(Controllerr);

    fprintf(stderr, "testForceNodeVacuum finished \n");
}

void testForceLinkVacuum() {
    fprintf(stderr, "testForceLinkVacuum started \n");

    struct StorageController *Controller = beginWork("test.bin");

    struct CreateSchemeRequest CGR = {
        .Name = "G",
        .AttributesDescription = NULL
    };
    size_t SchemeId = createScheme(Controller, &CGR);
    struct CreateNodeRequest CNR = {
        .Attributes = NULL,
        .SchemeIdType = Scheme_ID,
        .SchemeId.SchemeId = SchemeId
    };
    size_t Node1Id = createNode(Controller, &CNR);
    size_t Node2Id = createNode(Controller, &CNR);

    struct CreateNodeLinkRequest CNLR = {
        .LeftNodeId = Node1Id,
        .RightNodeId = Node2Id,
        .Type = UNIDIRECTIONAL,
        .Name = "Link"
    };
    size_t LinkIds[LINKS_PER_BLOCK * 5];
    for (size_t i = 0; i < LINKS_PER_BLOCK * 5; ++i) {
        CNLR.Weight = (float) i + 0.01;
        LinkIds[i] = createNodeLink(Controller, &CNLR);
    }
    fprintf(stderr, "After links creation\n");
    debugPrintFile(Controller->Allocator);
    struct DeleteNodeLinkRequest DNLR = {
        .Type = BY_ID,
    };
    for (size_t i = 2; i < LINKS_PER_BLOCK * 5 - 2; ++i) {
        DNLR.Id = LinkIds[i];
        deleteNodeLink(Controller, &DNLR);
    }
    fprintf(stderr, "After links delete\n");
    debugPrintFile(Controller->Allocator);

    fprintf(stderr, "Deleting Scheme\n");
    struct DeleteSchemeRequest DGR = {
        .Name = "G"
    };
    deleteScheme(Controller, &DGR);
    debugPrintFile(Controller->Allocator);
    endWork(Controller);

    fprintf(stderr, "testForceLinkVacuum finished\n\n");
}
