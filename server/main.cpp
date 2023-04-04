#include <iostream>
#include <nlohmann/json.hpp>
#include <ast.hpp>
using nlohmann::json;

int main() {
    std::cout << "Hello from server" << std::endl;
    std::string input = "";
    std::string line;
    while (std::getline(std::cin, line) && !line.empty()) {
        input = input + line;
    }
    json inputJson = json::parse(input);
    RequestNode* Request = new RequestNode(inputJson);
    json outputJson = Request->toJson();
    std::cout << outputJson.dump(4) << std::endl;
    delete Request;
    return 0;
}