#pragma once

#include <ast.hpp>

extern "C" {
    #include "../db/src/lib/graph_storage_lib.h"
}


#include <unordered_map>

struct ExecutionContext {
    std::unordered_map<std::string, struct NodeResultSet*> NodeSymTab;
    std::unordered_map<std::string, struct NodeLinkResultSet*> NodeLinkSymTab;
    std::unordered_map<std::string, struct ExternalScheme*> SchemeSymTab;
    std::unordered_map<std::string, std::string> NodeVarToScheme;
};