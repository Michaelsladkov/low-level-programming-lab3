#pragma once

#include <stdbool.h>
#include <stddef.h>

enum ATTRIBUTE_TYPE { INT, FLOAT, STRING, BOOL };

enum ConnectionType { DIRECTIONAL, UNIDIRECTIONAL };

struct OptionalFullAddr {
    bool HasValue;
    size_t BlockOffset;
    size_t DataOffset;
};

#define SMALL_STRING_LIMIT 48

struct MyString {
    size_t Length;
    union StringData {
        struct OptionalFullAddr DataPtr;
        char InlinedData[SMALL_STRING_LIMIT];
    } Data;
};

#define NULL_FULL_ADDR                                                                         \
    (struct OptionalFullAddr) { false, 0, 0 }

static inline struct OptionalFullAddr getOptionalFullAddr(size_t BlockOffset,
                                                          size_t DataOffset) {
    return (struct OptionalFullAddr){true, BlockOffset, DataOffset};
}

static inline bool isOptionalFullAddrsEq(struct OptionalFullAddr Addr1,
                                         struct OptionalFullAddr Addr2) {
    return ((Addr1.HasValue == Addr2.HasValue) && (Addr1.BlockOffset == Addr2.BlockOffset) &&
            (Addr1.DataOffset == Addr2.DataOffset));
}

static inline bool inOneBlock(struct OptionalFullAddr Addr1, struct OptionalFullAddr Addr2) {
    return Addr1.HasValue && Addr2.HasValue && (Addr1.BlockOffset == Addr2.BlockOffset);
}

static inline bool inDifferentBlocks(struct OptionalFullAddr Addr1,
                                     struct OptionalFullAddr Addr2) {
    return Addr1.HasValue && Addr2.HasValue && (Addr1.BlockOffset != Addr2.BlockOffset);
}
