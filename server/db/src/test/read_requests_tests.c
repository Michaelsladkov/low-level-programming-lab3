#include "read_requests_tests.h"

#include "external_representations.h"
#include "file_work/file_allocator_internals.h"
#include "request.h"
#include "request_result.h"
#include "storage_work/graph_storage_interface.h"
#include "storage_work/storage_controller.h"

#include "assert.h"
#include "stdio.h"
#include "string.h"

void testReadEmptyScheme() {
    fprintf(stderr, "testReadEmptyScheme started\n");

    struct StorageController* Controller = beginWork("test.bin");
    assert(Controller != NULL);
    struct CreateSchemeRequest CreateRequest;
    CreateRequest.AttributesDescription = NULL;
    CreateRequest.Name = "TestEmptyScheme";
    debugPrintFile(Controller->Allocator);
    fprintf(stderr, "Creating Scheme\n");
    size_t Created = createScheme(Controller, &CreateRequest);
    assert(Created != 0);
    struct ReadSchemeRequest ReadRequest;
    ReadRequest.Name = CreateRequest.Name;
    fprintf(stderr, "Reading our empty Scheme\n");
    struct SchemeResultSet* Result = readScheme(Controller, &ReadRequest);
    struct ExternalScheme* Scheme;
    fprintf(stderr, "Read request performed\n");
    assert(!SchemeResultSetIsEmpty(Result));
    assert(!hasNextScheme(Result));
    fprintf(stderr, "Getting external representation of Scheme\n");
    assert(readResultScheme(Result, &Scheme));
    fprintf(stderr, "Got external representation of Scheme\n");
    assert(Scheme->NodesNumber == 0);
    assert(Scheme->AttributesDescriptionNumber == 0);
    assert(strcmp(Scheme->Name, CreateRequest.Name) == 0);
    fprintf(stderr, "Before deleting external Scheme\n");
    fprintf(stderr, "Scheme pointer: %p\n", Scheme);
    deleteExternalScheme(&Scheme);
    fprintf(stderr, "Before deleting result set\n");
    deleteSchemeResultSet(&Result);
    fprintf(stderr, "Performing cleanup\n");
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
    fprintf(stderr, "testReadEmptyScheme finished\n\n");
}

void testReadEmptySchemeWithNodes() {
    fprintf(stderr, "testReadEmptySchemeWithNodes started\n");
    
    struct StorageController* Controller = beginWork("test.bin");
    assert(Controller != NULL);
    struct CreateSchemeRequest CreateSchemeRequest;
    CreateSchemeRequest.AttributesDescription = NULL;
    CreateSchemeRequest.Name = "TestEmptyScheme";
    debugPrintFile(Controller->Allocator);
    fprintf(stderr, "Creating Scheme\n");
    size_t Created = createScheme(Controller, &CreateSchemeRequest);
    assert(Created != 0);

    struct CreateNodeRequest CreateNodeRequest;
    CreateNodeRequest.SchemeIdType = Scheme_NAME;
    CreateNodeRequest.SchemeId.SchemeName = CreateSchemeRequest.Name;
    CreateNodeRequest.Attributes = NULL;
    size_t CreatedNode1 = createNode(Controller, &CreateNodeRequest);
    assert(CreatedNode1 != 0);
    fprintf(stderr, "Created node with id: %zu\n", CreatedNode1);
    size_t CreatedNode2 = createNode(Controller, &CreateNodeRequest);
    assert(CreatedNode2 != 0);
    fprintf(stderr, "Created node with id: %zu\n", CreatedNode2);

    struct CreateNodeLinkRequest CreateNodeLinkRequest;
    CreateNodeLinkRequest.Weight = 0;
    CreateNodeLinkRequest.Type = UNIDIRECTIONAL;
    CreateNodeLinkRequest.LeftNodeId = CreatedNode1;
    CreateNodeLinkRequest.RightNodeId = CreatedNode2;
    CreateNodeLinkRequest.Name = "Link";
    size_t CreatedLink = createNodeLink(Controller, &CreateNodeLinkRequest);
    assert(CreatedLink != 0);
    fprintf(stderr, "Created node link with id: %zu\n", CreatedLink);


    struct ReadSchemeRequest ReadRequest;
    ReadRequest.Name = CreateSchemeRequest.Name;
    struct SchemeResultSet* Result = readScheme(Controller, &ReadRequest);
    struct ExternalScheme* Scheme;
    fprintf(stderr, "Read request performed\n");
    assert(!SchemeResultSetIsEmpty(Result));
    assert(!hasNextScheme(Result));
    fprintf(stderr, "Getting external representation of Scheme\n");
    assert(readResultScheme(Result, &Scheme));
    fprintf(stderr, "Got external representation of Scheme\n");
    assert(Scheme->NodesNumber == 2);
    assert(Scheme->AttributesDescriptionNumber == 0);
    assert(strcmp(Scheme->Name, CreateSchemeRequest.Name) == 0);
    deleteExternalScheme(&Scheme);

    deleteSchemeResultSet(&Result);

    fprintf(stderr, "Performing cleanup\n");
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

    fprintf(stderr, "testReadEmptySchemeWithNodes finished\n\n");
}

void testReadFourAttrScheme() {
    fprintf(stderr, "testReadFourAttrScheme started\n");
    
    struct StorageController* Controller = beginWork("test.bin");

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

    struct ReadSchemeRequest ReadRequest;
    ReadRequest.Name = CreateSchemeRequest.Name;
    struct SchemeResultSet* Result = readScheme(Controller, &ReadRequest);
    struct ExternalScheme* Scheme;
    fprintf(stderr, "Read request performed\n");
    assert(!SchemeResultSetIsEmpty(Result));
    assert(!hasNextScheme(Result));
    fprintf(stderr, "Getting external representation of Scheme\n");
    assert(readResultScheme(Result, &Scheme));
    fprintf(stderr, "Got external representation of Scheme\n");
    assert(Scheme->NodesNumber == 0);
    assert(Scheme->AttributesDescriptionNumber == 4);
    assert(strcmp(Scheme->Name, CreateSchemeRequest.Name) == 0);
    for (size_t i = 0; i < Scheme->AttributesDescriptionNumber; ++i) {
        fprintf(stderr, "Checking attribute %zu\n", i);
        assert(Scheme->AttributesDescription[i].AttributeId == CreateSchemeRequest.AttributesDescription[i].AttributeId);
        assert(Scheme->AttributesDescription[i].Type == CreateSchemeRequest.AttributesDescription[i].Type);
        assert(strcmp(Scheme->AttributesDescription[i].Name, CreateSchemeRequest.AttributesDescription[i].Name) == 0);
    }
    deleteExternalScheme(&Scheme);
    deleteSchemeResultSet(&Result);
    fprintf(stderr, "Performing cleanup\n");
    debugPrintFile(Controller->Allocator);
    struct DeleteSchemeRequest DeleteRequest;
    DeleteRequest.Name = CreateSchemeRequest.Name;
    size_t Deleted = deleteScheme(Controller, &DeleteRequest);
    fprintf(stderr, "Deleted Schemes number %zu, 1 expected\n", Deleted);
    assert(Deleted == 1);
    assert(Controller->Storage.SchemeNumber == 0);
    assert(!Controller->Storage.LastScheme.HasValue);
    assert(!Controller->Storage.Schemes.HasValue);
    debugPrintFile(Controller->Allocator);
    endWork(Controller);

    fprintf(stderr, "testReadFourAttrScheme finished\n\n");
}

void testReadNodes() {
    fprintf(stderr, "testReadNodes started\n");
    
    struct StorageController* Controller = beginWork("test.bin");

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

    struct CreateNodeRequest CreateNodeRequest;
    CreateNodeRequest.SchemeIdType = Scheme_NAME;
    CreateNodeRequest.SchemeId.SchemeName = "MyScheme";
    struct ExternalAttribute Attributes[4];
    CreateNodeRequest.Attributes = Attributes;
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

    struct ReadNodeRequest ReadRequest;
    ReadRequest.ById = false;
    ReadRequest.AttributesFilterChain = NULL;
    ReadRequest.SchemeIdType = Scheme_NAME;
    ReadRequest.SchemeId.SchemeName = CreateSchemeRequest.Name;
    struct NodeResultSet* Result = readNode(Controller, &ReadRequest);
    assert(nodeResultSetGetSize(Result) == 3);

    struct ExternalNode* Node1;
    assert(readResultNode(Result, &Node1));
    assert(Node1->AttributesNumber == 4);
    assert(Node1->Attributes[0].Type == STRING);
    assert(strcmp(Node1->Attributes[0].Value.StringAddr, "Node 1") == 0);
    assert(Node1->Attributes[1].Type == INT);
    assert(Node1->Attributes[1].Value.IntValue == 42);
    assert(Node1->Attributes[2].Type == FLOAT);
    assert(Node1->Attributes[2].Value.FloatValue == 3.14f);
    assert(Node1->Attributes[3].Type == BOOL);
    assert(Node1->Attributes[3].Value.BoolValue == true);

    assert(moveToNextNode(Result));

    struct ExternalNode* Node2;
    assert(readResultNode(Result, &Node2));
    assert(Node2->AttributesNumber == 4);
    assert(Node2->Attributes[0].Type == STRING);
    assert(strcmp(Node2->Attributes[0].Value.StringAddr, "Node 2") == 0);
    assert(Node2->Attributes[1].Type == INT);
    assert(Node2->Attributes[1].Value.IntValue == 228);
    assert(Node2->Attributes[2].Type == FLOAT);
    assert(Node2->Attributes[2].Value.FloatValue == 3.14f);
    assert(Node2->Attributes[3].Type == BOOL);
    assert(Node2->Attributes[3].Value.BoolValue == true);

    assert(moveToNextNode(Result));

    struct ExternalNode* Node3;
    assert(readResultNode(Result, &Node3));
    assert(Node3->AttributesNumber == 4);
    assert(Node3->Attributes[0].Type == STRING);
    assert(strcmp(Node3->Attributes[0].Value.StringAddr, "Node 3") == 0);
    assert(Node3->Attributes[1].Type == INT);
    assert(Node3->Attributes[1].Value.IntValue == 1488);
    assert(Node3->Attributes[2].Type == FLOAT);
    assert(Node3->Attributes[2].Value.FloatValue == 2.783f);
    assert(Node3->Attributes[3].Type == BOOL);
    assert(Node3->Attributes[3].Value.BoolValue == false);

    assert(!moveToNextNode(Result));
    assert(!moveToNextNode(Result));
    assert(hasPreviousNode(Result));
    assert(moveToPreviousNode(Result));

    deleteExternalNode(&Node2);
    assert(readResultNode(Result, &Node2));
    assert(Node2->AttributesNumber == 4);
    assert(Node2->Attributes[0].Type == STRING);
    assert(strcmp(Node2->Attributes[0].Value.StringAddr, "Node 2") == 0);
    assert(Node2->Attributes[1].Type == INT);
    assert(Node2->Attributes[1].Value.IntValue == 228);
    assert(Node2->Attributes[2].Type == FLOAT);
    assert(Node2->Attributes[2].Value.FloatValue == 3.14f);
    assert(Node2->Attributes[3].Type == BOOL);
    assert(Node2->Attributes[3].Value.BoolValue == true);

    deleteExternalNode(&Node1);
    deleteExternalNode(&Node2);
    deleteExternalNode(&Node3);
    deleteNodeResultSet(&Result);

    struct AttributeFilter Filters[3];
    Filters[0].AttributeId = 0;
    Filters[0].Type = STRING_FILTER;
    Filters[0].Data.String.Type = STRING_EQUAL;
    Filters[0].Data.String.Data.StringEqual = "Node 2";
    Filters[0].Next = NULL;
    ReadRequest.AttributesFilterChain = Filters;
    Result = readNode(Controller, &ReadRequest);
    assert(nodeResultSetGetSize(Result) == 1);
    assert(readResultNode(Result, &Node2));
    assert(strcmp(Node2->Attributes[0].Value.StringAddr, "Node 2") == 0);
    assert(Node2->Attributes[1].Type == INT);
    assert(Node2->Attributes[1].Value.IntValue == 228);
    deleteExternalNode(&Node2);
    deleteNodeResultSet(&Result);

    Filters[0].Next = &(Filters[1]);
    Filters[1].Next = NULL;
    Filters[1].Type = INT_FILTER;
    Filters[1].AttributeId = 1;
    Filters[1].Data.Int.HasMin = false;
    Filters[1].Data.Int.HasMax = true;
    Filters[1].Data.Int.Max = 13;
    Result = readNode(Controller, &ReadRequest);
    assert(nodeResultSetIsEmpty(Result));
    assert(nodeResultSetGetSize(Result) == 0);
    assert(!hasNextNode(Result));
    assert(!hasPreviousNode(Result));
    deleteNodeResultSet(&Result);
    debugPrintFile(Controller->Allocator);

    fprintf(stderr, "Performing cleanup\n");
    struct DeleteSchemeRequest DeleteSchemeRequest;
    DeleteSchemeRequest.Name = CreateSchemeRequest.Name;
    assert(deleteScheme(Controller, &DeleteSchemeRequest) == 1);
    debugPrintFile(Controller->Allocator);
    endWork(Controller);
    fprintf(stderr, "testReadNodes finished\n\n");
}

void testReadNodesAfterDelete() {
    fprintf(stderr, "testReadNodesAfterDelete started\n");
    
    struct StorageController* Controller = beginWork("test.bin");

    struct CreateSchemeRequest CreateSchemeRequest;
    CreateSchemeRequest.Name = "MyScheme";
    struct ExternalAttributeDescription AttributeDescription;
    AttributeDescription.AttributeId = 0;
    AttributeDescription.Name = "Identifier";
    AttributeDescription.Next = NULL;
    AttributeDescription.Type = INT;
    CreateSchemeRequest.AttributesDescription = &AttributeDescription;
    size_t SchemeId = createScheme(Controller, &CreateSchemeRequest);
    assert(SchemeId != 0);

    struct CreateNodeRequest CreateNodeRequest;
    CreateNodeRequest.SchemeIdType = Scheme_ID;
    CreateNodeRequest.SchemeId.SchemeId = SchemeId;
    struct ExternalAttribute Attribute;
    Attribute.Id = 0;
    Attribute.Type = INT;
    Attribute.Value.IntValue = 0;
    CreateNodeRequest.Attributes = &Attribute;
    createNode(Controller, &CreateNodeRequest);
    Attribute.Value.IntValue = 1;
    createNode(Controller, &CreateNodeRequest);
    Attribute.Value.IntValue = 2;
    createNode(Controller, &CreateNodeRequest);
    Attribute.Value.IntValue = 3;
    createNode(Controller, &CreateNodeRequest);
    Attribute.Value.IntValue = 4;
    createNode(Controller, &CreateNodeRequest);
    Attribute.Value.IntValue = 5;
    createNode(Controller, &CreateNodeRequest);
    Attribute.Value.IntValue = 6;
    createNode(Controller, &CreateNodeRequest);
    Attribute.Value.IntValue = 7;
    size_t NodeToDeleteId = createNode(Controller, &CreateNodeRequest);
    assert(NodeToDeleteId != 0);
    Attribute.Value.IntValue = 8;
    createNode(Controller, &CreateNodeRequest);

    struct ReadNodeRequest ReadRequest;
    ReadRequest.AttributesFilterChain = NULL;
    ReadRequest.ById = false;
    ReadRequest.SchemeId.SchemeId = SchemeId;
    ReadRequest.SchemeIdType = Scheme_ID;
    struct NodeResultSet* Result = readNode(Controller, &ReadRequest);
    assert(nodeResultSetGetSize(Result) == 9);
    for (uint8_t i = 0; i <= 8; ++i) {
        struct ExternalNode* Node;
        assert(readResultNode(Result, &Node));
        assert(Node->Attributes[0].Id == 0);
        assert(Node->Attributes[0].Type == INT);
        assert(Node->Attributes[0].Value.IntValue == i);
        deleteExternalNode(&Node);
        moveToNextNode(Result);
    }
    deleteNodeResultSet(&Result);

    struct DeleteNodeRequest DeleteNodeRequest;
    DeleteNodeRequest.SchemeIdType = Scheme_ID;
    DeleteNodeRequest.SchemeId.SchemeId = SchemeId;
    DeleteNodeRequest.ById = true;
    DeleteNodeRequest.Id = NodeToDeleteId;
    assert(deleteNode(Controller, &DeleteNodeRequest) == 1);
    Result = readNode(Controller, &ReadRequest);
    fprintf(stderr, "Got %zu nodes\n", nodeResultSetGetSize(Result));
    assert(nodeResultSetGetSize(Result) == 8);
    for (uint8_t i = 0; i <= 8; ++i) {
        if (i == 7) continue;
        struct ExternalNode* Node;
        assert(readResultNode(Result, &Node));
        assert(Node->Attributes[0].Id == 0);
        assert(Node->Attributes[0].Type == INT);
        assert(Node->Attributes[0].Value.IntValue == i);
        deleteExternalNode(&Node);
        moveToNextNode(Result);
    }
    deleteNodeResultSet(&Result);

    DeleteNodeRequest.ById = false;
    DeleteNodeRequest.SchemeId.SchemeId = SchemeId;
    DeleteNodeRequest.SchemeIdType = Scheme_ID;
    struct AttributeFilter DeleteFilter;
    DeleteFilter.Type = INT_FILTER;
    DeleteFilter.AttributeId = 0;
    DeleteFilter.Next = NULL;
    DeleteFilter.Data.Int.HasMin = true;
    DeleteFilter.Data.Int.Min = 3;
    DeleteFilter.Data.Int.HasMax = true;
    DeleteFilter.Data.Int.Max = 5;
    DeleteNodeRequest.AttributesFilterChain = &DeleteFilter;
    assert(deleteNode(Controller, &DeleteNodeRequest) == 3);
    Result = readNode(Controller, &ReadRequest);
    assert(nodeResultSetGetSize(Result) == 5);
    for (uint8_t i = 0; i <= 8; ++i) {
        if (i == 7) continue;
        if (i >= 3 && i <=5) continue;
        struct ExternalNode* Node;
        assert(readResultNode(Result, &Node));
        assert(Node->Attributes[0].Id == 0);
        assert(Node->Attributes[0].Type == INT);
        assert(Node->Attributes[0].Value.IntValue == i);
        deleteExternalNode(&Node);
        moveToNextNode(Result);
    }
    deleteNodeResultSet(&Result);

    fprintf(stderr, "Performing cleanup\n");
    struct DeleteSchemeRequest DeleteSchemeRequest;
    DeleteSchemeRequest.Name = CreateSchemeRequest.Name;
    assert(deleteScheme(Controller, &DeleteSchemeRequest) == 1);
    debugPrintFile(Controller->Allocator);
    endWork(Controller);

    fprintf(stderr, "testReadNodesAfterDelete finished\n\n");
}

void testReadLinks() {
    fprintf(stderr, "testReadLinks started\n");
    
    struct StorageController *Controller = beginWork("test.bin");

    struct CreateSchemeRequest CGR;
    CGR.Name = "G";
    struct ExternalAttributeDescription EAD;
    EAD.AttributeId = 0;
    EAD.Name = "id";
    EAD.Next = NULL;
    EAD.Type = INT;
    CGR.AttributesDescription = &EAD;
    assert(createScheme(Controller, &CGR) != 0);
    struct CreateNodeRequest CNR;
    struct ExternalAttribute Attr;
    Attr.Id = 0;
    Attr.Type = INT;
    Attr.Value.IntValue = 0;
    CNR.SchemeIdType = Scheme_NAME;
    CNR.SchemeId.SchemeName = "G";
    CNR.Attributes = &Attr;
    size_t Node0Id = createNode(Controller, &CNR);
    Attr.Value.IntValue = 1;
    size_t Node1Id = createNode(Controller, &CNR);

    struct CreateNodeLinkRequest CNLR;
    CNLR.LeftNodeId = Node0Id;
    CNLR.RightNodeId = Node1Id;
    CNLR.Type = UNIDIRECTIONAL;
    CNLR.Weight = (float)1;
    CNLR.Name = "Link";
    assert(createNodeLink(Controller, &CNLR) != 0);
    CNLR.Weight = (float)2;
    assert(createNodeLink(Controller, &CNLR) != 0);
    CNLR.LeftNodeId = Node1Id;
    CNLR.RightNodeId = Node0Id;
    CNLR.Weight = 3;
    size_t LinkToDelete = createNodeLink(Controller, &CNLR);
    assert(LinkToDelete != 0);
    CNLR.Weight = 4;
    assert(createNodeLink(Controller, &CNLR) != 0);

    struct ReadNodeLinkRequest ReadRequest;
    ReadRequest.Type = ALL;
    struct NodeLinkResultSet* Result = readNodeLink(Controller, &ReadRequest);
    assert(nodeLinkResultSetGetSize(Result) == 4);
    struct ExternalNodeLink *NL1, *NL2, *NL3, *NL4;
    assert(readResultNodeLink(Result, &NL1));
    assert(moveToNextNodeLink(Result));
    assert(readResultNodeLink(Result, &NL2));
    assert(moveToNextNodeLink(Result));
    assert(readResultNodeLink(Result, &NL3));
    assert(moveToNextNodeLink(Result));
    assert(readResultNodeLink(Result, &NL4));

    assert(NL1->LeftNodeId == Node0Id);
    assert(NL2->LeftNodeId == Node0Id);
    assert(NL1->RightNodeId == Node1Id);
    assert(NL2->RightNodeId == Node1Id);
    assert(NL3->RightNodeId == Node0Id);
    assert(NL4->RightNodeId == Node0Id);
    assert(NL3->LeftNodeId == Node1Id);
    assert(NL4->LeftNodeId == Node1Id);
    assert(NL1->Type == UNIDIRECTIONAL);
    assert(NL2->Type == UNIDIRECTIONAL);
    assert(NL3->Type == UNIDIRECTIONAL);
    assert(NL4->Type == UNIDIRECTIONAL);
    assert(NL1->Weight == (float)1);
    assert(NL2->Weight == (float)2);
    assert(NL3->Weight == (float)3);
    assert(NL4->Weight == (float)4);

    deleteExternalNodeLink(&NL1);
    deleteExternalNodeLink(&NL2);
    deleteExternalNodeLink(&NL3);
    deleteExternalNodeLink(&NL4);
    deleteNodeLinkResultSet(&Result);

    ReadRequest.Type = BY_LEFT_NODE_ID;
    ReadRequest.Id = Node0Id;
    Result = readNodeLink(Controller, &ReadRequest);
    assert(nodeLinkResultSetGetSize(Result) == 2);
    assert(readResultNodeLink(Result, &NL1));
    assert(moveToNextNodeLink(Result));
    assert(readResultNodeLink(Result, &NL2));
    assert(!hasNextNodeLink(Result));
    assert(hasPreviousNodeLink(Result));
    assert(NL1->Weight == (float)1);
    assert(NL2->Weight == (float)2);

    deleteExternalNodeLink(&NL1);
    deleteExternalNodeLink(&NL2);
    deleteNodeLinkResultSet(&Result);

    struct DeleteSchemeRequest DGR;
    DGR.Name = "G";
    deleteScheme(Controller, &DGR);
    endWork(Controller);
    fprintf(stderr, "testReadLinks finished\n\n");
}

void testReadLinksAfterDelete() {
    fprintf(stderr, "testReadLinksAfterDelete started\n");
        
    struct StorageController *Controller = beginWork("test.bin");

    struct CreateSchemeRequest CGR;
    CGR.Name = "G";
    struct ExternalAttributeDescription EAD;
    EAD.AttributeId = 0;
    EAD.Name = "id";
    EAD.Next = NULL;
    EAD.Type = INT;
    CGR.AttributesDescription = &EAD;
    assert(createScheme(Controller, &CGR) != 0);
    struct CreateNodeRequest CNR;
    struct ExternalAttribute Attr;
    Attr.Id = 0;
    Attr.Type = INT;
    Attr.Value.IntValue = 0;
    CNR.SchemeIdType = Scheme_NAME;
    CNR.SchemeId.SchemeName = "G";
    CNR.Attributes = &Attr;
    size_t Node0Id = createNode(Controller, &CNR);
    Attr.Value.IntValue = 1;
    size_t Node1Id = createNode(Controller, &CNR);

    struct CreateNodeLinkRequest CNLR;
    CNLR.LeftNodeId = Node0Id;
    CNLR.RightNodeId = Node1Id;
    CNLR.Type = UNIDIRECTIONAL;
    CNLR.Weight = (float)1;
    assert(createNodeLink(Controller, &CNLR) != 0);
    CNLR.Weight = (float)2;
    assert(createNodeLink(Controller, &CNLR) != 0);
    CNLR.LeftNodeId = Node1Id;
    CNLR.RightNodeId = Node0Id;
    CNLR.Weight = 3;
    size_t LinkToDelete = createNodeLink(Controller, &CNLR);
    assert(LinkToDelete != 0);
    CNLR.Weight = 4;
    assert(createNodeLink(Controller, &CNLR) != 0);

    struct ReadNodeLinkRequest RNLR = {
        .Type = ALL
    };
    struct NodeLinkResultSet* Result = readNodeLink(Controller, &RNLR);
    assert(nodeLinkResultSetGetSize(Result) == 4);
    deleteNodeLinkResultSet(&Result);

    struct DeleteNodeLinkRequest DNLR = {
        .Id = LinkToDelete,
        .Type = BY_ID
    };
    assert(deleteNodeLink(Controller, &DNLR) == 1);
    Result = readNodeLink(Controller, &RNLR);
    assert(nodeLinkResultSetGetSize(Result) == 3);
    do {
        struct ExternalNodeLink *Link;
        readResultNodeLink(Result, &Link);
        assert(Link->Weight != (float)3);
        deleteExternalNodeLink(&Link);
        moveToNextNodeLink(Result);
    } while(hasNextNodeLink(Result));
    deleteNodeLinkResultSet(&Result);

    DNLR.Type = BY_LEFT_NODE_ID;
    DNLR.Id = Node0Id;
    assert(deleteNodeLink(Controller, &DNLR) == 2);
    Result = readNodeLink(Controller, &RNLR);
    assert(nodeLinkResultSetGetSize(Result) == 1);
    do {
        struct ExternalNodeLink *Link;
        readResultNodeLink(Result, &Link);
        assert(Link->LeftNodeId != Node0Id);
        deleteExternalNodeLink(&Link);
        moveToNextNodeLink(Result);
    } while(hasNextNodeLink(Result));
    deleteNodeLinkResultSet(&Result);

    struct DeleteSchemeRequest DGR;
    DGR.Name = "G";
    deleteScheme(Controller, &DGR);
    endWork(Controller);

    fprintf(stderr, "testReadLinksAfterDelete finished\n\n");
}

void testReadLinksAfterNodeDelete() {
    fprintf(stderr, "testReadLinksAfterNodeDelete started\n");
    
    struct StorageController *Controller = beginWork("test.bin");

    struct CreateSchemeRequest CGR;
    CGR.Name = "G";
    struct ExternalAttributeDescription EAD;
    EAD.AttributeId = 0;
    EAD.Name = "id";
    EAD.Next = NULL;
    EAD.Type = INT;
    CGR.AttributesDescription = &EAD;
    assert(createScheme(Controller, &CGR) != 0);
    struct CreateNodeRequest CNR;
    struct ExternalAttribute Attr;
    Attr.Id = 0;
    Attr.Type = INT;
    Attr.Value.IntValue = 0;
    CNR.SchemeIdType = Scheme_NAME;
    CNR.SchemeId.SchemeName = "G";
    CNR.Attributes = &Attr;
    size_t Node0Id = createNode(Controller, &CNR);
    Attr.Value.IntValue = 1;
    size_t Node1Id = createNode(Controller, &CNR);
    Attr.Value.IntValue = 2;
    size_t Node2Id = createNode(Controller, &CNR);
    Attr.Value.IntValue = 3;
    size_t Node3Id = createNode(Controller, &CNR);

    struct CreateNodeLinkRequest CNLR;
    CNLR.LeftNodeId = Node0Id;
    CNLR.RightNodeId = Node1Id;
    CNLR.Type = UNIDIRECTIONAL;
    CNLR.Weight = (float)1;
    assert(createNodeLink(Controller, &CNLR) != 0);
    CNLR.Weight = (float)2;
    assert(createNodeLink(Controller, &CNLR) != 0);
    CNLR.LeftNodeId = Node1Id;
    CNLR.RightNodeId = Node0Id;
    CNLR.Weight = (float)3;
    assert(createNodeLink(Controller, &CNLR) != 0);
    CNLR.Weight = (float)4;
    assert(createNodeLink(Controller, &CNLR) != 0);
    CNLR.LeftNodeId = Node2Id;
    CNLR.RightNodeId = Node3Id;
    CNLR.Weight = (float)5;
    assert(createNodeLink(Controller, &CNLR) != 0);

    struct ReadNodeLinkRequest RNLR = {
        .Type = ALL
    };
    struct NodeLinkResultSet* Result = readNodeLink(Controller, &RNLR);
    assert(nodeLinkResultSetGetSize(Result) == 5);
    deleteNodeLinkResultSet(&Result);

    struct DeleteNodeRequest DNR = {
        .SchemeIdType = Scheme_NAME,
        .SchemeId.SchemeName = "G",
        .ById = true,
        .Id = Node0Id
    };
    assert(deleteNode(Controller, &DNR) == 1);
    Result = readNodeLink(Controller, &RNLR);
    assert(nodeLinkResultSetGetSize(Result) == 1);
    do {
        struct ExternalNodeLink *Link;
        readResultNodeLink(Result, &Link);
        assert(Link->LeftNodeId != Node0Id && Link->RightNodeId != Node0Id);
        assert(Link->LeftNodeId != Node1Id && Link->RightNodeId != Node1Id);
        deleteExternalNodeLink(&Link);
        moveToNextNodeLink(Result);
    } while(hasNextNodeLink(Result));
    deleteNodeLinkResultSet(&Result);

    struct DeleteSchemeRequest DGR;
    DGR.Name = "G";
    deleteScheme(Controller, &DGR);
    endWork(Controller);
    fprintf(stderr, "testReadLinksAfterNodeDelete finished\n\n");
}

void testReadNodesAndLinksWithReopen() {
    fprintf(stderr, "testReadNodesAndLinksWithReopen started\n");
    
    /*
    Создать 4 ноды и 4 линки. Закрыть, открыть, проверить целостность.
    Удалить одну ноду. Закрыть, открыть, проверить целостность
    */
    struct StorageController *Controller = beginWork("test.bin");

    struct ExternalAttributeDescription Descriptions[2];
    Descriptions[0].AttributeId = 0;
    Descriptions[0].Name = "Id";
    Descriptions[0].Next = &(Descriptions[1]);
    Descriptions[0].Type = INT;
    Descriptions[1].AttributeId = 1;
    Descriptions[1].Name = "String";
    Descriptions[1].Next = NULL;
    Descriptions[1].Type = STRING;
    struct CreateSchemeRequest CGR = {
        .Name = "G",
        .AttributesDescription = Descriptions
    };
    assert(createScheme(Controller, &CGR) != 0);

    struct ExternalAttribute Attrs[2];
    Attrs[0].Id = 0;
    Attrs[0].Type = INT;
    Attrs[1].Id = 1;
    Attrs[1].Type = STRING;
    size_t NodeIds[4] = {0};
    char *NodeNames[4] = {"Node A", "Node B", "Node C", "Node D"};
    struct CreateNodeRequest CNR = {
        .Attributes = Attrs,
        .SchemeIdType = Scheme_NAME,
        .SchemeId.SchemeName = "G"
    };
    for (uint8_t i = 0; i < 4; ++i) {
        fprintf(stderr, "Creating Node %d\n", i + 1);
        Attrs[0].Value.IntValue = 10 + i + 1;
        Attrs[1].Value.StringAddr = NodeNames[i];
        NodeIds[i] = createNode(Controller, &CNR);
        assert(NodeIds[i] != 0);
        fprintf(stderr, "Created node with id %zu\n", NodeIds[i]);
    }
    float LinkWeights[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    size_t NodeLinkIds[4] = {0};
    struct CreateNodeLinkRequest CNLR = {
        .Type = UNIDIRECTIONAL,
        .Name = "Link"
    };
    for (uint8_t i = 0; i < 4; ++i) {
        fprintf(stderr, "Creating Node Link %d\n", i + 1);
        CNLR.LeftNodeId = NodeIds[i];
        CNLR.RightNodeId = NodeIds[(i + 1) % 4];
        CNLR.Weight = LinkWeights[i];
        NodeLinkIds[i] = createNodeLink(Controller, &CNLR);
        assert(NodeLinkIds[i] != 0);
        fprintf(stderr, "Created node link with id %zu\n", NodeLinkIds[i]);
    }

    fprintf(stderr, "Data file closing and reopening\n");
    endWork(Controller);

    Controller = beginWork("test.bin");
    struct ReadSchemeRequest RGR = {
        .Name = "G"
    };
    struct SchemeResultSet *GRS = readScheme(Controller, &RGR);
    assert(!SchemeResultSetIsEmpty(GRS));
    struct ExternalScheme *EG;
    assert(readResultScheme(GRS, &EG));
    assert(EG->AttributesDescriptionNumber == 2);
    assert(strcmp(EG->AttributesDescription[0].Name, "Id") == 0);
    assert(strcmp(EG->AttributesDescription[1].Name, "String") == 0);
    assert(EG->NodesNumber == 4);
    assert(strcmp(EG->Name, "G") == 0);
    deleteExternalScheme(&EG);
    deleteSchemeResultSet(&GRS);
    struct ReadNodeRequest RNR = {
        .AttributesFilterChain = NULL,
        .ById = false,
        .SchemeIdType = Scheme_NAME,
        .SchemeId.SchemeName = "G"
    };
    struct NodeResultSet *NRS = readNode(Controller, &RNR);
    assert(nodeResultSetGetSize(NRS) == 4);
    for(uint8_t i = 0; i < 4; ++i) {
        struct ExternalNode *ToCheck;
        assert(readResultNode(NRS, &ToCheck));
        assert(ToCheck->Id == NodeIds[i]);
        assert(ToCheck->AttributesNumber == 2);
        assert(ToCheck->Attributes[0].Type == INT);
        assert(ToCheck->Attributes[0].Id == 0);
        assert(ToCheck->Attributes[0].Value.IntValue == 11 + i);
        assert(ToCheck->Attributes[1].Type == STRING);
        assert(ToCheck->Attributes[1].Id == 1);
        assert(strcmp(ToCheck->Attributes[1].Value.StringAddr, NodeNames[i]) == 0);
        deleteExternalNode(&ToCheck);
        moveToNextNode(NRS);
    }
    deleteNodeResultSet(&NRS);
    fprintf(stderr, "Checking link filter\n");
    struct AttributeFilter LinkFilter = {
        .Type = LINK_FILTER,
        .Next = NULL,
        .Data.Link.NodeId = NodeIds[0],
        .Data.Link.Relation = HAS_LINK_FROM,
        .Data.Link.WeightFilter.HasMax = false,
        .Data.Link.WeightFilter.HasMin = false
    };
    RNR.AttributesFilterChain = &LinkFilter;
    NRS = readNode(Controller, &RNR);
    assert(nodeResultSetGetSize(NRS) == 2);
    fprintf(stderr, "Reading neighbours\n");
    struct ExternalNode *Neighbour1;
    assert(readResultNode(NRS, &Neighbour1));
    assert(Neighbour1->Id == NodeIds[1] || Neighbour1->Id == NodeIds[3]);
    assert(moveToNextNode(NRS));
    struct ExternalNode *Neighbour2;
    assert(readResultNode(NRS, &Neighbour2));
    assert(Neighbour2->Id == NodeIds[1] || Neighbour2->Id == NodeIds[3]);
    assert(Neighbour1->Id != Neighbour2->Id);
    deleteExternalNode(&Neighbour1);
    deleteExternalNode(&Neighbour2);
    deleteNodeResultSet(&NRS);
    struct ReadNodeLinkRequest RNLR = {
        .Type = ALL
    };
    struct NodeLinkResultSet *NLRS = readNodeLink(Controller, &RNLR);
    assert(nodeLinkResultSetGetSize(NLRS) == 4);
    for (uint8_t i = 0; i < 4; ++i) {
        struct ExternalNodeLink *ToCheck;
        assert(readResultNodeLink(NLRS, &ToCheck));
        assert(ToCheck->Id == NodeLinkIds[i]);
        assert(ToCheck->LeftNodeId = NodeIds[i]);
        assert(ToCheck->RightNodeId = NodeIds[(i + 1) % 4]);
        assert(ToCheck->Weight == LinkWeights[i]);
        deleteExternalNodeLink(&ToCheck);
        moveToNextNodeLink(NLRS);
    }
    deleteNodeLinkResultSet(&NLRS);

    struct DeleteNodeLinkRequest DNLR = {
        .Type = BY_LEFT_NODE_ID,
        .Id = NodeIds[1]
    };
    assert(deleteNodeLink(Controller, &DNLR) == 1);
    struct AttributeFilter Filter = {
        .AttributeId = 0,
        .Next = NULL,
        .Type = INT,
        .Data.Int.Max = 11,
        .Data.Int.Min = 11,
        .Data.Int.HasMax = true,
        .Data.Int.HasMin = true
    };
    struct DeleteNodeRequest DNR = {
        .SchemeIdType = Scheme_NAME,
        .SchemeId.SchemeName = "G",
        .AttributesFilterChain = &Filter,
        .ById = false
    };
    assert(deleteNode(Controller, &DNR) == 1);
    endWork(Controller);

    Controller = beginWork("test.bin"); 
    GRS = readScheme(Controller, &RGR);
    assert(!SchemeResultSetIsEmpty(GRS));
    assert(readResultScheme(GRS, &EG));
    assert(EG->AttributesDescriptionNumber == 2);
    assert(EG->NodesNumber == 3);
    assert(strcmp(EG->Name, "G") == 0);
    deleteExternalScheme(&EG);
    deleteSchemeResultSet(&GRS);
    RNR.AttributesFilterChain = NULL;
    NRS = readNode(Controller, &RNR);
    assert(nodeResultSetGetSize(NRS) == 3);
    for(uint8_t i = 1; i < 4; ++i) {
        struct ExternalNode *ToCheck;
        assert(readResultNode(NRS, &ToCheck));
        assert(ToCheck->Id == NodeIds[i]);
        assert(ToCheck->AttributesNumber == 2);
        assert(ToCheck->Attributes[0].Type == INT);
        assert(ToCheck->Attributes[0].Id == 0);
        assert(ToCheck->Attributes[0].Value.IntValue == 11 + i);
        assert(ToCheck->Attributes[1].Type == STRING);
        assert(ToCheck->Attributes[1].Id == 1);
        assert(strcmp(ToCheck->Attributes[1].Value.StringAddr, NodeNames[i]) == 0);
        deleteExternalNode(&ToCheck);
        moveToNextNode(NRS);
    }
    deleteNodeResultSet(&NRS);
    NLRS = readNodeLink(Controller, &RNLR);
    assert(nodeLinkResultSetGetSize(NLRS) == 1);
    struct ExternalNodeLink *ToCheck;
    assert(readResultNodeLink(NLRS, &ToCheck));
    assert(ToCheck->Id == NodeLinkIds[2]);
    assert(ToCheck->LeftNodeId = NodeIds[2]);
    assert(ToCheck->RightNodeId = NodeIds[3]);
    assert(ToCheck->Weight == LinkWeights[2]);
    deleteExternalNodeLink(&ToCheck);
    deleteNodeLinkResultSet(&NLRS);

    struct DeleteSchemeRequest DGR;
    DGR.Name = "G";
    deleteScheme(Controller, &DGR);
    endWork(Controller);

    fprintf(stderr, "testReadNodesAndLinksWithReopen finished\n\n");
}
