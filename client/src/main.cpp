#include <iostream>
#include <parser_driver.hpp>
#include <dto.hpp>
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
    Message* msg = new RequestDTO(driver.getResult());
    auto j = msg->toJson();
    std::cout << j.dump(4) << std::endl;
    delete msg;
    delete lexer;
}