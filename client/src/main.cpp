#include <iostream>
#include <parser_driver.hpp>
#include <nlohmann/json.hpp>

int main() {
    FlexLexer* lexer = new yyFlexLexer;
    yy::ParserDriver driver(lexer, 0);
    #if PARSE_STRING
        std::string Inp = "MATCH (E)";
        driver.parse(Inp);
    #else
        driver.parse();
    #endif
    auto j = driver.getJson();
    std::cout << j.dump() << std::endl;
    delete lexer;
}