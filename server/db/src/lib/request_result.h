#pragma once

#include "external_representations.h"

struct NodeResultSet {
    const struct StorageController *Controller;
    struct OptionalFullAddr SchemeAddr;
    size_t Cnt;
    size_t Index;
    struct OptionalFullAddr *NodeAddrs;
};

struct NodeLinkResultSet {
    const struct StorageController *Controller;
    size_t Cnt;
    size_t Index;
    struct OptionalFullAddr *LinkAddrs;
};

struct SchemeResultSet {
    const struct StorageController *Controller;
    size_t Cnt;
    size_t Index;
    struct OptionalFullAddr *SchemeAddrs;
};

bool readResultNode(struct NodeResultSet *ResultSet, struct ExternalNode **Node);
bool moveToNextNode(struct NodeResultSet *ResultSet);
bool moveToPreviousNode(struct NodeResultSet *ResultSet);
bool hasNextNode(struct NodeResultSet *ResultSet);
bool hasPreviousNode(struct NodeResultSet *ResultSet);
bool nodeResultSetIsEmpty(struct NodeResultSet *ResultSet);
size_t nodeResultSetGetSize(struct NodeResultSet *ResultSet);
void deleteNodeResultSet(struct NodeResultSet **ResultSet);

bool readResultNodeLink(struct NodeLinkResultSet *ResultSet,
                        struct ExternalNodeLink **NodeLink);
bool moveToNextNodeLink(struct NodeLinkResultSet *ResultSet);
bool moveToPreviousNodeLink(struct NodeLinkResultSet *ResultSet);
bool hasNextNodeLink(struct NodeLinkResultSet *ResultSet);
bool hasPreviousNodeLink(struct NodeLinkResultSet *ResultSet);
bool nodeLinkResultSetIsEmpty(struct NodeLinkResultSet *ResultSet);
size_t nodeLinkResultSetGetSize(struct NodeLinkResultSet *ResultSet);
void deleteNodeLinkResultSet(struct NodeLinkResultSet **ResultSet);

bool readResultScheme(struct SchemeResultSet *ResultSet, struct ExternalScheme **Scheme);
bool moveToNextScheme(struct SchemeResultSet *ResultSet);
bool moveToPreviousScheme(struct SchemeResultSet *ResultSet);
bool hasNextScheme(struct SchemeResultSet *ResultSet);
bool hasPreviousScheme(struct SchemeResultSet *ResultSet);
bool schemeResultSetIsEmpty(struct SchemeResultSet *ResultSet);
size_t schemeResultSetGetSize(struct SchemeResultSet *ResultSet);
void deleteSchemeResultSet(struct SchemeResultSet **ResultSet);
