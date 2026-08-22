#include <iostream>
#include <chava/tokenizer.hpp>
#include <chava/parser.hpp>

int main() {
    auto input = R"(
string y;
y = "balesh";
int x;
x = (5 + 2) + y * 7 - 6 /2;
while(x - 2 > 5 * 2 * 6 - (5+2) - 5 == x + 5 < 7) {
    x = x + 1;
    if(x == 7) {
    } else {
    }
}
)";
    std::cout << input << "\n";
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
}
