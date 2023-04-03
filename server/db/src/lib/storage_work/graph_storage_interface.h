#pragma once

#include "external_representations.h"
#include "logical_structures.h"
#include "request.h"
#include "request_result.h"

struct StorageController;

struct StorageController *beginWork(char *DataFile);
void endWork(struct StorageController *Controller);
void dropStorage(struct StorageController *Controller);

size_t createScheme(struct StorageController *const Controller,
                   const struct CreateSchemeRequest *const Request);
size_t createNode(struct StorageController *const Controller,
                  const struct CreateNodeRequest *const Request);
size_t createNodeLink(struct StorageController *const Controller,
                      const struct CreateNodeLinkRequest *const Request);

struct NodeResultSet *readNode(const struct StorageController *const Controller,
                               const struct ReadNodeRequest *const Request);
struct NodeLinkResultSet *readNodeLink(const struct StorageController *const Controller,
                                       const struct ReadNodeLinkRequest *const Request);
struct SchemeResultSet *readScheme(const struct StorageController *const Controller,
                                 const struct ReadSchemeRequest *const Request);

size_t updateNode(const struct StorageController *const Controller,
                  const struct UpdateNodeRequest *const Request);
size_t updateNodeLink(const struct StorageController *const Controller,
                      const struct UpdateNodeLinkRequest *const Request);

size_t deleteNode(const struct StorageController *const Controller,
                  const struct DeleteNodeRequest *const Request);
size_t deleteNodeLink(const struct StorageController *const Controller,
                      const struct DeleteNodeLinkRequest *const Request);
size_t deleteScheme(struct StorageController *const Controller,
                   const struct DeleteSchemeRequest *const Request);
