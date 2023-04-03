#include "benchmarks.h"

#include "graph_storage_lib.h"
#include "file_work/file_allocator_internals.h" // to get file size
#include "storage_work/storage_controller.h" // to acess file allocator
#include "logical_structures.h" // to calculate physical structure size

#include <stdio.h>
#include <time.h>

void benchmarkNodeInsert(FILE *OutFile) {
    const char *CSVHeader = "Node Number, Insert time ns";
    FILE *CSVOut = OutFile;
    fprintf(CSVOut, "%s\n", CSVHeader);
    struct StorageController *Controller = beginWork("bench.bin");
    struct ExternalAttributeDescription SchemeAttributes[4] = {
        {.AttributeId = 0, .Name = "Node Name", .Type = STRING, .Next = SchemeAttributes + 1},
        {.AttributeId = 1, .Name = "Int value", .Type = INT, .Next = SchemeAttributes + 2},
        {.AttributeId = 2, .Name = "Bool value", .Type = BOOL, .Next = SchemeAttributes + 3},
        {.AttributeId = 3, .Name = "Float value", .Type = FLOAT, .Next = NULL}};
    struct CreateSchemeRequest CGR = {.AttributesDescription = SchemeAttributes, .Name = "G"};
    createScheme(Controller, &CGR);
    struct ExternalAttribute NodeAttributes[4] = {
        {.Id = 0, .Type = STRING, .Value.StringAddr = "Some String"},
        {
            .Id = 1,
            .Type = INT,
        },
        {.Id = 2, .Type = BOOL, .Value.BoolValue = false},
        {.Id = 3, .Type = FLOAT, .Value.FloatValue = 3.14f}};
    struct CreateNodeRequest CNR = {
        .Attributes = NodeAttributes, .SchemeIdType = Scheme_NAME, .SchemeId.SchemeName = "G"};
    for (int i = 0; i < 1000; ++i) {
        clock_t Begin = clock();
        for (int j = 0; j < 100; ++j) {
            NodeAttributes[1].Value.IntValue = i * 100 + j;
            createNode(Controller, &CNR);
        }
        clock_t End = clock();
        double TimeDiff = ((double)(End - Begin) * 10e9) / CLOCKS_PER_SEC;
        fprintf(CSVOut, "%d, %lf\n", (i + 1) * 100, TimeDiff);
    }
    struct DeleteSchemeRequest DGR = {.Name = "G"};
    deleteScheme(Controller, &DGR);
    endWork(Controller);
}

void benchmarkSelectByAttributes(FILE *OutFile) {
    const char *CSVHeader = "Total Node Number,Selected Node Number,Select time ns";
    FILE *CSVOut = OutFile;
    fprintf(CSVOut, "%s\n", CSVHeader);
    struct StorageController *Controller = beginWork("bench.bin");
    struct ExternalAttributeDescription SchemeAttributes[2] = {
        {.AttributeId = 0, .Name = "Int value", .Type = INT, .Next = SchemeAttributes + 1},
        {.AttributeId = 1, .Name = "Divisible by 3", .Next = NULL, .Type = BOOL}};
    struct CreateSchemeRequest CGR = {.AttributesDescription = SchemeAttributes, .Name = "G"};
    createScheme(Controller, &CGR);
    struct ExternalAttribute NodeAttributes[2] = {{
                                                      .Id = 0,
                                                      .Type = INT,
                                                  },
                                                  {.Id = 1, .Type = BOOL}};
    struct CreateNodeRequest CNR = {
        .Attributes = NodeAttributes, .SchemeIdType = Scheme_NAME, .SchemeId.SchemeName = "G"};
    struct AttributeFilter DivByThreeFilter = {
        .AttributeId = 1, .Type = BOOL_FILTER, .Data = true, .Next = NULL};
    struct ReadNodeRequest RNR = {.SchemeIdType = Scheme_NAME,
                                  .SchemeId.SchemeName = "G",
                                  .ById = false,
                                  .AttributesFilterChain = &DivByThreeFilter};
    for (int i = 0; i < 1000; ++i) {
        for (int j = 0; j < 100; ++j) {
            NodeAttributes[0].Value.IntValue = i * 100 + j;
            NodeAttributes[1].Value.BoolValue = ((j % 3) == 0);
            createNode(Controller, &CNR);
        }
        clock_t Begin = clock();
        struct NodeResultSet *NRS = readNode(Controller, &RNR);
        size_t ResultSetSize = nodeResultSetGetSize(NRS);
        while (hasNextNode(NRS)) {
            struct ExternalNode *Node;
            readResultNode(NRS, &Node);
            moveToNextNode(NRS);
            deleteExternalNode(&Node);
        }
        clock_t End = clock();
        deleteNodeResultSet(&NRS);
        double TimeDiff = ((double)(End - Begin) * 10e9) / CLOCKS_PER_SEC;
        fprintf(CSVOut, "%d,%zu,%lf\n", (i + 1) * 100, ResultSetSize, TimeDiff);
    }
    struct DeleteSchemeRequest DGR = {.Name = "G"};
    deleteScheme(Controller, &DGR);
    endWork(Controller);
}

void benchmarkDeleteElements(FILE *OutFile) {
    FILE *CSVOut = OutFile;
    const char *CSVHeader = "Total Node Number,Deleted Node Number,Delete Time ns";
    fprintf(CSVOut, "%s\n", CSVHeader);
    struct StorageController *Controller = beginWork("bench.bin");
    struct ExternalAttributeDescription SchemeAttributes[2] = {
        {.AttributeId = 0, .Name = "Int value", .Type = INT, .Next = SchemeAttributes + 1},
        {.AttributeId = 1, .Name = "To be Deleted", .Next = NULL, .Type = BOOL}};
    struct CreateSchemeRequest CGR = {.AttributesDescription = SchemeAttributes, .Name = "G"};
    createScheme(Controller, &CGR);
    struct ExternalAttribute NodeAttributes[2] = {{
                                                      .Id = 0,
                                                      .Type = INT,
                                                  },
                                                  {.Id = 1, .Type = BOOL}};
    struct CreateNodeRequest CNR = {
        .Attributes = NodeAttributes, .SchemeIdType = Scheme_NAME, .SchemeId.SchemeName = "G"};
    struct AttributeFilter DivByThreeFilter = {
        .AttributeId = 1, .Type = BOOL_FILTER, .Data = true, .Next = NULL};
    struct DeleteNodeRequest DNR = {.SchemeIdType = Scheme_NAME,
                                  .SchemeId.SchemeName = "G",
                                  .ById = false,
                                  .AttributesFilterChain = &DivByThreeFilter,};
    for (int i = 0; i < 1000; ++i) {
        for (int j = 0; j < 100; ++j) {
            NodeAttributes[0].Value.IntValue = i * 100 + j;
            NodeAttributes[1].Value.BoolValue = ((j % 5) == 0);
            createNode(Controller, &CNR);
        }
        clock_t Begin = clock();
        size_t DeletedNodeNumber = deleteNode(Controller, &DNR);
        clock_t End = clock();
        double TimeDiff = ((double)(End - Begin) * 10e9) / CLOCKS_PER_SEC;
        fprintf(CSVOut, "%d,%zu,%lf\n", (i + 1) * 100, DeletedNodeNumber, TimeDiff);
    }
    
    struct DeleteSchemeRequest DGR = {.Name = "G"};
    deleteScheme(Controller, &DGR);
    endWork(Controller);
}

void benchmarkUpdateProgressingElements(FILE *OutFile) {
    FILE *CSVOut = OutFile;
    const char *CSVHeader = "Total Node Number,Updated Node Number,Delete Time ns";
    fprintf(CSVOut, "%s\n", CSVHeader);
    struct StorageController *Controller = beginWork("bench.bin");
    struct ExternalAttributeDescription SchemeAttributes[3] = {
        {.AttributeId = 0, .Name = "Id", .Type = INT, .Next = SchemeAttributes + 1},
        {.AttributeId = 1, .Name = "Reminder of id to 7", .Next = SchemeAttributes + 2, .Type = INT},
        {.AttributeId = 2, .Name = "Updated", .Next = NULL, .Type = BOOL}
        };
    struct CreateSchemeRequest CGR = {.AttributesDescription = SchemeAttributes, .Name = "G"};
    createScheme(Controller, &CGR);
    struct ExternalAttribute NodeAttributes[3] = {{
                                                      .Id = 0,
                                                      .Type = INT,
                                                  },
                                                  {
                                                        .Id = 1,
                                                        .Type = INT
                                                  },
                                                  {
                                                        .Id = 2,
                                                        .Type = BOOL,
                                                        .Value.BoolValue = false
                                                  }
                                                  };
    struct CreateNodeRequest CNR = {
        .Attributes = NodeAttributes, .SchemeIdType = Scheme_NAME, .SchemeId.SchemeName = "G"};
    struct AttributeFilter UpdFilter = {
        .AttributeId = 1,
        .Type = INT_FILTER,
        .Next = NULL,
        .Data.Int = {
            .HasMax = true,
            .Max = 5,
            .HasMin = true,
            .Min = 1
        }
    };
    struct ExternalAttribute UpdatedAttributes[2] = {
        {
            .Id = 1,
            .Type = INT,
            .Value.IntValue = 2
        },
        {
            .Id = 2,
            .Type = BOOL,
            .Value.BoolValue = true
        }
    };
    struct UpdateNodeRequest UNR = {
        .SchemeIdType = Scheme_NAME,
        .SchemeId.SchemeName = "G",
        .AttributesFilterChain = &UpdFilter,
        .UpdatedAttributesNumber = 2,
        .Attributes = UpdatedAttributes,
        .ById = false
    };
    for (int i = 0; i < 1000; ++i) {
        for (int j = 0; j < 100; ++j) {
            NodeAttributes[0].Value.IntValue = i * 100 + j;
            NodeAttributes[1].Value.IntValue = ((i * 100) + j) % 7;
            createNode(Controller, &CNR);
        }
        clock_t Begin = clock();
        size_t UpdatedNodeNumber = updateNode(Controller, &UNR);
        clock_t End = clock();
        double TimeDiff = ((double)(End - Begin) * 10e9) / CLOCKS_PER_SEC;
        fprintf(CSVOut, "%d,%zu,%lf\n", (i + 1) * 100, UpdatedNodeNumber, TimeDiff);
    }
    
    struct DeleteSchemeRequest DGR = {.Name = "G"};
    deleteScheme(Controller, &DGR);
    endWork(Controller);
}

void benchmarkUpdateConstantElements(FILE *OutFile) {
    FILE *CSVOut = OutFile;
    const char *CSVHeader = "Total Node Number,Updated Node Number,Delete Time ns";
    fprintf(CSVOut, "%s\n", CSVHeader);
    struct StorageController *Controller = beginWork("bench.bin");
    struct ExternalAttributeDescription SchemeAttributes[3] = {
        {.AttributeId = 0, .Name = "Id", .Type = INT, .Next = SchemeAttributes + 1},
        {.AttributeId = 1, .Name = "Reminder of id to 7", .Next = SchemeAttributes + 2, .Type = INT},
        {.AttributeId = 2, .Name = "Updated", .Next = NULL, .Type = BOOL}
        };
    struct CreateSchemeRequest CGR = {.AttributesDescription = SchemeAttributes, .Name = "G"};
    createScheme(Controller, &CGR);
    struct ExternalAttribute NodeAttributes[3] = {{
                                                      .Id = 0,
                                                      .Type = INT,
                                                  },
                                                  {
                                                        .Id = 1,
                                                        .Type = INT
                                                  },
                                                  {
                                                        .Id = 2,
                                                        .Type = BOOL,
                                                        .Value.BoolValue = false
                                                  }
                                                  };
    struct CreateNodeRequest CNR = {
        .Attributes = NodeAttributes, .SchemeIdType = Scheme_NAME, .SchemeId.SchemeName = "G"};
    struct AttributeFilter UpdFilter[2] = {
        {
            .AttributeId = 1,
            .Type = INT_FILTER,
            .Next = UpdFilter + 1,
            .Data.Int = {
                .HasMax = true,
                .Max = 5,
                .HasMin = true,
                .Min = 1
            }
        },
        {
            .AttributeId = 2,
            .Type = BOOL_FILTER,
            .Next = NULL,
            .Data.Bool = {
                .Value = false
            }
        }
    };
    struct ExternalAttribute UpdatedAttributes[2] = {
        {
            .Id = 1,
            .Type = INT,
            .Value.IntValue = 2
        },
        {
            .Id = 2,
            .Type = BOOL,
            .Value.BoolValue = true
        }
    };
    struct UpdateNodeRequest UNR = {
        .SchemeIdType = Scheme_NAME,
        .SchemeId.SchemeName = "G",
        .AttributesFilterChain = UpdFilter,
        .UpdatedAttributesNumber = 2,
        .Attributes = UpdatedAttributes,
        .ById = false
    };
    for (int i = 0; i < 1000; ++i) {
        for (int j = 0; j < 100; ++j) {
            NodeAttributes[0].Value.IntValue = i * 100 + j;
            NodeAttributes[1].Value.IntValue = ((i * 100) + j) % 7;
            createNode(Controller, &CNR);
        }
        clock_t Begin = clock();
        size_t UpdatedNodeNumber = updateNode(Controller, &UNR);
        clock_t End = clock();
        double TimeDiff = ((double)(End - Begin) * 10e9) / CLOCKS_PER_SEC;
        fprintf(CSVOut, "%d,%zu,%lf\n", (i + 1) * 100, UpdatedNodeNumber, TimeDiff);
    }
    
    struct DeleteSchemeRequest DGR = {.Name = "G"};
    deleteScheme(Controller, &DGR);
    endWork(Controller);
}

void benchmarkFileSize(FILE *OutFile) {
    FILE* CSVOut = OutFile;
    const char* CSVHeader = "Operation Number,Node Size,File Size";
    fprintf(OutFile, "%s\n", CSVHeader);
    struct StorageController *Controller = beginWork("bench.bin");
    struct ExternalAttributeDescription AttrDesc = {
        .AttributeId = 0,
        .Name = "Ordinal",
        .Next = NULL,
        .Type = INT
    };
    struct CreateSchemeRequest CGR = {
        .AttributesDescription = &AttrDesc,
        .Name = "G"
    };
    createScheme(Controller, &CGR);
    struct ExternalAttribute NodeAttribute = {
        .Id = 0,
        .Type = INT
    };
    struct CreateNodeRequest CNR = {
        .SchemeIdType = Scheme_NAME,
        .SchemeId.SchemeName = "G",
        .Attributes = &NodeAttribute
    };
    struct ReadNodeRequest ReadAll = {
        .SchemeIdType = Scheme_NAME,
        .SchemeId.SchemeName = "G",
        .ById = false,
        .AttributesFilterChain = NULL
    };
    int OpNum = 0;
    size_t NodeNum = 0;
    struct NodeResultSet *NRS;
    for (int i = 0; i < 50000; ++i) {
        CNR.Attributes->Value.IntValue = i + 1;
        createNode(Controller, &CNR);
        NodeNum++;
        OpNum++;
        fprintf(CSVOut, "%d,%zu,%zu\n", OpNum, NodeNum * (sizeof(struct Node) + sizeof(struct Attribute)), getFileSize(Controller->Allocator));
    }
    struct AttributeFilter After10k = {
        .AttributeId = 0,
        .Type = INT_FILTER,
        .Next = NULL,
        .Data.Int = {
            .HasMax = false,
            .HasMin = true,
            .Min = 10000
        }
    };
    struct DeleteNodeRequest DNR = {
        .SchemeIdType = Scheme_NAME,
        .SchemeId.SchemeName = "G",
        .ById = false,
        .AttributesFilterChain = &After10k
    };
    NodeNum -= deleteNode(Controller, &DNR);
    fprintf(CSVOut, "%d,%zu,%zu\n", OpNum, NodeNum * (sizeof(struct Node) + sizeof(struct Attribute)),getFileSize(Controller->Allocator));
    for (int i = 0; i < 150000; ++i) {
        CNR.Attributes->Value.IntValue = 10000 + i + 1;
        createNode(Controller, &CNR);
        NodeNum++;
        OpNum++;
        fprintf(CSVOut, "%d,%zu,%zu\n", OpNum, NodeNum * (sizeof(struct Node) + sizeof(struct Attribute)), getFileSize(Controller->Allocator));
    }
    NRS = readNode(Controller, &ReadAll);
    if (nodeResultSetGetSize(NRS) == NodeNum) {
        fprintf(stderr, "File Size benchmark verified\n");
    }
    struct DeleteSchemeRequest DGR = {.Name = "G"};
    deleteScheme(Controller, &DGR);
    endWork(Controller);
}
