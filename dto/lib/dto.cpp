#include "dto.hpp"

#include <string>
#include <unordered_map>

const std::unordered_map<MessageType, std::string> TypeToStringDict = {
    {RESPONSE, "RESPONSE"},
    {REQUEST, "REQUEST"}
};

const std::unordered_map<std::string, MessageType> StringToTypeDict = {
    {"RESPONSE", RESPONSE},
    {"REQUEST", REQUEST}
};

Message::Message(json JSON) {
    Type_ = StringToTypeDict.at(JSON["Type"].get<std::string>());
}

MessageType Message::getType() {
    return Type_;
}

Request::Request(RequestNode* Req) : Message(REQUEST) {
    RequestTree = new RequestNode(Req->toJson());
}

Request::Request(json JSON) : Message(JSON) {
    RequestTree = new RequestNode(JSON["RequestTree"]);
}

json Request::toJson() const {
    json ret;
    ret["Type"] = TypeToStringDict.at(REQUEST);
    ret["RequestTree"] = RequestTree->toJson();
    return ret;
}

Request::~Request() {
    delete RequestTree;
}

const std::string& ValueObj::getName() const {
    return Name_;
}

StringValueObj::StringValueObj(const char* Name, const char* Val) {
    Name_ = Name;
    Value = Val;
}

StringValueObj::StringValueObj(json JSON) {
    Name_ = JSON["Name"].get<std::string>();
    Value = JSON["Value"].get<std::string>();
}

json StringValueObj::toJson() const {
    json ret;
    ret["Name"] = Name_;
    ret["Value"] = Value;
    ret["ValueType"] = "String";
    return ret;
}

const std::string& StringValueObj::getValue() const {
    return Value;
}

IntValueObj::IntValueObj(const char* Name, int Val) {
    Name_ = Name;
    Value = Val;
}

IntValueObj::IntValueObj(json JSON) {
    Name_ = JSON["Name"].get<std::string>();
    Value = JSON["Value"].get<int>();
}

json IntValueObj::toJson() const {
    json ret;
    ret["Name"] = Name_;
    ret["Value"] = Value;
    ret["ValueType"] = "Int";
    return ret;
}

int IntValueObj::getValue() const {
    return Value;
}

FloatValueObj::FloatValueObj(const char* Name, float Val) {
    Name_ = Name;
    Value = Val;
}

FloatValueObj::FloatValueObj(json JSON) {
    Name_ = JSON["Name"].get<std::string>();
    Value = JSON["Value"].get<float>();
}

json FloatValueObj::toJson() const {
    json ret;
    ret["Name"] = Name_;
    ret["Value"] = Value;
    ret["ValueType"] = "Float";
    return ret;
}

float FloatValueObj::getValue() const {
    return Value;
}

BoolValueObj::BoolValueObj(const char* Name, bool Val) {
    Name_ = Name;
    Value = Val;
}

BoolValueObj::BoolValueObj(json JSON) {
    Name_ = JSON["Name"].get<std::string>();
    Value = JSON["Value"].get<bool>();
}

json BoolValueObj::toJson() const {
    json ret;
    ret["Name"] = Name_;
    ret["Value"] = Value;
    ret["ValueType"] = "Bool";
    return ret;
}

bool BoolValueObj::getValue() const {
    return Value;
}

NodeValueObj::NodeValueObj(size_t Id, const char* Name, const char* SchemeName) {
    Name_ = Name;
    Id_ = Id;
    SchemeName_ = SchemeName;
}

NodeValueObj::NodeValueObj(json JSON) {
    Name_ = JSON["Name"].get<std::string>();
    Id_ = JSON["Id"].get<size_t>();
    SchemeName_ = JSON["SchemeName"].get<std::string>();
    for (auto j : JSON["Attributes"].get<std::vector<json>>()) {
        auto TypeStr = j["ValueType"].get<std::string>();
        if (TypeStr == "String") {
            auto v = new StringValueObj(j);
            Attributes_.push_back(v);
            continue;
        }
        if (TypeStr == "Int") {
            auto v = new IntValueObj(j);
            Attributes_.push_back(v);
            continue;
        }
        if (TypeStr == "Float") {
            auto v = new FloatValueObj(j);
            Attributes_.push_back(v);
            continue;
        }
        if (TypeStr == "Bool") {
            auto v = new BoolValueObj(j);
            Attributes_.push_back(v);
        }
    }
}

void NodeValueObj::addAttribute(ValueObj* Attr) {
    Attributes_.push_back(Attr);
}

json NodeValueObj::toJson() const {
    json ret;
    ret["Type"] = "Node";
    ret["Id"] = Id_;
    ret["Name"] = Name_;
    ret["SchemeName"] = SchemeName_;
    ret["Attributes"] = json::array();
    // for (const auto& a : Attributes_) {
        // ret["Attributes"].emplace(a.toJson());
    // }
    return ret;
}

const std::vector<ValueObj*>& NodeValueObj::getAttributes() const {
    return Attributes_;
}

const std::string& NodeValueObj::getSchemeName() const {
    return SchemeName_;
}

size_t NodeValueObj::getId() const {
    return Id_;
}

NodeValueObj::~NodeValueObj() {
    for (size_t i = 0; i < Attributes_.size(); ++i) {
        auto p = Attributes_[Attributes_.size() - 1];
        Attributes_.pop_back();
        delete p;
    }
}

const std::unordered_map<ResponseStatus, std::string> StatusToStringDict = {
    {SUCCESS, "SUCCESS"},
    {FAIL, "FAIL"}
};

const std::unordered_map<std::string, ResponseStatus> StringToStatusDict = {
    {"SUCCESS", SUCCESS},
    {"FAIL", FAIL}
};

Response::Response(ResponseStatus Status) : Message(RESPONSE) {
    Status_ = Status;
}

Response::Response(json JSON) : Message(RESPONSE) {
    auto StatusStr = JSON["Status"].get<std::string>();
}

ResponseStatus Response::getStatus() const {
    return Status_;
}

ErrResponse::ErrResponse(const char* Message) : Response(FAIL) {
    ErrorMessage = Message;
}

ErrResponse::ErrResponse(json JSON) : Response(FAIL) {
    ErrorMessage = JSON["ErrorMessage"].get<std::string>();
}

const std::string& ErrResponse::getMessage() const {
    return ErrorMessage;
}

json ErrResponse::toJson() const {
    json ret;
    ret["Status"] = StatusToStringDict.at(Status_);
    ret["Type"] = TypeToStringDict.at(Type_);
    ret["ErrorMessage"] = ErrorMessage;
    return ret;
}

SuccessResponse::SuccessResponse() : Response(SUCCESS) {}

SuccessResponse::SuccessResponse(json JSON) : Response(JSON) {
    auto Vals = JSON["Values"].get<std::vector<json>>();
    for (auto j : Vals) {
        auto TypeStr = j["ValueType"].get<std::string>();
        if (TypeStr == "String") {
            auto v = new StringValueObj(j);
            Values.push_back(v);
            continue;
        }
        if (TypeStr == "Int") {
            auto v = new IntValueObj(j);
            Values.push_back(v);
            continue;
        }
        if (TypeStr == "Float") {
            auto v = new FloatValueObj(j);
            Values.push_back(v);
            continue;
        }
        if (TypeStr == "Bool") {
            auto v = new BoolValueObj(j);
            Values.push_back(v);
            continue;
        }
        if (TypeStr == "Node") {
            auto v = new NodeValueObj(j);
            Values.push_back(v);
        }
    }
}

void SuccessResponse::addValue(ValueObj* Val) {
    Values.push_back(Val);
}

const std::vector<ValueObj*>& SuccessResponse::getValues() const {
    return Values;
}

json SuccessResponse::toJson() const {
    json ret;
    ret["Status"] = StatusToStringDict.at(Status_);
    ret["Type"] = TypeToStringDict.at(Type_);
    ret["Values"] = json::array();
    // for (const auto& v : Values) {
        // ret["Values"].emplace(v.toJson());
    // }
    return ret;
}

SuccessResponse::~SuccessResponse() {
    for (size_t i = 0; i < Values.size(); ++i) {
        auto p = Values[Values.size() - 1];
        Values.pop_back();
        delete p;
    }
}

Message* getMessage(json JSON) {
    auto TypeStr = JSON["MessageType"].get<std::string>();
    if (StringToTypeDict.at(TypeStr) == RESPONSE) {
        auto StatusStr = JSON["Status"].get<std::string>();
        if (StringToStatusDict.at(StatusStr) == SUCCESS) {
            return new SuccessResponse(JSON);
        }
        if (StringToStatusDict.at(StatusStr) == FAIL) {
            return new ErrResponse(JSON);
        }
    }
    if (StringToTypeDict.at(TypeStr) == REQUEST) {
        return new Request(JSON);
    }
    return nullptr;
}
