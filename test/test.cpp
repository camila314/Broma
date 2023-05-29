#include <broma.hpp>
#include <iostream>

void print_func(broma::FunctionProto& func) {
    std::cout << "\t" << func.ret.name << " " << func.name << "(";
    for (auto arg : func.args) {
        std::cout << arg.first.name << " " << arg.second << ", ";
    }
    std::cout << ")\n";
}

void print_ast(broma::Root& ast) {
    std::cout << "Classes: " << ast.classes.size() << "\n";
    for (auto cls : ast.classes) {
        std::cout << "class " << cls.name << " {\n";
        for (auto field : cls.fields) {
            if (auto func = field.get_fn()) {
                print_func(*func);
            }
        }
        std::cout << "};\n";
    }
    std::cout << "Functions: " << ast.functions.size() << "\n";
    for (auto func : ast.functions) {
        print_func(func.prototype);
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