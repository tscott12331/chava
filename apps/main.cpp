#include <iostream>
#include <chava/tokenizer.hpp>

int main() {
    auto input = R"(
int x = 5;
while(x < 5) {
    println(x);
    x = x + 1;
    if(x == 7) {
        println(x);
    } else {
        println(x);
    }
}
)";
    std::cout << input << "\n";
    auto tokens = Tokenizer::Tokenize(input);
    
    for(const auto token : tokens) {
        std::cout << token_to_string(token) << "\n";
    }
}
