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
class balesh {
    String y;
    init() {}
    method a(int x, bool y) int {
        int z;
        z = x;
        return z;
    }
}

class baylesh extends balesh {
    init(balesh b, int x) { super(); }
}

int x;
x = (5 + 2) + 7 - 6 / 2;
balesh b;
b = new balesh();
baylesh g;
g = new baylesh(b, 5);

int xyz;
xyz = g.a(5, false);
while(x - 2 > 5 * 2 * 6 - (5+2) - 5 == x + 5 < 7) {
    x = x + 1;
    if(x == 7) {
    } else {
    }
}
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
