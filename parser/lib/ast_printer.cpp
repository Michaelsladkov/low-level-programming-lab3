#include "ast.hpp"

#include <iostream>

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

void StringLiteralNode::print(int level, std::ostream& out) const {
    out << "String Literal node: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Value: " << *Value << std::endl;
    addSpaces(level, out);
    out << "}" << std::endl;
}

void BoolLiteralNode::print(int level, std::ostream& out) const {
    out << "Bool Literal node: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Value: " << Value << std::endl;
    addSpaces(level, out);
    out << "}" << std::endl;
}

void IntLiteralNode::print(int level, std::ostream& out) const {
    out << "Int Literal node: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Value: " << Value << std::endl;
    addSpaces(level, out);
    out << "}" << std::endl;
}

void FloatLiteralNode::print(int level, std::ostream& out) const {
    out << "Float Literal node: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Value: " << Value << std::endl;
    addSpaces(level, out);
    out << "}" << std::endl;
}

void FilterNode::print(int level, std::ostream& out) const {
    out << "Filter node: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Left part: ";
    LHS->print(level + 4, out);
    addSpaces(level + 4, out);
    out << "Operation: ";
    switch (Operation)
    {
    case GREATER:
        out << "GREATER";
        break;
    case GREATER_OR_EQUAL:
        out << "GREATER_OR_EQUAL";
        break;
    case LESS:
        out << "LESS";
        break;
    case LESS_OR_EQUAL:
        out << "LESS_OR_EQUAL";
        break;
    case EQUAL:
        out << "EQUAL";
        break;  
    case CONTAINS:
        out << "CONTAINS";
        break;  
    default:
        break;
    }
    out << std::endl;
    addSpaces(level + 4, out);
    out << "Right part: ";
    RHS->print(level + 4, out);
    addSpaces(level, out);
    out << "}" << std::endl;
}

void FilterByPassNode::print(int level, std::ostream& out) const {
    out << "Filter bypass: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Wrapped filter: ";
    Wrapped->print(level + 4, out);
    addSpaces(level, out);
    out << "}" << std::endl;
}

void NotOperationNode::print(int level, std::ostream& out) const {
    out << "NOT: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Operand: ";
    Operand->print(level+4, out);
    addSpaces(level, out);
    out << "}" << std::endl;
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

void PredicateNode::print(int level, std::ostream& out) const {
    out << "Predicate: {" << std::endl;
    addSpaces(level + 4, out);
    out << "expr: ";
    Body->print(level+4, out);
    addSpaces(level, out);
    out << "}" << std::endl;
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

void VariableMatchNode::print(int level, std::ostream& out) const {
    out << "Any variable match: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Variable name: " << *VariableName << std::endl;
    addSpaces(level + 4, out);
    out << "Scheme name: " << *SchemeName << std::endl;
    addSpaces(level, out);
    out << "}" << std::endl;
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

void RelationMatchNode::print(int level, std::ostream& out) const {
    out << "Relation match: {" << std::endl;
    addSpaces(level + 4, out);
    out << "Variable name: " << (VariableName->empty() ? "ANON" : *VariableName) << std::endl;
    addSpaces(level + 4, out);
    out << "Relation: " << (RelationName->empty() ? "ANY" : *RelationName) << std::endl;
    addSpaces(level + 4, out);
    out << "Direction: ";
    switch (Direction)
    {
    case FORWARD:
        out << "FORWARD";
        break;
    case REVERSE:
        out << "REVERSE";
        break;
    case ANY:
        out << "ANY";
        break;
    
    default:
        break;
    }
    out << std::endl;
    addSpaces(level, out);
    out << "}" << std::endl;
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

void ReturnExpressionNode::print(int level, std::ostream& out) const {
    out << "Return Expression: {" << std::endl;
    for (auto v : Values) {
        addSpaces(level + 4, out);
        v->print(level+4, out);
    }
    addSpaces(level, out);
    out << "}" << std::endl;
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

void DeleteExpressionNode::print(int level, std::ostream& out) const {
    out << "Delete Expresssion {" << std::endl;
    addSpaces(level + 4, out);
    out << "Variable: " << *VariableName << std::endl;
    addSpaces(level, out);
    out << "}" << std::endl;
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

void RequestNode::print(int level, std::ostream& out) const {
    out << "Request: {" << std::endl;
    for(auto e : Expressions) {
        addSpaces(level + 4, out);
        e->print(level + 4, out);
    }
    addSpaces(level, out);
    out << "}" << std::endl;
}
