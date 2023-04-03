#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#include "common_types.h"

struct ExternalAttribute {
    enum ATTRIBUTE_TYPE Type;
    size_t Id;
    union ExternalAttributeValue {
        int32_t IntValue;
        float FloatValue;
        char *StringAddr;
        bool BoolValue;
    } Value;
};

struct ExternalAttributeDescription {
    enum ATTRIBUTE_TYPE Type;
    char *Name;
    size_t AttributeId;
    struct ExternalAttributeDescription *Next;
};

struct ExternalNode {
    size_t Id;
    size_t SchemeId;
    struct ExternalAttribute *Attributes;
    size_t AttributesNumber;
};

struct ExternalNodeLink {
    size_t Id;
    size_t LeftNodeId;
    size_t RightNodeId;
    enum ConnectionType Type;
    float Weight;
    char* Name;
};

struct ExternalScheme {
    size_t Id;
    char *Name;
    struct ExternalAttributeDescription *AttributesDescription;
    size_t AttributesDescriptionNumber;
    size_t NodesNumber;
};

void deleteExternalNode(struct ExternalNode **Node);
void deleteExternalNodeLink(struct ExternalNodeLink **Link);
void deleteExternalScheme(struct ExternalScheme **Scheme);