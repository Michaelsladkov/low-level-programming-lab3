#include "storage_controller.h"

#include <malloc.h>

#include "common_types.h"
#include "file_work/file_allocator.h"
#include "graph_storage_interface.h"
#include "logical_structures.h"
#include "params.h"

struct StorageController *beginWork(char *DataFile) {
    struct StorageController *Controller = malloc(sizeof(struct StorageController));
    Controller->Allocator = initFileAllocator(DataFile);
    size_t BlockLinksSize = sizeof(struct NodeLink) * LINKS_PER_BLOCK; 
    if (Controller->Allocator == NULL) {
        return NULL;
    }
    struct OptionalFullAddr MayBeStorageAddr = getFirstBlockData(Controller->Allocator);
    if (!MayBeStorageAddr.HasValue) {
        MayBeStorageAddr = allocate(Controller->Allocator, sizeof(struct GraphStorage) + BlockLinksSize);
        Controller->Storage.SchemeNumber = 0;
        Controller->Storage.LastScheme = NULL_FULL_ADDR;
        Controller->Storage.Schemes = NULL_FULL_ADDR;
        Controller->Storage.NextSchemeId = 1;
        Controller->Storage.NextNodeId = 1;
        Controller->Storage.NextNodeLinkId = 1;
        Controller->Storage.Links = getOptionalFullAddr(MayBeStorageAddr.BlockOffset,
                                       MayBeStorageAddr.DataOffset + sizeof(struct GraphStorage));
        Controller->Storage.Links.HasValue = false;
        Controller->Storage.LastLink = Controller->Storage.Links;
        Controller->Storage.LinksPlaceable = LINKS_PER_BLOCK;
        Controller->Storage.PlacedLinks = 0;
        Controller->Storage.LazyDeletedLinkNumber = 0;
        storeData(Controller->Allocator, MayBeStorageAddr, sizeof(struct GraphStorage),
                  &Controller->Storage);
    } else {
        fetchData(Controller->Allocator, MayBeStorageAddr, sizeof(struct GraphStorage),
                  &Controller->Storage);
    }
    Controller->StorageAddr = MayBeStorageAddr;
    return Controller;
}

void endWork(struct StorageController *Controller) {
    shutdownFileAllocator(Controller->Allocator);
    free(Controller);
}

size_t increaseSchemeNumber(struct StorageController *const Controller) {
    Controller->Storage.NextSchemeId++;
    Controller->Storage.SchemeNumber++;
    struct OptionalFullAddr StorageAddr = getFirstBlockData(Controller->Allocator);
    storeData(Controller->Allocator, StorageAddr, sizeof(struct GraphStorage),
              &Controller->Storage);
    return Controller->Storage.NextSchemeId;
}

size_t decreaseSchemeNumber(struct StorageController *const Controller) {
    Controller->Storage.SchemeNumber--;
    struct OptionalFullAddr StorageAddr = getFirstBlockData(Controller->Allocator);
    storeData(Controller->Allocator, StorageAddr, sizeof(struct GraphStorage),
              &Controller->Storage);
    return Controller->Storage.NextSchemeId;
}

size_t increaseNodeNumber(struct StorageController *const Controller) {
    Controller->Storage.NextNodeId++;
    struct OptionalFullAddr StorageAddr = getFirstBlockData(Controller->Allocator);
    storeData(Controller->Allocator, StorageAddr, sizeof(struct GraphStorage),
              &Controller->Storage);
    return Controller->Storage.NextNodeId;
}

size_t increaseNodeLinkNumber(struct StorageController *const Controller) {
    Controller->Storage.NextNodeLinkId++;
    struct OptionalFullAddr StorageAddr = getFirstBlockData(Controller->Allocator);
    storeData(Controller->Allocator, StorageAddr, sizeof(struct GraphStorage),
              &Controller->Storage);
    return Controller->Storage.NextSchemeId;
}

void updateLastScheme(struct StorageController *const Controller,
                     struct OptionalFullAddr LastSchemeAddr) {
    if (Controller->Storage.LastScheme.HasValue) {
        struct Scheme OldLastScheme;
        fetchData(Controller->Allocator, Controller->Storage.LastScheme, sizeof(struct Scheme),
                  &OldLastScheme);
        OldLastScheme.Next = LastSchemeAddr;
        storeData(Controller->Allocator, Controller->Storage.LastScheme, sizeof(struct Scheme),
                  &OldLastScheme);
    }
    Controller->Storage.LastScheme = LastSchemeAddr;
    struct OptionalFullAddr StorageAddr = getFirstBlockData(Controller->Allocator);
    storeData(Controller->Allocator, StorageAddr, sizeof(struct GraphStorage),
              &Controller->Storage);
}

void updateFirstScheme(struct StorageController *const Controller,
                      struct OptionalFullAddr FirstSchemeAddr) {
    Controller->Storage.Schemes = FirstSchemeAddr;
    struct OptionalFullAddr StorageAddr = getFirstBlockData(Controller->Allocator);
    storeData(Controller->Allocator, StorageAddr, sizeof(struct GraphStorage),
              &Controller->Storage);
}