#ifndef PARSER_HPP
#define PARSER_HPP

#include "chava/tokenizer.hpp"
#include <vector>
class Parser {
public:
    Parser(std::vector<Token> tokens);

    void parse();
};

#endif
