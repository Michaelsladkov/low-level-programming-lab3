#include "ast.hpp"

#include <iostream>

#include <nlohmann/json.hpp>

using nlohmann::json;

static void addSpaces(int level, std::ostream& out) {
    for (int i = 0; i < level; ++i) {
        out << " ";
    }
}

void VariableValueNode::print(int level, std::ostream& out) const {
    out << "Variable value node: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Variable name: " << *VariableName << std::endl;
    addSpaces(level + 4, out);
    out << "Field name: " << *FieldName << std::endl;
    addSpaces(level, out);
    out << "}" << std::endl;
}
json VariableValueNode::toJson() const {
    json ret;
    ret["kind"] = "VariableValueNode";
    ret["VariableName"] = *VariableName;
    ret["FieldName"] = *FieldName;
    return ret;
}

void StringLiteralNode::print(int level, std::ostream& out) const {
    out << "String Literal node: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Value: " << *Value << std::endl;
    addSpaces(level, out);
    out << "}" << std::endl;
}
json StringLiteralNode::toJson() const {
    json ret;
    ret["kind"] = "StringLiteralNode";
    ret["Value"] = *Value;
    return ret;
}

void BoolLiteralNode::print(int level, std::ostream& out) const {
    out << "Bool Literal node: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Value: " << Value << std::endl;
    addSpaces(level, out);
    out << "}" << std::endl;
}
json BoolLiteralNode::toJson() const {
    json ret;
    ret["kind"] = "BoolLiteralNode";
    ret["Value"] = Value;
    return ret;
}

void IntLiteralNode::print(int level, std::ostream& out) const {
    out << "Int Literal node: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Value: " << Value << std::endl;
    addSpaces(level, out);
    out << "}" << std::endl;
}
json IntLiteralNode::toJson() const {
    json ret;
    ret["kind"] = "IntLiteralNode";
    ret["Value"] = Value;
    return ret;
}

void FloatLiteralNode::print(int level, std::ostream& out) const {
    out << "Float Literal node: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Value: " << Value << std::endl;
    addSpaces(level, out);
    out << "}" << std::endl;
}
json FloatLiteralNode::toJson() const {
    json ret;
    ret["kind"] = "FloatLiteralNode";
    ret["Value"] = Value;
    return ret;
}

const char* operationToStr(enum FilterCheckOperation op) {
    const char* ret;
    switch (op)
    {
    case GREATER:
        ret = "GREATER";
        break;
    case GREATER_OR_EQUAL:
        ret = "GREATER_OR_EQUAL";
        break;
    case LESS:
        ret = "LESS";
        break;
    case LESS_OR_EQUAL:
        ret = "LESS_OR_EQUAL";
        break;
    case EQUAL:
        ret = "EQUAL";
        break;  
    case CONTAINS:
        ret = "CONTAINS";
        break;  
    default:
        ret = "UNKNOWN";
        break;
    }
    return ret;
}

void FilterNode::print(int level, std::ostream& out) const {
    out << "Filter node: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Left part: ";
    LHS->print(level + 4, out);
    addSpaces(level + 4, out);
    out << "Operation: ";
    const char* OpStr = operationToStr(Operation);
    out << OpStr;
    out << std::endl;
    addSpaces(level + 4, out);
    out << "Right part: ";
    RHS->print(level + 4, out);
    addSpaces(level, out);
    out << "}" << std::endl;
}
json FilterNode::toJson() const {
    json ret;
    ret["kind"] = "FilterNode";
    ret["Operation"] = operationToStr(Operation);
    ret["LHS"] = LHS->toJson();
    ret["RHS"] = RHS->toJson();
    return ret;
}

void FilterByPassNode::print(int level, std::ostream& out) const {
    out << "Filter bypass: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Wrapped filter: ";
    Wrapped->print(level + 4, out);
    addSpaces(level, out);
    out << "}" << std::endl;
}
json FilterByPassNode::toJson() const {
    json ret;
    ret["kind"] = "FilterByPassNode";
    ret["Wrapped"] = Wrapped->toJson();
    return ret;
}

void NotOperationNode::print(int level, std::ostream& out) const {
    out << "NOT: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Operand: ";
    Operand->print(level+4, out);
    addSpaces(level, out);
    out << "}" << std::endl;
}
json NotOperationNode::toJson() const {
    json ret;
    ret["kind"] = "NotOperationNode";
    ret["Operand"] = Operand->toJson();
    return ret;
}

void AndOperationNode::print(int level, std::ostream& out) const {
    out << "AND: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Left operand: ";
    LHS->print(level + 4, out);
    addSpaces(level + 4, out);
    out << "Right operand: ";
    RHS->print(level + 4, out);
    addSpaces(level, out);
    out << "}" << std::endl;
}
json AndOperationNode::toJson() const {
    json ret;
    ret["kind"] = "AndOperationNode";
    ret["LHS"] = LHS->toJson();
    ret["RHS"] = RHS->toJson();
    return ret;
}

void OrOperationNode::print(int level, std::ostream& out) const {
    out << "OR: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Left operand: ";
    LHS->print(level + 4, out);
    addSpaces(level + 4, out);
    out << "Right operand: ";
    RHS->print(level + 4, out);
    addSpaces(level, out);
    out << "}" << std::endl;
}
json OrOperationNode::toJson() const {
    json ret;
    ret["kind"] = "OrOperationNode";
    ret["LHS"] = LHS->toJson();
    ret["RHS"] = RHS->toJson();
    return ret;
}

void PredicateNode::print(int level, std::ostream& out) const {
    out << "Predicate: {" << std::endl;
    addSpaces(level + 4, out);
    out << "expr: ";
    Body->print(level+4, out);
    addSpaces(level, out);
    out << "}" << std::endl;
}
json PredicateNode::toJson() const {
    json ret;
    ret["kind"] = "PredicateNode";
    ret["Body"] = Body->toJson();
    return ret;
}

void AttributeListNode::print(int level, std::ostream& out) const {
    out << "Attribute List: {" << std::endl;
    for (auto &[name, p] : AttrList) {
        addSpaces(level + 4, out);
        out << "{" << std::endl;
        addSpaces(level + 8, out);
        out << "Attribute name: " << name << std::endl;
        addSpaces(level + 8, out);
        out << "Value: ";
        p->print(level + 8, out);
        addSpaces(level + 4, out);
        out << "}," << std::endl;
    }
    addSpaces(level, out);
    out << "}" << std::endl;
}
json AttributeListNode::toJson() const {
    json ret;
    ret["kind"] = "AttributeListNode";
    ret["AttrList"] = json::array();
    for (auto &[name, p] : AttrList) {
        json attr;
        attr["name"] = name;
        attr["Value"] = p->toJson();
        ret["AttrList"].emplace_back(attr);
    } 
    return ret;
}

void VariableMatchNode::print(int level, std::ostream& out) const {
    out << "Any variable match: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Variable name: " << *VariableName << std::endl;
    addSpaces(level + 4, out);
    out << "Scheme name: " << *SchemeName << std::endl;
    addSpaces(level, out);
    out << "}" << std::endl;
}
json VariableMatchNode::toJson() const {
    json ret;
    ret["kind"] = "VariableMatchNode";
    ret["VariableName"] = *VariableName;
    ret["SchemeName"] = *SchemeName;
    return ret;
}

void VariablePatternMatchNode::print(int level, std::ostream& out) const {
    out << "Variable pattern match: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Variable name: " << *VariableName << std::endl;
    addSpaces(level + 4, out);
    out << "Scheme name: " << (SchemeName->empty() ? "ANY" : *SchemeName) << std::endl;
    addSpaces(level + 4, out);
    out << "Pattern: ";
    Pattern->print(level + 4, out);
    addSpaces(level, out);
    out << "}" << std::endl;
}
json VariablePatternMatchNode::toJson() const {
    json ret;
    ret["kind"] = "VariablePatternMatchNode";
    ret["VariableName"] = *VariableName;
    ret["SchemeName"] = *SchemeName;
    ret["Pattern"] = Pattern->toJson();
    return ret;
}

void VariableFilterMatchNode::print(int level, std::ostream& out) const {
    out << "Variable filter match: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Variable name: " << *VariableName << std::endl;
    addSpaces(level + 4, out);
    out << "Scheme name: " << *SchemeName << std::endl;
    addSpaces(level + 4, out);
    out << "Filter: ";
    Predicate->print(level + 4, out);
    addSpaces(level, out);
    out << "}" << std::endl;
}
json VariableFilterMatchNode::toJson() const {
    json ret;
    ret["kind"] = "VariableFilterMatchNode";
    ret["VariableName"] = *VariableName;
    ret["SchemeName"] = *SchemeName;
    ret["Predicate"] = Predicate->toJson();
    return ret;
}

const char* directionToStr(enum RelationDirection Direction) {
    const char* ret;
    switch (Direction)
    {
    case FORWARD:
        ret = "FORWARD";
        break;
    case REVERSE:
        ret = "REVERSE";
        break;
    case ANY:
        ret = "ANY";
        break;
    
    default:
        ret = "UNKNOWN";
        break;
    }
    return ret;
}

void RelationMatchNode::print(int level, std::ostream& out) const {
    out << "Relation match: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Variable name: " << (VariableName->empty() ? "ANON" : *VariableName) << std::endl;
    addSpaces(level + 4, out);
    out << "Relation: " << (RelationName->empty() ? "ANY" : *RelationName) << std::endl;
    addSpaces(level + 4, out);
    out << "Direction: ";
    out << directionToStr(Direction);
    out << std::endl;
    addSpaces(level, out);
    out << "}" << std::endl;
}
json RelationMatchNode::toJson() const {
    json ret;
    ret["kind"] = "RealationMatchNode";
    ret["VariableName"] = *VariableName;
    ret["Relation"] = (RelationName->empty() ? "ANY" : *RelationName);
    ret["Direction"] = directionToStr(Direction);
    return ret;
}

void MatchExpressionNode::print(int level, std::ostream& out) const {
    out << "Match expression: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Left node: ";
    LeftNode->print(level + 4, out);
    if (Relation != nullptr) {
        addSpaces(level + 4, out);
        out << "Relation: ";
        Relation->print(level + 4, out);
    }
    if (RightNode != nullptr) {
        addSpaces(level + 4, out);
        out << "Right node: ";
        RightNode->print(level + 4, out);
    }
    addSpaces(level, out);
    out << "}" << std::endl;
}
json MatchExpressionNode::toJson() const {
    json ret;
    ret["kind"] = "MatchExpressionNode";
    ret["LeftNode"] = LeftNode->toJson();
    ret["Relation"] = (Relation != nullptr) ? Relation->toJson() : nullptr;
    ret["RightNode"] = (RightNode != nullptr) ? RightNode->toJson() : nullptr;
    return ret;
}

void ReturnExpressionNode::print(int level, std::ostream& out) const {
    out << "Return Expression: {" << std::endl;
    for (auto v : Values) {
        addSpaces(level + 4, out);
        v->print(level+4, out);
    }
    addSpaces(level, out);
    out << "}" << std::endl;
}
json ReturnExpressionNode::toJson() const {
    json ret;
    ret["kind"] = "ReturnExpressionNode";
    ret["Values"] = json::array();
    for (auto v : Values) {
        ret["Values"].emplace_back(v->toJson());
    }
    return ret;
}

void SetExpressionNode::print(int level, std::ostream& out) const {
    out << "Set Expression: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Destination: ";
    Dest->print(level + 4, out);
    addSpaces(level + 4, out);
    out << "Source: ";
    Src->print(level + 4, out);
    addSpaces(level, out);
    out << "}" << std::endl;
}
json SetExpressionNode::toJson() const {
    json ret;
    ret["kind"] = "SetExpressionNode";
    ret["Dest"] = Dest->toJson();
    ret["Src"] = Src->toJson();
    return ret;
}

void DeleteExpressionNode::print(int level, std::ostream& out) const {
    out << "Delete Expresssion {" << std::endl;
    addSpaces(level + 4, out);
    out << "Variable: " << *VariableName << std::endl;
    addSpaces(level, out);
    out << "}" << std::endl;
}
json DeleteExpressionNode::toJson() const {
    json ret;
    ret["kind"] = "DeleteExpressionNode";
    ret["VariableName"] = *VariableName;
    return ret;
}

void CreateExpressionNode::print(int level, std::ostream& out) const {
    out << "Create expression: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Left node: ";
    LeftNode->print(level + 4, out);
    if (Relation != nullptr) {
        addSpaces(level + 4, out);
        out << "Relation: ";
        Relation->print(level + 4, out);
    }
    if (RightNode != nullptr) {
        addSpaces(level + 4, out);
        out << "Right node: ";
        RightNode->print(level + 4, out);
    }
    addSpaces(level, out);
    out << "}" << std::endl;
}
json CreateExpressionNode::toJson() const {
    json ret;
    ret["kind"] = "CreateExpressionNode";
    ret["LeftNode"] = LeftNode->toJson();
    ret["Relation"] = (Relation != nullptr) ? Relation->toJson() : nullptr;
    ret["RightNode"] = (RightNode != nullptr) ? RightNode->toJson() : nullptr;
    return ret;
}

void RequestNode::print(int level, std::ostream& out) const {
    out << "Request: {" << std::endl;
    for(auto e : Expressions) {
        addSpaces(level + 4, out);
        e->print(level + 4, out);
    }
    addSpaces(level, out);
    out << "}" << std::endl;
}
json RequestNode::toJson() const {
    json ret;
    ret["kind"] = "RequestNode";
    ret["Expressions"] = json::array();
    for(auto e : Expressions) {
        ret["Expressions"].emplace_back(e->toJson());
    }
    return ret;
}
