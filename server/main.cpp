#include <iostream>
#include <nlohmann/json.hpp>
#include <ast.hpp>

#include "executor/executor.hpp"

using nlohmann::json;

int main() {
    std::cout << "Hello from server" << std::endl;
    std::string input = "";
    std::string line;
    while (std::getline(std::cin, line) && !line.empty()) {
        input = input + line;
    }
    json inputJson = json::parse(input);
    RequestDTO* Request = new RequestDTO(inputJson);
    Executor Ex;
    auto res = Ex.processRequest(Request);
    std::cout << res->toJson().dump(4) << std::endl;
    delete Request;
    return 0;
}