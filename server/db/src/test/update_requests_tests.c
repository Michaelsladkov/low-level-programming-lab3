#include "update_requests_tests.h"

#include "file_work/file_allocator.h"
#include "file_work/file_allocator_internals.h"
#include "storage_work/storage_controller.h"
#include "storage_work/graph_storage_interface.h"
#include "request.h"

#include "assert.h"
#include "stdio.h"
#include "string.h"

/*
Create Scheme G with nodes: A, B, C, D, E, F
Fieilds:
    String name ("Node A", "Node B" ...)
    Int id: 1, 2, 3, 4, 5, 6
    Bool updated: false for all on init
    Float load: 0 for all on init
Also create UNIDIRECTIONAL Links with weight 1: AB AC CD CB BF
*/

static size_t initScheme(struct StorageController * const Controller) {
    struct ExternalAttributeDescription AttrDescs[4];
    AttrDescs[0].AttributeId = 0;
    AttrDescs[0].Name = "name";
    AttrDescs[0].Type = STRING;
    AttrDescs[0].Next = &(AttrDescs[1]);
    AttrDescs[1].AttributeId = 1;
    AttrDescs[1].Name = "id";
    AttrDescs[1].Type = INT;
    AttrDescs[1].Next = &(AttrDescs[2]);
    AttrDescs[2].AttributeId = 2;
    AttrDescs[2].Name = "updated";
    AttrDescs[2].Type = BOOL;
    AttrDescs[2].Next = &(AttrDescs[3]);
    AttrDescs[3].AttributeId = 3;
    AttrDescs[3].Name = "load";
    AttrDescs[3].Type = FLOAT;
    AttrDescs[3].Next = NULL;
    struct CreateSchemeRequest CGR = {
        .Name = "G",
        .AttributesDescription = AttrDescs
    };
    size_t SchemeId = createScheme(Controller, &CGR);
    fprintf(stderr, "Initialized Scheme with id %zu\n", SchemeId);

    const char* NodeNames[6] = {
        "Node A",
        "Node B",
        "Node C",
        "Node D",
        "Node E",
        "Node F"
    };
    struct ExternalAttribute NodeAttributes[4] = {
        {
            .Id = 0,
            .Type = STRING
        },
        {
            .Id = 1,
            .Type = INT
        },
        {
            .Id = 2,
            .Type = BOOL,
            .Value.BoolValue = false
        },
        {
            .Id = 3,
            .Type = FLOAT,
            .Value.FloatValue = 0.0f
        }
    };
    struct CreateNodeRequest CNR = {
        .SchemeId.SchemeId = SchemeId,
        .SchemeIdType = Scheme_ID,
        .Attributes = NodeAttributes
    };
    size_t NodeIds[6];
    for (uint8_t i = 0; i < 6; ++i) {
        NodeAttributes[0].Value.StringAddr = NodeNames[i];
        NodeAttributes[1].Value.IntValue = i + 1;
        NodeIds[i] = createNode(Controller, &CNR);
        fprintf(stderr, "Created node %s, with id %zu\n", NodeNames[i], NodeIds[i]);
    }
    struct CreateNodeLinkRequest CNLR = {
        .Type = UNIDIRECTIONAL,
        .Weight = 1.0f,
        .Name = "Link"
    };
    // AB Link
    CNLR.LeftNodeId = NodeIds[0];
    CNLR.RightNodeId = NodeIds[1];
    size_t LinkId = createNodeLink(Controller, &CNLR);
    fprintf(stderr, "Created Node Link AB with id: %zu\n", LinkId);
    // AC Link
    CNLR.LeftNodeId = NodeIds[0];
    CNLR.RightNodeId = NodeIds[2];
    LinkId = createNodeLink(Controller, &CNLR);
    fprintf(stderr, "Created Node Link AC with id: %zu\n", LinkId);
    // CD Link
    CNLR.LeftNodeId = NodeIds[2];
    CNLR.RightNodeId = NodeIds[3];
    LinkId = createNodeLink(Controller, &CNLR);
    fprintf(stderr, "Created Node Link CD with id: %zu\n", LinkId);
    // CB Link
    CNLR.LeftNodeId = NodeIds[2];
    CNLR.RightNodeId = NodeIds[1];
    LinkId = createNodeLink(Controller, &CNLR);
    fprintf(stderr, "Created Node Link CB with id: %zu\n", LinkId);
    // BF Link
    CNLR.LeftNodeId = NodeIds[1];
    CNLR.RightNodeId = NodeIds[5];
    LinkId = createNodeLink(Controller, &CNLR);
    fprintf(stderr, "Created Node Link BF with id: %zu\n", LinkId);
    fprintf(stderr, "Scheme initialization finished\n");
    return SchemeId;
}

static void removeScheme(struct StorageController * const Controller) {
    struct DeleteSchemeRequest DGR = {
        .Name = "G"
    };
    deleteScheme(Controller, &DGR);
}

void testUpdateOneNode() {
    fprintf(stderr, "testUpdateOneNode started\n\n");

    struct StorageController *Controller = beginWork("test.bin");
    size_t SchemeId = initScheme(Controller);

    struct AttributeFilter NodeAFilter = {
        .AttributeId = 0,
        .Type = STRING_FILTER,
        .Next = NULL,
        .Data.String.Type = STRING_EQUAL,
        .Data.String.Data.StringEqual = "Node A"
    };
    struct ExternalAttribute AttrsToUpdate[2] = {
        {
            .Id = 3,
            .Type = FLOAT,
            .Value.FloatValue = 3.1415f
        },
        {
            .Id = 2,
            .Type = BOOL,
            .Value.BoolValue = true
        }
    };
    struct UpdateNodeRequest UNR = {
        .Attributes = AttrsToUpdate,
        .AttributesFilterChain = &NodeAFilter,
        .SchemeIdType = Scheme_ID,
        .SchemeId.SchemeId = SchemeId,
        .UpdatedAttributesNumber = 2,
        .ById = false
    };
    size_t Upd = updateNode(Controller, &UNR);
    assert(Upd == 1);
    struct ReadNodeRequest RNR = {
        .SchemeIdType = Scheme_ID,
        .SchemeId.SchemeId = SchemeId,
        .AttributesFilterChain = NULL,
        .ById = false
    };
    struct NodeResultSet* Result = readNode(Controller, &RNR);
    struct ExternalNode *NodeA;
    readResultNode(Result, &NodeA);
    assert(strcmp(NodeA->Attributes[0].Value.StringAddr, "Node A") == 0);
    assert(NodeA->Attributes[1].Value.IntValue == 1);
    assert(NodeA->Attributes[2].Value.BoolValue == true);
    printf("Node A load: %f\n", NodeA->Attributes[3].Value.FloatValue);
    assert(NodeA->Attributes[3].Value.FloatValue == 3.1415f);
    deleteExternalNode(&NodeA);
    moveToNextNode(Result);
    for (uint8_t i = 1; i < nodeResultSetGetSize(Result); ++i) {
        struct ExternalNode* Node;
        readResultNode(Result, &Node);
        assert(Node->Attributes[1].Value.IntValue == i + 1);
        assert(Node->Attributes[2].Value.BoolValue == false);
        assert(Node->Attributes[3].Value.FloatValue == 0.0f);
        deleteExternalNode(&Node);
        moveToNextNode(Result);
    }
    deleteNodeResultSet(&Result);
    removeScheme(Controller);
    endWork(Controller);
    fprintf(stderr, "testUpdateOneNode finished\n\n");
}

void testUpdateNodeWithString() {
    fprintf(stderr, "testUpdateNodeWithString started\n\n");

    struct StorageController *Controller = beginWork("test.bin");
    size_t SchemeId = initScheme(Controller);

    struct AttributeFilter NodeAFilter = {
        .AttributeId = 0,
        .Type = STRING_FILTER,
        .Next = NULL,
        .Data.String.Type = STRING_EQUAL,
        .Data.String.Data.StringEqual = "Node A"
    };
    struct ExternalAttribute AttrsToUpdate[2] = {
        {
            .Id = 0,
            .Type = STRING,
            .Value.StringAddr = "Noda"
        },
        {
            .Id = 2,
            .Type = BOOL,
            .Value.BoolValue = true
        }
    };
    struct UpdateNodeRequest UNR = {
        .Attributes = AttrsToUpdate,
        .AttributesFilterChain = &NodeAFilter,
        .SchemeIdType = Scheme_ID,
        .SchemeId.SchemeId = SchemeId,
        .UpdatedAttributesNumber = 2,
        .ById = false
    };
    fprintf(stderr, "Blocks before update\n");
    debugPrintFile(Controller->Allocator);
    size_t Upd = updateNode(Controller, &UNR);
    assert(Upd == 1);
    struct ReadNodeRequest RNR = {
        .SchemeIdType = Scheme_ID,
        .SchemeId.SchemeId = SchemeId,
        .AttributesFilterChain = &NodeAFilter,
        .ById = false
    };
    NodeAFilter.Data.String.Data.StringEqual = "Noda";
    struct NodeResultSet* Result = readNode(Controller, &RNR);
    assert(nodeResultSetGetSize(Result));
    struct ExternalNode *NodeA;
    readResultNode(Result, &NodeA);
    assert(strcmp(NodeA->Attributes[0].Value.StringAddr, "Noda") == 0);
    assert(NodeA->Attributes[1].Value.IntValue == 1);
    assert(NodeA->Attributes[2].Value.BoolValue == true);
    deleteExternalNode(&NodeA);
    deleteNodeResultSet(&Result);
    fprintf(stderr, "Blocks after update\n");
    debugPrintFile(Controller->Allocator);

    removeScheme(Controller);
    endWork(Controller);

    fprintf(stderr, "testUpdateNodeWithString finished\n\n");
}

void testUpdateSetOfNodes() {
    fprintf(stderr, "testUpdateSetOfNodes started\n\n");

    struct StorageController *Controller = beginWork("test.bin");
    size_t SchemeId = initScheme(Controller);

    struct AttributeFilter NodeCFilter = {
        .AttributeId = 0,
        .Type = STRING_FILTER,
        .Next = NULL,
        .Data.String.Type = STRING_EQUAL,
        .Data.String.Data.StringEqual = "Node C"
    };
    struct ReadNodeRequest ReadC = {
        .SchemeIdType = Scheme_ID,
        .SchemeId.SchemeId = SchemeId,
        .AttributesFilterChain = &NodeCFilter,
        .ById = false,
    };
    struct NodeResultSet* CResult = readNode(Controller, &ReadC);
    struct ExternalNode* NodeC;
    readResultNode(CResult, &NodeC);
    size_t CId = NodeC->Id;
    deleteExternalNode(&NodeC);
    deleteNodeResultSet(&CResult);
    
    struct AttributeFilter LinkedWithCFilter = {
        .Type = LINK_FILTER,
        .Next = NULL,
        .Data.Link.Relation = HAS_LINK_FROM,
        .Data.Link.NodeId = CId,
        .Data.Link.WeightFilter = {0}
    };
    struct ExternalAttribute AttrsToUpdate = {
        .Id = 2,
        .Type = BOOL,
        .Value.BoolValue = true
    };
    struct UpdateNodeRequest UNR = {
        .Attributes = &AttrsToUpdate,
        .AttributesFilterChain = &LinkedWithCFilter,
        .SchemeIdType = Scheme_ID,
        .SchemeId.SchemeId = SchemeId,
        .UpdatedAttributesNumber = 1
    };
    assert(updateNode(Controller, &UNR) == 3);

    struct ReadNodeRequest RNR = {
        .SchemeIdType = Scheme_ID,
        .SchemeId.SchemeId = SchemeId,
        .AttributesFilterChain = NULL,
        .ById = false
    };
    struct NodeResultSet* Result = readNode(Controller, &RNR);
    struct ExternalNode *NodeA;
    for (uint8_t i = 0; i < nodeResultSetGetSize(Result); ++i) {
        struct ExternalNode* Node;
        readResultNode(Result, &Node);
        assert(Node->Attributes[1].Value.IntValue == i + 1);
        if (strcmp(Node->Attributes[0].Value.StringAddr, "Node D") == 0 || strcmp(Node->Attributes[0].Value.StringAddr, "Node B") == 0 || strcmp(Node->Attributes[0].Value.StringAddr, "Node A") == 0) {
            assert(Node->Attributes[2].Value.BoolValue == true);
        } else {
            assert(Node->Attributes[2].Value.BoolValue == false);
        }
        deleteExternalNode(&Node);
        moveToNextNode(Result);
    }
    deleteNodeResultSet(&Result);
    removeScheme(Controller);
    endWork(Controller);

    fprintf(stderr, "testUpdateSetOfNodes finished\n\n");
}

void testUpdateLink() {
    fprintf(stderr, "testUpdateLink started\n\n");

    struct StorageController *Controller = beginWork("test.bin");
    size_t SchemeId = initScheme(Controller);

    struct ReadNodeLinkRequest GetAllLinksReq = {
        .Type = ALL,
    };
    struct NodeLinkResultSet* AllLinks = readNodeLink(Controller, &GetAllLinksReq);
    moveToNextNodeLink(AllLinks);
    struct ExternalNodeLink* Link2;
    readResultNodeLink(AllLinks, &Link2);
    size_t Link2Id = Link2->Id;
    deleteExternalNodeLink(&Link2);
    deleteNodeLinkResultSet(&AllLinks);

    struct UpdateNodeLinkRequest UpdateLink2Req = {
        .UpdateType = true,
        .Type = DIRECTIONAL,
        .UpdateWeight = true,
        .Weight = 5.0f,
        .Id = Link2Id
    };
    assert(updateNodeLink(Controller, &UpdateLink2Req) == 1);

    AllLinks = readNodeLink(Controller, &GetAllLinksReq);
    for (uint8_t i = 0; i < nodeLinkResultSetGetSize(AllLinks); ++i) {
        struct ExternalNodeLink* Link;
        readResultNodeLink(AllLinks, &Link);
        if (Link->Id == Link2Id) {
            assert(Link->Type == DIRECTIONAL);
            assert(Link->Weight == 5.0f);
        } else {
            assert(Link->Type == UNIDIRECTIONAL);
            assert(Link->Weight == 1.0f);
        }
        deleteExternalNodeLink(&Link);
        moveToNextNodeLink(AllLinks);
    }
    deleteNodeLinkResultSet(&AllLinks);

    removeScheme(Controller);
    endWork(Controller);

    fprintf(stderr, "testUpdateLink finished\n\n");
}
