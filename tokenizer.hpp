#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <expected>
#include <optional>
#include <string>
#include <vector>
#include "common.hpp"

enum class TokenType {
    // types
    IntToken,
    BoolToken,
    VoidToken,

    // control flow
    IfToken,
    ElseToken,
    WhileToken,
    BreakToken,
    ReturnToken,

    // ops
    PlusToken,
    DashToken,
    FSlashToken,
    StarToken,

    // literals
    TrueToken,
    FalseToken,
    IdentToken,
    NumberToken,

    // class
    ClassToken,
    ExtendsToken,
    InitToken,
    MethodToken,
    NewToken,

    // syntax
    CommaToken,
    LParenToken,
    RParenToken,
    SemiColonToken,
    AssignToken,
    EqualToken,
    NotEqualToken,
    LAngleToken,
    RAngleToken,
    LBracketToken,
    RBracketToken,
};

struct Token {
    TokenType type;
    std::string_view raw;
    Position pos;
};


class Tokenizer {
public:
    Tokenizer(std::string_view input) : input(input) {};
    std::vector<Token> tokenize();
    static std::vector<Token> Tokenize(std::string_view input);
private:
    std::string_view input;
    int offset;
    int line;
    int col;

    void skip_whitespace();
    std::expected<std::optional<Token>, std::string> get_token();
    Token create_token(TokenType token_type, std::string_view raw);
    std::string err_unexpected_token(std::string_view raw);
    std::string format_error(std::string_view message);
};


#endif
