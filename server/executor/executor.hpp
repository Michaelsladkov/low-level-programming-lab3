#pragma once

#include <ast.hpp>
#include <dto.hpp>
#include "execution_context.hpp"

class Executor {
    StorageController* DBController;
    void getSchemeInfo(const char* SchemeName, ExecutionContext* context);
    void processMatchExpr(MatchExpressionNode* Expr, ExecutionContext* context);
    size_t createNewScheme(VariablePatternMatchNode* MatchWithNewScheme, ExecutionContext* context);
    size_t createNewNode(VariablePatternMatchNode* MatchWithNewNode, ExecutionContext* context);
    size_t createNewRelation(RelationMatchNode* Rel, ExecutionContext* context, size_t left, size_t right);
    size_t processCreateExpr(CreateExpressionNode* Expr, ExecutionContext* context);
    ReadNodeRequest* generateReadNodeRequest(VariableMatchNode* expr, ExecutionContext* context);
public:
    Executor();
    Response* processRequest(RequestDTO* Req);
    ~Executor();
};