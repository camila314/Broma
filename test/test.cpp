#include "ast.hpp"
#include <broma.hpp>
#include <iostream>

void print_func(broma::FunctionProto& func, broma::PlatformNumber& addrs) {

    std::cout << "\tlinks: " << (long)func.attributes.links << "\n";
    std::cout << "\t" << func.ret.name << " " << func.name << "(";
    for (auto arg : func.args) {
        std::cout << arg.first.name << " " << arg.second << ", ";
    }
    std::cout << ") = " << std::hex;
    std::cout << "win 0x" << addrs.win << ", ";
    std::cout << "mac 0x" << addrs.mac << ", ";
    std::cout << "ios 0x" << addrs.ios << ";\n";
    std::cout << std::dec;
}

void print_ast(broma::Root& ast) {
    std::cout << "Classes: " << ast.classes.size() << "\n";
    for (auto cls : ast.classes) {
        std::cout << "links: " << (long)cls.attributes.links << "\n";
        std::cout << "class " << cls.name << " {\n";
        for (auto field : cls.fields) {
            if (auto func = field.get_as<broma::FunctionBindField>()) {
                print_func(func->prototype, func->binds);
            }
        }
        std::cout << "};\n";
    }
    std::cout << "Functions: " << ast.functions.size() << "\n";
    for (auto func : ast.functions) {
        print_func(func.prototype, func.binds);
    }
}

int main() {
    try {
        auto parsed_class = broma::parse_file("class.bro");
        print_ast(parsed_class);

        auto parsed_free = broma::parse_file("free.bro");
        print_ast(parsed_free);
    }
    catch (std::exception const& err) {
        std::cout << "Test failed with error: " << err.what() << '\n';
    }
}