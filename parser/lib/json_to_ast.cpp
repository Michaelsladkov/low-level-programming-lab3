#include "ast.hpp"

#include <unordered_map>
#include <nlohmann/json.hpp>

const std::unordered_map<std::string, FilterCheckOperation> FilterCheckOperationDict = {
    {"GREATER", GREATER},
    {"GREATER_OR_EQUAL", GREATER_OR_EQUAL},
    {"LESS", LESS},
    {"LESS_OR_EQUAL", LESS_OR_EQUAL},
    {"EQUAL", EQUAL},
    {"CONTAINS", CONTAINS}
};

VariableValueNode::VariableValueNode(json JSON) {
    VariableName = new std::string(JSON["VariableName"].get<std::string>());
    FieldName = new std::string(JSON["FieldName"].get<std::string>());
}

FloatLiteralNode::FloatLiteralNode(json JSON) {
    Value = JSON["Value"].get<float>();
}

BoolLiteralNode::BoolLiteralNode(json JSON) {
    Value = JSON["Value"].get<bool>();
}

IntLiteralNode::IntLiteralNode(json JSON) {
    Value = JSON["Value"].get<int>();
}

StringLiteralNode::StringLiteralNode(json JSON) {
    Value = new std::string(JSON["Value"].get<std::string>());
}

ValueNode* getValueNode(json JSON) {
    std::string Kind = JSON["kind"].get<std::string>();
    if (Kind == "VariableValueNode") {
        return new VariableValueNode(JSON);
    }
    if (Kind == "StringLiteralNode") {
        return new StringLiteralNode(JSON);
    }
    if (Kind == "BoolLiteralNode") {
        return new BoolLiteralNode(JSON);
    }
    if (Kind == "IntLiteralNode") {
        return new IntLiteralNode(JSON);
    }
    if (Kind == "FloatLiteralNode") {
        return new FloatLiteralNode(JSON);
    }
    return nullptr;
}

FilterNode::FilterNode(json JSON) {
    std::string OperationStr = JSON["Operation"].get<std::string>();
    Operation = FilterCheckOperationDict.at(OperationStr);
    RHS = getValueNode(JSON["RHS"]);
    LHS = getValueNode(JSON["LHS"]);
}

LogicalExpressionNode* getLogicalExpression(json JSON) {
    std::string Kind = JSON["kind"];
    if (Kind == "FilterByPassNode") {
        return new FilterByPassNode(JSON);
    }
    if (Kind == "NotOperationNode") {
        return new NotOperationNode(JSON);
    }
    if (Kind == "AndOperationNode") {
        return new AndOperationNode(JSON);
    }
    if (Kind == "OrOperationNode") {
        return new OrOperationNode(JSON);
    }
    return nullptr;
}

FilterByPassNode::FilterByPassNode(json JSON) {
    Wrapped = new FilterNode(JSON["Wrapped"]);
}

NotOperationNode::NotOperationNode(json JSON) {
    Operand = getLogicalExpression(JSON["Operand"]);
}

AndOperationNode::AndOperationNode(json JSON)
    : BinaryLogicalOperationNode(getLogicalExpression(JSON["LHS"]),
                                 getLogicalExpression(JSON["RHS"])) {}


OrOperationNode::OrOperationNode(json JSON)
    : BinaryLogicalOperationNode(getLogicalExpression(JSON["LHS"]),
                                 getLogicalExpression(JSON["RHS"])) {}

PredicateNode::PredicateNode(json JSON) {
    Body = getLogicalExpression(JSON["Body"]);
}

AttributeListNode::AttributeListNode(json JSON) {
    for (auto j : JSON["AttrList"].get<std::vector<json>>()) {
        auto name = j["name"].get<std::string>();
        ValueNode* value = getValueNode(j["Value"]);
        AttrList[name] = value;
    }
}

VariableMatchNode::VariableMatchNode(json JSON) {
    VariableName = new std::string(JSON["VariableName"].get<std::string>());
    SchemeName = new std::string(JSON["SchemeName"].get<std::string>());
}

VariablePatternMatchNode::VariablePatternMatchNode(json JSON) 
    : VariableMatchNode(JSON) {
    Pattern = new AttributeListNode(JSON["Pattern"]);
}

VariableFilterMatchNode::VariableFilterMatchNode(json JSON)
    : VariableMatchNode(JSON) {
    Predicate = new PredicateNode(JSON["Predicate"]);
}

VariableMatchNode* getVariableMatchNode(json JSON) {
    auto Kind = JSON["kind"].get<std::string>();
    if (Kind == "VariableMatchNode") {
        return new VariableMatchNode(JSON);
    }
    if (Kind == "VariablePatternMatchNode") {
        return new VariablePatternMatchNode(JSON);
    }
    if (Kind == "VariableFilterMatchNode") {
        return new VariableFilterMatchNode(JSON);
    }
    return nullptr;
}

const std::unordered_map<std::string, RelationDirection> RelationDirectionDict = {
    {"FORWARD",FORWARD},
    {"REVERSE", REVERSE},
    {"ANY", ANY}  
};

RelationMatchNode::RelationMatchNode(json JSON) {
    Direction = RelationDirectionDict.at(JSON["Direction"].get<std::string>());
    VariableName = new std::string(JSON["VariableName"].get<std::string>());
    std::string RelationNameFromJson = JSON["RelationName"].get<std::string>();
    if (RelationNameFromJson == "ANY") {
        RelationName = new std::string("");
    } else {
        RelationName = new std::string(RelationNameFromJson);
    }
}

MatchExpressionNode::MatchExpressionNode(json JSON) {
    LeftNode = getVariableMatchNode(JSON["LeftNode"]);
    json RelationJSON = JSON["Relation"];
    json RightNodeJSON = JSON["RightNode"];
    if (RelationJSON.is_null()) {
        Relation = nullptr;
    } else {
        Relation = new RelationMatchNode(RelationJSON);
    }
    if (RightNodeJSON.is_null()) {
        RightNode = nullptr;
    } else {
        RightNode = getVariableMatchNode(RightNodeJSON);
    }
}

ReturnExpressionNode::ReturnExpressionNode(json JSON) {
    auto ValuesJSONs = JSON["Values"].get<std::vector<json>>();
    for (auto j : ValuesJSONs) {
        Values.push_back(getValueNode(j));
    }
}

SetExpressionNode::SetExpressionNode(json JSON) {
    Dest = new VariableValueNode(JSON["Dest"]);
    Src = getValueNode(JSON["Src"]);
}

DeleteExpressionNode::DeleteExpressionNode(json JSON) {
    VariableName = new std::string(JSON["VariableName"].get<std::string>());
}

CreateExpressionNode::CreateExpressionNode(json JSON) {
    LeftNode = getVariableMatchNode(JSON["LeftNode"]);
    json RelationJSON = JSON["Relation"];
    json RightNodeJSON = JSON["RightNode"];
    if (RelationJSON.is_null()) {
        Relation = nullptr;
    } else {
        Relation = new RelationMatchNode(RelationJSON);
    }
    if (RightNodeJSON.is_null()) {
        RightNode = nullptr;
    } else {
        RightNode = getVariableMatchNode(RightNodeJSON);
    }
}

ExpressionNode* getExpressionNode(json JSON) {
    auto Kind = JSON["kind"].get<std::string>();
    if (Kind == "CreateExpressionNode") {
        return new CreateExpressionNode(JSON);
    }
    if (Kind == "DeleteExpressionNode") {
        return new DeleteExpressionNode(JSON);
    }
    if (Kind == "SetExpressionNode") {
        return new SetExpressionNode(JSON);
    }
    if (Kind == "ReturnExpressionNode") {
        return new ReturnExpressionNode(JSON);
    }
    if (Kind == "MatchExpressionNode") {
        return new MatchExpressionNode(JSON);
    }
    return nullptr;
}

RequestNode::RequestNode(json JSON) {
    auto ExpressionsJSONs = JSON["Expressions"].get<std::vector<json>>();
    for (auto j : ExpressionsJSONs) {
        Expressions.push_back(getExpressionNode(j));
    }
}
