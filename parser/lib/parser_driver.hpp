#pragma once

#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <FlexLexer.h>

#include "ast.hpp"
#include "generated/parser.tab.hpp"
namespace yy{

using nlohmann::json;

class ParserDriver {
    FlexLexer *plex_;
    RequestNode *result;
    int DebugLevel;
    bool BadInput;

public:
    ParserDriver(FlexLexer *plex, int Debug) : plex_(plex), DebugLevel(Debug) {}
    yy::parser::token_type yylex(yy::parser::semantic_type *yylval);

    void markBadInput();

    bool parse();

    bool parse(std::string string);

    void insert(RequestNode *v);

    void printout() const;

    json getJson() const;

    ~ParserDriver();
};

} // namespace yy