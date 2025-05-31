#include "ast.hpp"
#include <broma.hpp>
#include <iostream>

void print_special_constant(std::ptrdiff_t num) {
    if (num == -1) {
        std::cout << "default";
        return;
    }

    if (num == -2) {
        std::cout << "inline";
        return;
    }

    std::cout << "0x" << num;
}

void print_func(broma::FunctionProto& func, broma::PlatformNumber& addrs, std::string inner = "") {

    std::cout << "\tmissing: " << (long)func.attributes.missing << "\n";
    std::cout << "\tsince: " << func.attributes.since << "\n";
    std::cout << "\t" << func.ret.name << " " << func.name << "(";
    for (auto arg : func.args) {
        std::cout << arg.first.name << " " << arg.second << ", ";
    }
    std::cout << ") = " << std::hex;

    std::cout << "win ";
    print_special_constant(addrs.win);
    std::cout << ", ";

    std::cout << "imac ";
    print_special_constant(addrs.imac);
    std::cout << ", ";

    std::cout << "m1 ";
    print_special_constant(addrs.m1);
    std::cout << ", ";

    std::cout << "ios ";
    print_special_constant(addrs.ios);

    if (!inner.empty()) {
        std::cout << " " << inner << "\n";
    } else {
        std::cout << ";\n";
    }

    std::cout << std::dec;
}

inline std::string nameForPlatform(broma::Platform platform) {
    using namespace broma;

    switch (platform) {
        case Platform::MacArm: return "m1";
        case Platform::MacIntel: return "imac";
        case Platform::Mac: return "mac";
        case Platform::Windows: return "win";
        case Platform::iOS: return "ios";
        case Platform::Android: return "android";
        case Platform::Android32: return "android32";
        case Platform::Android64: return "android64";
        default:
            return "win";
    }
}

void print_member(std::string name, broma::Platform handles, std::size_t count) {
    using namespace broma;

    std::cout << "\t" << name << "{";

    if (handles == Platform::None) {
        std::cout << "all";
    } else {
        for (auto platform : {Platform::MacArm, Platform::MacIntel, Platform::Windows, Platform::iOS, Platform::Android}) {
            if ((handles & platform) != Platform::None) {
                std::cout << nameForPlatform(platform) << ", ";
            }
        }
    }


    std::cout << "}\n";
}

void print_pad(broma::PlatformNumber& addrs) {
    std::cout << "\tPAD = " << std::hex;

    std::cout << "win ";
    print_special_constant(addrs.win);
    std::cout << ", ";

    std::cout << "imac ";
    print_special_constant(addrs.imac);
    std::cout << ", ";

    std::cout << "m1 ";
    print_special_constant(addrs.m1);
    std::cout << ", ";

    std::cout << "ios ";
    print_special_constant(addrs.ios);
    std::cout << ";\n";
}

void print_ast(broma::Root& ast) {
    std::cout << "Classes: " << ast.classes.size() << "\n";
    for (auto cls : ast.classes) {
        std::cout << "links: " << (long)cls.attributes.links << "\n";
        std::cout << "class " << cls.name << " {\n";
        for (auto field : cls.fields) {
            if (auto func = field.get_as<broma::FunctionBindField>()) {
                if (func->prototype.access == broma::AccessModifier::Protected) {
                    std::cout << "Protected!!!\n";
                } else {
                    std::cout << "Not protected.\n";
                }
                print_func(func->prototype, func->binds, func->inner);
            } else if (auto member = field.get_as<broma::MemberField>()) {
                print_member(member->name, member->platform, member->count);
            } else if (auto pad = field.get_as<broma::PadField>()) {
                print_pad(pad->amount);
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