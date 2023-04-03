#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include "common_types.h"
#include "file_work/file_allocator.h"

struct AttributeDescription {
    enum ATTRIBUTE_TYPE Type;
    struct MyString Name;
    struct OptionalFullAddr Next;
    size_t AttributeId;
};

struct Attribute {
    size_t AttributeId;
    enum ATTRIBUTE_TYPE Type;
    union AttributeValue {
        int32_t IntValue;
        float FloatValue;
        struct MyString StringValue;
        bool BoolValue;
    } Value;
    struct OptionalFullAddr Next;
};

struct String {
    size_t Length;
    char Value[];
};

struct Node {
    size_t Id;
    size_t SchemeId;
    bool Deleted;
    struct OptionalFullAddr Attributes;
    struct OptionalFullAddr Next;
    struct OptionalFullAddr Previous;
};

struct NodeLink {
    size_t Id;
    bool Deleted;
    size_t LeftNodeId;
    size_t RightNodeId;
    enum ConnectionType Type;
    float Weight;
    struct MyString Name;
    struct OptionalFullAddr Next;
    struct OptionalFullAddr Previous;
};

struct Scheme {
    size_t Id;
    size_t NodeNumber;
    size_t AttributeNumber;
    struct OptionalFullAddr AttributesDecription;
    struct MyString Name;
    struct OptionalFullAddr Nodes;
    struct OptionalFullAddr LastNode;
    struct OptionalFullAddr Next;
    struct OptionalFullAddr Previous;
    size_t LazyDeletedNodeNumber;
    size_t NodesPlaceable;
    size_t PlacedNodes;
};

struct GraphStorage {
    size_t SchemeNumber;
    size_t NextSchemeId;
    size_t NextNodeId;
    size_t NextNodeLinkId;
    struct OptionalFullAddr Schemes;
    struct OptionalFullAddr LastScheme;
    size_t LinkNumber;
    struct OptionalFullAddr Links;
    struct OptionalFullAddr LastLink;
    size_t LazyDeletedLinkNumber;
    size_t LinksPlaceable;
    size_t PlacedLinks;
};
