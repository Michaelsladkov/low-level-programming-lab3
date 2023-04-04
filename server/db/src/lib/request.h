#pragma once

#include <inttypes.h>

#include "external_representations.h"

enum RequestType { CREATE, READ, UPDATE, DELETE };

enum CreateRequestType {
    CREATE_NODE,
    CREATE_NODE_LINK,
    CREATE_Scheme,
};

enum SchemeIdType { Scheme_ID, Scheme_NAME };

union SchemeId {
    size_t SchemeId;
    char *SchemeName;
};

struct CreateNodeRequest {
    enum SchemeIdType SchemeIdType;
    union SchemeId SchemeId;
    struct ExternalAttribute *Attributes;
};

struct CreateNodeLinkRequest {
    size_t LeftNodeId;
    size_t RightNodeId;
    enum ConnectionType Type;
    char* Name;
    float Weight;
};

struct CreateSchemeRequest {
    char *Name;
    struct ExternalAttributeDescription *AttributesDescription;
};

struct CreateRequest {
    enum CreateRequestType Type;
    union CreateRequestData {
        struct CreateNodeRequest Node;
        struct CreateNodeLinkRequest NodeLink;
        struct CreateSchemeRequest Scheme;
    } Data;
};

enum ReadRequestType {
    READ_NODE,
    READ_NODE_LINK,
    READ_Scheme,
};

struct FloatFilter {
    bool HasMin;
    float Min;
    bool HasMax;
    float Max;
};

struct IntFilter {
    bool HasMin;
    int32_t Min;
    bool HasMax;
    int32_t Max;
};

enum StringFilterType { STRLEN_RANGE, STRING_EQUAL, CONTAINS };

struct StringFilter {
    enum StringFilterType Type;
    union StringFilterData {
        struct IntFilter StrlenRange;
        char *StringEqual;
    } Data;
};

struct BoolFilter {
    bool Value;
};

enum LINKING_RELATION { HAS_LINK_TO, HAS_LINK_FROM };

struct LinkFilter {
    size_t NodeId;
    enum LINKING_RELATION Relation;
    struct FloatFilter WeightFilter;
};

enum FILTER_TYPE { INT_FILTER, FLOAT_FILTER, BOOL_FILTER, STRING_FILTER, LINK_FILTER };

struct AttributeFilter {
    size_t AttributeId;
    enum FILTER_TYPE Type;
    union AttributeFilterData {
        struct FloatFilter Float;
        struct IntFilter Int;
        struct StringFilter String;
        struct BoolFilter Bool;
        struct LinkFilter Link;
    } Data;
    struct AttributeFilter *Next;
};

struct ReadNodeRequest {
    enum SchemeIdType SchemeIdType;
    union SchemeId SchemeId;
    struct AttributeFilter *AttributesFilterChain;
    bool ById;
    size_t Id;
};

enum NodeLinkRequestType { BY_ID, BY_LEFT_NODE_ID, BY_RIGHT_NODE_ID, BY_STRING_FILTER, ALL };

struct ReadNodeLinkRequest {
    enum NodeLinkRequestType Type;
    struct StringFilter NameFilter;
    size_t Id;
};

struct ReadSchemeRequest {
    char *Name;
};

struct ReadRequest {
    enum ReadRequestType Type;
    union ReadRequestData {
        struct ReadNodeRequest Node;
        struct ReadNodeLinkRequest NodeLink;
        struct ReadSchemeRequest Scheme;
    } Data;
};

enum UpdateRequestType { UPDATE_NODE, UPDATE_NODE_LINK };

struct UpdateNodeRequest {
    enum SchemeIdType SchemeIdType;
    union SchemeId SchemeId;
    const struct AttributeFilter *AttributesFilterChain;
    bool ById;
    size_t Id;
    struct ExternalAttribute *Attributes;
    size_t UpdatedAttributesNumber;
};

struct UpdateNodeLinkRequest {
    size_t Id;
    bool UpdateType;
    enum ConnectionType Type;
    bool UpdateWeight;
    float Weight;
};

struct UpdateRequest {
    enum UpdateRequestType Type;
    union UpdateRequestData {
        struct UpdateNodeRequest Node;
        struct UpdateNodeLinkRequest NodeLink;
    } Data;
};

enum DeleteRequestType {
    DELETE_NODE,
    DELETE_NODE_LINK,
    DELETE_Scheme,
};

struct DeleteNodeRequest {
    enum SchemeIdType SchemeIdType;
    union SchemeId SchemeId;
    bool ById;
    size_t Id;
    struct AttributeFilter *AttributesFilterChain;
};

struct DeleteNodeLinkRequest {
    enum NodeLinkRequestType Type;
    struct StringFilter NameFilter;
    size_t Id;
};

struct DeleteSchemeRequest {
    char *Name;
};

struct DeleteRequest {
    enum DeleteRequestType Type;
    union DeleteRequestData {
        struct DeleteNodeRequest Node;
        struct DeleteNodeLinkRequest NodeLink;
        struct DeleteSchemeRequest Scheme;
    } Data;
};

struct Request {
    enum RequestType Type;
    char *SchemeName;
    union RequestData {
        struct CreateRequest Create;
        struct ReadRequest Read;
        struct UpdateRequest Update;
        struct DeleteRequest Delete;
    } Data;
};
