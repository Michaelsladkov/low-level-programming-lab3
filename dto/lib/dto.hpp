#pragma once

#include <nlohmann/json.hpp>

#include <ast.hpp>

#include <string>
#include <unordered_map>

using nlohmann::json;

enum MessageType {
    REQUEST,
    RESPONSE
};

enum ResponseStatus {
    SUCCESS,
    FAIL
};

class Message {
protected:
    MessageType Type_;
public:
    Message(MessageType Type) : Type_(Type) {};
    Message(json JSON);
    MessageType getType();
    virtual json toJson() const = 0;
    virtual ~Message() {}
};

class RequestDTO : public Message {
    RequestNode* RequestTree;
public:
    RequestDTO(RequestNode* Req);
    RequestDTO(json JSON);
    RequestNode* getRequestTree();
    virtual json toJson() const override;
    virtual ~RequestDTO() override;
};

class ValueObj {
protected:
    std::string Name_;
public:
    virtual const std::string& getName() const;
    virtual json toJson() const = 0;
    virtual ~ValueObj() {}
};

class StringValueObj : public ValueObj {
    std::string Value;
public:
    StringValueObj(const char* Name, const char* Val);
    StringValueObj(json JSON);
    virtual json toJson() const override;
    const std::string& getValue() const;
};

class IntValueObj : public ValueObj {
    int Value;
public:
    IntValueObj(const char* Name, int Val);
    IntValueObj(json JSON);
    virtual json toJson() const override;
    int getValue() const;
};

class FloatValueObj : public ValueObj {
    float Value;
public:
    FloatValueObj(const char* Name, float Val);
    FloatValueObj(json JSON);
    virtual json toJson() const override;
    float getValue() const;
};

class BoolValueObj : public ValueObj {
    bool Value;
public:
    BoolValueObj(const char* Name, bool Val);
    BoolValueObj(json JSON);
    virtual json toJson() const override;
    bool getValue() const;
};

class NodeValueObj : public ValueObj {
    size_t Id_;
    std::string SchemeName_;
    std::vector<ValueObj*> Attributes_; 
public:
    NodeValueObj(size_t Id, const char* Name, const char* SchemeName);
    NodeValueObj(json JSON);
    void addAttribute(ValueObj* Attr);
    virtual json toJson() const override;
    const std::vector<ValueObj*>& getAttributes() const;
    const std::string& getSchemeName() const;
    size_t getId() const;
    virtual ~NodeValueObj() override;
};

class Response : public Message {
protected:
    ResponseStatus Status_;
public:
    Response(ResponseStatus Stat);
    Response(json JSON);
    ResponseStatus getStatus() const;
};

class ErrResponse : public Response {
    std::string ErrorMessage;
public:
    ErrResponse(const char* Message);
    ErrResponse(json JSON);
    const std::string& getMessage() const;
    virtual json toJson() const override;
};

class SuccessResponse : public Response {
    std::vector<ValueObj*> Values;
public:
    SuccessResponse(json JSON);
    SuccessResponse();
    void addValue(ValueObj* Val);
    const std::vector<ValueObj*>& getValues() const;
    virtual json toJson() const override;
    virtual ~SuccessResponse() override;
};

Message* getMessage(json JSON);
