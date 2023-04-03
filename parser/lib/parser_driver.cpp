#include "parser_driver.hpp"

#include <nlohmann/json.hpp>

int yyFlexLexer::yywrap() {
    return 1;
}

namespace yy
{

yy::parser::token_type ParserDriver::yylex(yy::parser::semantic_type *yylval) {
    yy::parser::token_type tt = static_cast<yy::parser::token_type>(this->plex_->yylex());
    if (this->DebugLevel) std::cout << "lexer brought this: " << this->plex_->YYText() << std::endl;
    const char *TokenStr = this->plex_->YYText();
    if (tt == yy::parser::token_type::INT_LITERAL) {
        yylval->integer = std::stoi(TokenStr);
    }
    if (tt == yy::parser::token_type::FLOAT_LITERAL) {
        yylval->real = std::atof(TokenStr);
    }
    if (tt == yy::parser::token_type::BOOL_LITERAL) {
        bool value = false;
        if (std::strcmp(TokenStr, "true") == 0) {
            value = true;
        }
        yylval->boolean = value;
    }
    if (tt == yy::parser::token_type::STRING_LITERAL) {
        yylval->string = new std::string(TokenStr);
    }
    if (tt == yy::parser::token_type::NAME) {
        yylval->name = new std::string(TokenStr);
    }
    return tt;
}

void ParserDriver::markBadInput() {
    BadInput = true;
}

bool ParserDriver::parse() {
    this->BadInput = false;
    parser parser(this);
    parser.set_debug_level(DebugLevel);
    if (DebugLevel) std::cout << "begin parsing via parser" << std::endl;
    bool res = parser.parse();
    return !res;
}

bool ParserDriver::parse(std::string string) {
    this->BadInput = false;
    auto StringStream = std::istringstream(string);
    auto Buff = plex_->yy_create_buffer(StringStream, string.length());
    this->plex_->yy_switch_to_buffer(Buff);
    parser parser(this);
    parser.set_debug_level(DebugLevel);
    bool res = parser.parse();
    plex_->yy_delete_buffer(Buff);
    return !res;
}

void ParserDriver::insert(RequestNode *v) {
    this->result = v;
    if (this->DebugLevel) std::cout << "something inserted into controller" << std::endl;
}

void ParserDriver::printout() const {
    if (this->BadInput) {
        std::cout << "Input request incorrect, AST can't be built" << std::endl;
    } else {
        this->result->print(0, std::cout);
    }
}

json ParserDriver::getJson() const {
    return result->toJson(); 
}

ParserDriver::~ParserDriver() {
    if (!this->BadInput)
        delete this->result;
}

} // namespace