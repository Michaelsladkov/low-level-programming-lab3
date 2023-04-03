#include <iostream>
#include <parser_driver.hpp>

int main() {
    FlexLexer* lexer = new yyFlexLexer;
    yy::ParserDriver driver(lexer, 0);
    #if PARSE_STRING
        std::string Inp = "MATCH (E)";
        driver.parse(Inp);
    #else
        driver.parse();
    #endif
    driver.printout();
    delete lexer;
}