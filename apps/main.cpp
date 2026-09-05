#include <format>
#include <iostream>
#include <chava/tokenizer.hpp>
#include <chava/parser.hpp>
#include <chava/typechecker.hpp>
#include <ranges>
#include <string_view>

int main() {
    std::string input = 
R"(
class A { init() {} }
class B extends A { init() { super(); } }
class C extends B { init () { super(); } }
class Test {
    init() {}

    method a(B p1, A p2, B p3) void {}
    method a(A p1, C p2, B p3) void {}
}

A a;
a = new A();
B b;
b = new B();
C c;
c = new C();
Test t;
t = new Test();

t.a(b, c, b);
)";

    int line_num = 1;
    for(const auto line : std::views::split(input, '\n')) {
        std::cout << std::format("{:>3}| {}\n", line_num, std::string_view(line.data(), line.size()));
        line_num += 1;
    }

    auto tokens = Tokenizer::Tokenize(input);
    if(!tokens) {
        std::cout << tokens.error() << "\n";
        exit(1);
    }

    auto program = Parser::Parse(tokens.value());
    if(!program) {
        std::cout << program.error() << "\n";
        exit(1);
    }
    std::cout << "parsed successfully\n";
    std::cout << program->classdefs.size() << " classes\n";
    std::cout << program->stmts.size() << " stmts\n";

    if(auto typecheck_res = TypeChecker::Typecheck(program.value()); !typecheck_res) {
        std::cout << "TYPE ERROR: ";
        std::cout << typecheck_res.error() << "\n";
    }
}
