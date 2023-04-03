#pragma once

#include <string>
#include <unordered_map>
#include <vector>

class INode {
public:
    virtual ~INode() {}
    virtual void print(int level, std::ostream& out) const = 0;
};

class ExpressionNode : public INode {

};

class ValueNode : public INode {
    
};

class VariableValueNode : public ValueNode {
    std::string* VariableName;
    std::string* FieldName;
public:
    VariableValueNode(std::string *VName, std::string *FName) {
        VariableName = VName;
        FieldName = FName;
    }
    const std::string* getVariableName() {
        return VariableName;
    }
    const std::string* getFieldName() {
        return FieldName;
    }
    virtual void print(int level, std::ostream& out) const override;
    virtual ~VariableValueNode() override {
        delete VariableName;
        delete FieldName;
    }
};

class StringLiteralNode : public ValueNode {
    std::string *Value;
public:
    StringLiteralNode(std::string *Val) {
        Value = Val;
    }
    const std::string* getValue() const {
        return Value;
    }
    virtual void print(int level, std::ostream& out) const override;
    virtual ~StringLiteralNode() override {
        delete Value;
    }
};

class BoolLiteralNode : public ValueNode {
    bool Value;
public:
    BoolLiteralNode(bool Val) {
        Value = Val;
    }
    bool getValue() const {
        return Value;
    }
    virtual void print(int level, std::ostream& out) const override;
};

class IntLiteralNode : public ValueNode {
    int Value;
public:
    IntLiteralNode(int Val) {
        Value = Val;
    }
    int getValue() const {
        return Value;
    }
    virtual void print(int level, std::ostream& out) const override;
};

class FloatLiteralNode : public ValueNode {
    float Value;
public:
    FloatLiteralNode(float Val) {
        Value = Val;
    }
    float getValue() const {
        return Value;
    }
    virtual void print(int level, std::ostream& out) const override;
};

enum FilterCheckOperation {
    GREATER,
    GREATER_OR_EQUAL,
    LESS,
    LESS_OR_EQUAL,
    EQUAL,
    CONTAINS
};

class FilterNode : public INode {
    ValueNode *RHS;
    ValueNode *LHS;
    FilterCheckOperation Operation;
public:
    FilterNode(ValueNode *Left, ValueNode *Right, FilterCheckOperation Op) {
        RHS = Right;
        LHS = Left;
        Operation = Op;
    }
    const virtual ValueNode* getRHS() const {
        return RHS;
    }
    const virtual ValueNode* getLHS() const {
        return LHS;
    }
    const virtual FilterCheckOperation getOperation() const {
        return Operation;
    }
    virtual ~FilterNode() override {
        delete RHS;
        delete LHS;
    }
    virtual void print(int level, std::ostream& out) const override;
};

class LogicalExpressionNode : public INode {

};

class FilterByPassNode : public LogicalExpressionNode {
    FilterNode *Wrapped;
public:
    FilterByPassNode(class FilterNode *F){
        Wrapped = F;
    }
    const virtual FilterNode* getFilter() const {
        return Wrapped;
    }
    virtual void print(int level, std::ostream& out) const override;
};

class NotOperationNode : public LogicalExpressionNode {
    LogicalExpressionNode *Operand;
public:
    NotOperationNode(LogicalExpressionNode *Op) {
        Operand = Op;
    }
    const virtual LogicalExpressionNode* getOperand() const {
        return Operand;
    }
    virtual ~NotOperationNode() override {
        delete Operand;
    }
    virtual void print(int level, std::ostream& out) const override;
};

class BinaryLogicalOperationNode : public LogicalExpressionNode {
protected:
    LogicalExpressionNode *RHS;
    LogicalExpressionNode *LHS;
    BinaryLogicalOperationNode(LogicalExpressionNode *Left, LogicalExpressionNode *Right) {
        RHS = Right;
        LHS = Left;
    }
public:
    const virtual LogicalExpressionNode* getLeft() const {
        return LHS;
    }
    const virtual LogicalExpressionNode* getRight() const {
        return RHS;
    }
    virtual ~BinaryLogicalOperationNode() override {
        delete RHS;
        delete LHS;
    }
};

class AndOperationNode : public BinaryLogicalOperationNode {
public:
    AndOperationNode(LogicalExpressionNode *Left, LogicalExpressionNode *Right) : BinaryLogicalOperationNode(Left, Right) {}
    virtual void print(int level, std::ostream& out) const override;
};

class OrOperationNode : public BinaryLogicalOperationNode {
public:
    OrOperationNode(LogicalExpressionNode *Left, LogicalExpressionNode *Right) : BinaryLogicalOperationNode(Left, Right) {}
    virtual void print(int level, std::ostream& out) const override;
};

class PredicateNode : public INode {
    LogicalExpressionNode *Body;
public:
    PredicateNode(LogicalExpressionNode *Expr) {
        Body = Expr;
    }
    const LogicalExpressionNode* getExpr() const {
        return Body;
    }
    virtual ~PredicateNode() {
        delete Body;
    }
    virtual void print(int level, std::ostream& out) const override;
};

class AttributeListNode : public INode {
    std::unordered_map<std::string, ValueNode*> AttrList;
public:
    bool addAttribute(std::string* name, ValueNode* Value) {
        if (AttrList.count(*name)) return false;
        AttrList[*name] = Value;
        delete name;
        return true;
    }
    const std::unordered_map<std::string, ValueNode*>& getAttributeList() const {
        return AttrList;
    }
    virtual ~AttributeListNode() override {
        for (auto &[name, p] : AttrList) {
            delete p;
        }
    }
    virtual void print(int level, std::ostream& out) const override;
};

class VariableMatchNode : public INode {
protected:
    std::string *VariableName;
    std::string *SchemeName;
public:
    VariableMatchNode(std::string *Var, std::string *Scheme) {
        VariableName = Var;
        SchemeName = Scheme;
    }
    const std::string* getVariableName() const {
        return VariableName;
    }
    const std::string* getSchemeName() const {
        return SchemeName;
    }
    virtual void print(int level, std::ostream& out) const override;
    virtual ~VariableMatchNode() override {
        delete VariableName;
        delete SchemeName;
    }
};

class VariablePatternMatchNode : public VariableMatchNode {
    AttributeListNode *Pattern;
public:
    VariablePatternMatchNode(std::string *Var, std::string *Scheme, AttributeListNode* AttrList) :
        VariableMatchNode(Var, Scheme) {
        Pattern = AttrList;
    }
    virtual ~VariablePatternMatchNode() override {
        delete Pattern;
    }
    const AttributeListNode* getPattern() const {
        return Pattern;
    }
    virtual void print(int level, std::ostream& out) const override;
};

class VariableFilterMatchNode : public VariableMatchNode {
    PredicateNode* Predicate;
public:
    VariableFilterMatchNode(std::string *Var, std::string *Scheme, PredicateNode* Filters) :
        VariableMatchNode(Var, Scheme) {
        Predicate = Filters;
    }
    virtual ~VariableFilterMatchNode() {
        delete Predicate;
    }
    const PredicateNode* getFilter() const {
        return Predicate;
    }
    virtual void print(int level, std::ostream& out) const override;
};

enum RelationDirection {
    FORWARD,
    REVERSE,
    ANY
};

class RelationMatchNode : public INode {
    std::string *VariableName;
    std::string *RelationName;
    RelationDirection Direction;
public:
    RelationMatchNode(std::string *Var, std::string *Rel, RelationDirection Dir) {
        VariableName = Var;
        RelationName = Rel;
        Direction = Dir;
    }
    virtual void print(int level, std::ostream& out) const override;
    const std::string* getVariableName() const {
        return VariableName;
    }
    const std::string* getRelationName() const {
        return RelationName;
    }
    RelationDirection getDirection() const {
        return Direction;
    }
    virtual ~RelationMatchNode() override {
        delete VariableName;
        delete RelationName;
    }
};

class MatchExpressionNode : public ExpressionNode {
    VariableMatchNode *LeftNode;
    VariableMatchNode *RightNode;
    RelationMatchNode *Relation;
public:
    MatchExpressionNode(VariableMatchNode *Node) {
        LeftNode = Node;
        RightNode = nullptr;
        Relation = nullptr;
    }
    MatchExpressionNode(VariableMatchNode *Left, VariableMatchNode *Right) {
        LeftNode = Left;
        RightNode = Right;
        Relation = new RelationMatchNode(new std::string(""), new std::string(""), ANY);
    }
    MatchExpressionNode(VariableMatchNode *Left, VariableMatchNode *Right, RelationMatchNode *Rel) {
        LeftNode = Left;
        RightNode = Right;
        Relation = Rel;
    }
    virtual void print(int level, std::ostream& out) const override;
    const VariableMatchNode* getRightMatchNode() const {
        return RightNode;
    }
    const VariableMatchNode* getLeftMatchNode() const {
        return LeftNode;
    }
    const RelationMatchNode* getRelationMatchNode() const {
        return Relation;
    }
    virtual ~MatchExpressionNode() {
        delete LeftNode;
        delete RightNode;
        delete Relation;
    }
};

class ReturnExpressionNode : public ExpressionNode {
    std::vector<ValueNode*> Values;
public:
    virtual void print(int level, std::ostream& out) const override;
    void addElement(ValueNode* Val) {
        Values.push_back(Val);
    }
    const std::vector<ValueNode*>& getValues() const {
        return Values;
    }
    virtual ~ReturnExpressionNode() override {
        for (size_t i = 0; i < Values.size(); ++i) {
            ValueNode* v = *(Values.end());
            Values.pop_back();
            delete v;
        }
    }
};

class SetExpressionNode : public ExpressionNode {
    VariableValueNode *Dest;
    ValueNode *Src;
public:
    SetExpressionNode(VariableValueNode *Destination, ValueNode *Source) {
        Dest = Destination;
        Src = Source;
    }
    const VariableValueNode* getDestination() const {
        return Dest;
    }
    const ValueNode* getSource() const {
        return Src;
    }
    virtual void print(int level, std::ostream& out) const override;
};

class DeleteExpressionNode : public ExpressionNode {
    std::string* VariableName;
public:
    DeleteExpressionNode(std::string* Name) {
        VariableName = Name;
    }
    virtual void print(int level, std::ostream& out) const override;
    const std::string* getVariableName() const {
        return VariableName;
    }
    virtual ~DeleteExpressionNode() override {
        delete VariableName;
    }
};

class CreateExpressionNode : public ExpressionNode {
    VariableMatchNode *LeftNode;
    VariableMatchNode *RightNode;
    RelationMatchNode *Relation;
public:
    CreateExpressionNode(VariableMatchNode *Node) {
        LeftNode = Node;
        RightNode = nullptr;
        Relation = nullptr;
    }
    CreateExpressionNode(VariableMatchNode *Left, VariableMatchNode *Right, RelationMatchNode *Rel) {
        LeftNode = Left;
        RightNode = Right;
        Relation = Rel;
    }
    virtual void print(int level, std::ostream& out) const override;
    const VariableMatchNode* getRightMatchNode() const {
        return RightNode;
    }
    const VariableMatchNode* getLeftMatchNode() const {
        return LeftNode;
    }
    const RelationMatchNode* getRelationMatchNode() const {
        return Relation;
    }
    virtual ~CreateExpressionNode() override {
        delete LeftNode;
        delete RightNode;
        delete Relation;
    }
};

class RequestNode : public INode {
    std::vector<ExpressionNode*> Expressions;
public:
    virtual void print(int level, std::ostream& out) const override;
    RequestNode(ExpressionNode *Expr) {
        Expressions.push_back(Expr);
    }
    void addExpr(ExpressionNode* Expr) {
        Expressions.push_back(Expr);
    }
    const std::vector<ExpressionNode*>& getExpressions() const {
        return Expressions;
    }
    virtual ~RequestNode() override {
        for (size_t i = 0; i < Expressions.size(); ++i) {
            ExpressionNode* e = *(Expressions.end());
            Expressions.pop_back();
            delete e;
        }
    }
};
