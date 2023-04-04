#include "executor.hpp"
#include "execution_context.hpp"

#include <ast.hpp>

#include <iostream>
#include <cstring>

void Executor::getSchemeInfo(const char* SchemeName, ExecutionContext* context) {
    std::cerr << "obtaining scheme info\n";
    if (context->SchemeSymTab.count(SchemeName)) {
        return;
    }
    ReadSchemeRequest RSR;
    RSR.Name = const_cast<char*>(SchemeName);
    SchemeResultSet *Result = readScheme(this->DBController, &RSR);
    ExternalScheme *SchemeInfo;
    if (!schemeResultSetGetSize(Result)) {
        return;
    }
    readResultScheme(Result, &SchemeInfo);
    std::cout << "read scheme under iterator\n";
    context->SchemeSymTab[SchemeName] = SchemeInfo;
}

ReadNodeRequest* Executor::generateReadNodeRequest(VariableMatchNode* expr, ExecutionContext* context) {
    std::cerr << "generating read node request\n";
    const char* SchemeName = expr->getSchemeName()->data();
    std::cerr << SchemeName << std::endl;
    this->getSchemeInfo(SchemeName, context);
    if (!context->SchemeSymTab.count(*(expr->getSchemeName()))) {
        return nullptr;
    }
    std::unordered_map<std::string, ExternalAttributeDescription> AttrNameToDescription;
    auto Scheme = context->SchemeSymTab.at(SchemeName);
    for (size_t i = 0; i < Scheme->AttributesDescriptionNumber; ++i) {
        auto attr = Scheme->AttributesDescription[i];
        AttrNameToDescription[attr.Name] = attr;
    }
    VariablePatternMatchNode *byPattern;
    VariableFilterMatchNode *byFilter;
    ReadNodeRequest *ret = new ReadNodeRequest();
    ret->ById = false;
    ret->SchemeIdType = Scheme_NAME;
    ret->SchemeId.SchemeName = const_cast<char*>(SchemeName);
    if (expr->toJson()["kind"] == "VariablePatternMatchNode") {
        byPattern = static_cast<VariablePatternMatchNode*>(expr);
        auto Patterns = byPattern->getPattern()->getAttributeList();
        auto FiltersNumber = Patterns.size();
        ret->AttributesFilterChain = new AttributeFilter[FiltersNumber];
        size_t FilterIndex = 0;
        for (auto &[name, value] : Patterns) {
            size_t AttrId = AttrNameToDescription[name].AttributeId;
            AttributeFilter *filter = const_cast<AttributeFilter*>(ret->AttributesFilterChain + FilterIndex);
            filter->AttributeId = AttrId;
            filter->Next = filter + 1;
            if (AttrNameToDescription[name].Type == INT) {
                filter->Type = INT_FILTER;
                filter->Data.Int.HasMax = true;
                filter->Data.Int.HasMin = true;
                filter->Data.Int.Max = static_cast<IntLiteralNode*>(value)->getValue();
                filter->Data.Int.Min = static_cast<IntLiteralNode*>(value)->getValue();
            }
            if (AttrNameToDescription[name].Type == FLOAT) {
                filter->Type = FLOAT_FILTER;
                filter->Data.Int.HasMax = true;
                filter->Data.Int.HasMin = true;
                filter->Data.Int.Max = static_cast<FloatLiteralNode*>(value)->getValue() + 1e-6f;
                filter->Data.Int.Min = static_cast<FloatLiteralNode*>(value)->getValue() - 1e-6f;
            }
            if (AttrNameToDescription[name].Type == BOOL) {
                filter->Type = BOOL_FILTER;
                filter->Data.Bool.Value = static_cast<BoolLiteralNode*>(value)->getValue();
            }
        }
        return ret;
    }
    if (expr->toJson()["kind"] == "VariableFilterMatchNode") {
        byFilter = static_cast<VariableFilterMatchNode*>(expr);
        const LogicalExpressionNode *LogicalExpr = (byFilter->getFilter()->getExpr());
        FilterByPassNode* FilterBp;
        AndOperationNode* AndOnFilters;
        ret->AttributesFilterChain = new AttributeFilter[2];
        if (LogicalExpr->toJson()["kind"] == "FilterByPassNode") {
            FilterBp = (FilterByPassNode*)(LogicalExpr);
            auto FilterFromAST = const_cast<FilterNode*>(FilterBp->getFilter());
            std::cout << FilterFromAST->toJson().dump(2) << std::endl;
            int ref = ((IntLiteralNode*)(FilterFromAST->getRHS()))->getValue();
            AttributeFilter *filter = ret->AttributesFilterChain;
            filter->Type = INT_FILTER;
            filter->Next = NULL;
            auto VarName = *(((VariableValueNode*)(FilterFromAST->getLHS()))->getFieldName());
            filter->AttributeId = AttrNameToDescription[VarName].AttributeId;
            if (FilterFromAST->getOperation() == GREATER) {
                filter->Data.Int.HasMax = false;
                filter->Data.Int.HasMin = true;
                filter->Data.Int.Min = ref - 1;
            }
            if (FilterFromAST->getOperation() == LESS) {
                filter->Data.Int.HasMin = false;
                filter->Data.Int.HasMax = true;
                filter->Data.Int.Max = ref + 1;
            }
            return ret;
        }
        if (LogicalExpr->toJson()["kind"] == "AndOperationNode") {
            AndOnFilters = (AndOperationNode*)(LogicalExpr);
            AttributeFilter *filter1 = ret->AttributesFilterChain;
            AttributeFilter *filter2 = filter1 + 1;
            filter1->Type = filter2->Type = INT_FILTER;
            filter1->Next = filter2;
            filter2->Next = NULL;
            auto LeftExpr = (FilterByPassNode*)AndOnFilters->getLeft();
            auto RightExpr = (FilterByPassNode*)AndOnFilters->getRight();
            auto LeftASTFilter = LeftExpr->getFilter();
            auto RightASTFilter = RightExpr->getFilter();
            int LeftRef = ((IntLiteralNode*)(LeftASTFilter->getRHS()))->getValue();
            int RightRef = ((IntLiteralNode*)(RightASTFilter->getRHS()))->getValue();
            auto LeftVarName = *(((VariableValueNode*)LeftASTFilter->getRHS())->getFieldName());
            auto RightVarName = *(((VariableValueNode*)RightASTFilter->getRHS())->getFieldName());
            filter1->AttributeId = AttrNameToDescription[LeftVarName].AttributeId;
            filter2->AttributeId = AttrNameToDescription[RightVarName].AttributeId;
            if (LeftASTFilter->getOperation() == GREATER) {
                filter1->Data.Int.HasMax = false;
                filter1->Data.Int.HasMin = true;
                filter1->Data.Int.Min = LeftRef - 1;
            }
            if (LeftASTFilter->getOperation() == LESS) {
                filter1->Data.Int.HasMin = false;
                filter1->Data.Int.HasMax = true;
                filter1->Data.Int.Max = LeftRef + 1;
            }
            if (RightASTFilter->getOperation() == GREATER) {
                filter2->Data.Int.HasMax = false;
                filter2->Data.Int.HasMin = true;
                filter2->Data.Int.Min = RightRef - 1;
            }
            if (RightASTFilter->getOperation() == LESS) {
                filter2->Data.Int.HasMin = false;
                filter2->Data.Int.HasMax = true;
                filter2->Data.Int.Max = RightRef + 1;
            }
            return ret;
        }
    }
    return nullptr;
}

void Executor::processMatchExpr(MatchExpressionNode* Expr, ExecutionContext* context) {
    std::cerr << "processing match expr\n";
    VariableMatchNode* LeftMatch = const_cast<VariableMatchNode*>(Expr->getLeftMatchNode());
    if (!context->NodeSymTab.count(*(LeftMatch->getVariableName()))) {
        std::cerr << "we are in if\n";
        ReadNodeRequest* req = generateReadNodeRequest(LeftMatch, context);
        if (req == nullptr) {
            std::cout << "failed to generate node req\n";
            return;
        }
        std::cout << "reading nodes\n";
        NodeResultSet* res = readNode(DBController, req);
        if (nodeResultSetGetSize(res)) {
            context->NodeSymTab[*(LeftMatch->getVariableName())] = res;
        }
        std::cout << nodeResultSetGetSize(context->NodeSymTab[*(LeftMatch->getVariableName())]) << std::endl;
        delete[] req->AttributesFilterChain;
        delete req;
    }
    VariableMatchNode* RightMatch = const_cast<VariableMatchNode*>(Expr->getLeftMatchNode());
    RelationMatchNode* RelationMatch = const_cast<RelationMatchNode*>(Expr->getRelationMatchNode());
}

ATTRIBUTE_TYPE getTypeFromValueNode(ValueNode* Value) {
    IntLiteralNode *IntLit;
    FloatLiteralNode *FloatLit;
    BoolLiteralNode *BoolLit;
    StringLiteralNode *StringLit;
    if (Value->toJson()["kind"] == "IntLiteralNode") {
        return INT;
    }
    if (Value->toJson()["kind"] == "FloatLiteralNode") {
        return FLOAT;
    }
    if (Value->toJson()["kind"] == "BoolLiteralNode") {
        return BOOL;
    }
    if (Value->toJson()["kind"] == "StringLiteralNode") {
        return STRING;
    }
}

size_t Executor::createNewScheme(VariablePatternMatchNode* MatchWithNewScheme, ExecutionContext* context) {
    auto Attributes = MatchWithNewScheme->getPattern()->getAttributeList();
    size_t AttrNum = Attributes.size();
    std::string SchemeName(MatchWithNewScheme->getSchemeName()->data());
    CreateSchemeRequest CSR;
    CSR.Name = const_cast<char*>(SchemeName.data());
    CSR.AttributesDescription = new ExternalAttributeDescription[AttrNum];
    auto AttributesIterator = Attributes.begin();
    for (size_t i = 0; i < AttrNum; ++i) {
        CSR.AttributesDescription[i].AttributeId = i;
        std::string AttrName(AttributesIterator->first);
        std::cout << i << " " << AttrName << std::endl;
        CSR.AttributesDescription[i].Name = new char[AttrName.size() + 1];
        std::strcpy(CSR.AttributesDescription[i].Name, AttrName.data());
        CSR.AttributesDescription[i].Name[AttrName.size()] = 0;
        ATTRIBUTE_TYPE type = getTypeFromValueNode(AttributesIterator->second);
        CSR.AttributesDescription[i].Type = type;
        CSR.AttributesDescription[i].Next = i == AttrNum - 1 ? NULL : (CSR.AttributesDescription + i + 1);
        ++AttributesIterator;
    }
    size_t SchemeCreated = createScheme(DBController, &CSR);
    getSchemeInfo(SchemeName.data(), context);
    return SchemeCreated;
}

size_t Executor::createNewNode(VariablePatternMatchNode* MatchWithNewNode, ExecutionContext* context) {
    auto Attributes = MatchWithNewNode->getPattern()->getAttributeList();
    size_t AttrNum = Attributes.size();
    std::string SchemeName(MatchWithNewNode->getSchemeName()->data());
    CreateNodeRequest CNR;
    CNR.SchemeIdType = Scheme_NAME;
    CNR.SchemeId.SchemeName = const_cast<char*>(SchemeName.data());
    CNR.Attributes = new ExternalAttribute[AttrNum];
    auto AttributesIterator = Attributes.begin();
    for (size_t i = 0; i < AttrNum; ++i) {
        CNR.Attributes[i].Type = getTypeFromValueNode(AttributesIterator->second);
        for (size_t j = 0; j < context->SchemeSymTab[SchemeName]->AttributesDescriptionNumber; ++j) {
            auto d = context->SchemeSymTab[SchemeName]->AttributesDescription[j];
            if (AttributesIterator->first == d.Name) {
                CNR.Attributes[i].Id = d.AttributeId;
                break;
            }
        }
        switch (CNR.Attributes[i].Type) {
            case (INT):
                CNR.Attributes[i].Value.IntValue = ((IntLiteralNode*)(AttributesIterator->second))->getValue();
                break;
            case (FLOAT):
                CNR.Attributes[i].Value.FloatValue = ((FloatLiteralNode*)(AttributesIterator->second))->getValue();
                break;
            case (BOOL):
                CNR.Attributes[i].Value.FloatValue = ((BoolLiteralNode*)(AttributesIterator->second))->getValue();
                break;
            case (STRING):
                CNR.Attributes[i].Value.StringAddr = const_cast<char*>(((StringLiteralNode*)(AttributesIterator->second))->getValue()->c_str());
                break;
            default:
                break;
        }
        ++AttributesIterator;
    }
    std::cout << "AttrNum: " << AttrNum << std::endl;
    std::cout << "Creating node" << std::endl;
    size_t NodeCreated = createNode(DBController, &CNR);
    delete[] CNR.Attributes;
    std::cout << "Node created: " << NodeCreated << std::endl;
    return NodeCreated;
}

size_t Executor::createNewRelation(RelationMatchNode* Rel, ExecutionContext* context, size_t left, size_t right) {
    CreateNodeLinkRequest CNLR;
    if (Rel->getDirection() == FORWARD) {
        CNLR.LeftNodeId = left;
        CNLR.RightNodeId = right;
        CNLR.Type = DIRECTIONAL;
    }
    if (Rel->getDirection() == REVERSE) {
        CNLR.LeftNodeId = right;
        CNLR.RightNodeId = left;
        CNLR.Type = DIRECTIONAL;
    }
    if (Rel->getDirection() == ANY) {
        CNLR.LeftNodeId = left;
        CNLR.RightNodeId = right;
        CNLR.Type = UNIDIRECTIONAL;
    }
    CNLR.Name = const_cast<char*>(Rel->getRelationName()->c_str());
    return createNodeLink(DBController, &CNLR);
}

size_t Executor::processCreateExpr(CreateExpressionNode* Expr, ExecutionContext* context) {
    auto LeftNode = Expr->getLeftMatchNode();
    getSchemeInfo(((VariablePatternMatchNode*)LeftNode)->getSchemeName()->data(), context);
    size_t ObjectsCreated = 0;
    if (!context->SchemeSymTab.count(*(LeftNode->getSchemeName()))) {
        std::cout << "need to create scheme\n";
        ObjectsCreated += createNewScheme((VariablePatternMatchNode *)LeftNode, context) != 0 ? 1 : 0;
    }
    size_t LeftNodeId;
    if (!context->NodeSymTab.count(*(LeftNode->getVariableName()))) {
        LeftNodeId = createNewNode((VariablePatternMatchNode *)LeftNode, context);
        ObjectsCreated += LeftNodeId != 0 ? 1 : 0;
    } else {
        NodeResultSet* LeftRes = context->NodeSymTab[*(LeftNode->getVariableName())];
        ExternalNode* Node;
        readResultNode(LeftRes, &Node);
        LeftNodeId = Node->Id;
        deleteExternalNode(&Node);
    }

    auto RightNode = Expr->getRightMatchNode();
    if (RightNode == nullptr) return ObjectsCreated;
    if (!context->SchemeSymTab.count(*(RightNode->getSchemeName()))) {
        std::cout << "need to create scheme\n";
        ObjectsCreated += createNewScheme((VariablePatternMatchNode *)RightNode, context) != 0 ? 1 : 0;
    }
    size_t RightNodeId;
    if (!context->NodeSymTab.count(*(RightNode->getVariableName()))) {
        RightNodeId = createNewNode((VariablePatternMatchNode *)RightNode, context);
        ObjectsCreated += RightNodeId != 0 ? 1 : 0;
    } else {
        NodeResultSet* RightRes = context->NodeSymTab[*(RightNode->getVariableName())];
        ExternalNode* Node;
        readResultNode(RightRes, &Node);
        RightNodeId = Node->Id;
        deleteExternalNode(&Node);
    }

    auto Relation = (RelationMatchNode*)Expr->getRelationMatchNode();
    size_t RelId;
    if (!context->NodeLinkSymTab.count(*(Relation->getVariableName()))) {
        RelId = createNewRelation(Relation, context, LeftNodeId, RightNodeId);
        ObjectsCreated += RelId != 0 ? 1 : 0;
    }
    return ObjectsCreated;
}

Response* Executor::processRequest(RequestDTO* Req) {
    auto Expressions = Req->getRequestTree()->getExpressions();
    ExecutionContext context;
    for (auto Expr : Expressions) {
        CreateExpressionNode *AsCreate;
        MatchExpressionNode *AsMatch;
        if (Expr->toJson()["kind"] == "CreateExpressionNode") {
            std::cout << "processing create\n";
            AsCreate = (CreateExpressionNode*)Expr;
            size_t created = processCreateExpr(AsCreate, &context);
            std::cout << "created " << created << "objects\n";
        }
        if (Expr->toJson()["kind"] == "MatchExpressionNode") {
            AsMatch = (MatchExpressionNode*)Expr;
            processMatchExpr(AsMatch, &context);
        }
    }
    return nullptr;
}

Executor::Executor() {
    DBController = beginWork("test.bin");
}

Executor::~Executor() {
    endWork(DBController);
}
