#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <expected>
#include <format>
#include <optional>
#include <string>
#include <vector>
#include "common.hpp"

#if defined(_WIN32) || defined(_WIN64)
    constexpr std::string newline = "\r\n";
#else
    constexpr std::string newline = "\n";
#endif

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
    StringToken,

    // class
    ClassToken,
    ExtendsToken,
    InitToken,
    MethodToken,
    NewToken,
    ThisToken,
    SuperToken,

    // symbol
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
    DotToken,
};

struct Token {
    TokenType type;
    std::string_view raw;
    Position pos;

    bool operator==(const Token &other) const {
        return type == other.type && raw == other.raw && pos == other.pos;
    }
};


class Tokenizer {
public:
    Tokenizer(std::string_view input);
    std::expected<std::vector<Token>, std::string> tokenize();
    static std::expected<std::vector<Token>, std::string> Tokenize(std::string_view input);
private:
    std::string_view input;
    int cursor;
    int line;
    int col;

    void skip_whitespace();
    std::expected<std::optional<Token>, std::string> get_token();
    std::expected<std::optional<Token>, std::string> get_num_keyword_or_identifier();
    std::expected<std::optional<Token>, std::string> get_num();
    std::expected<std::optional<Token>, std::string> get_string();
    Token create_token(TokenType token_type, std::string_view raw);
    std::string err_unexpected_token(std::string_view raw);
    std::string format_error(std::string_view message);
};

std::string_view token_to_string(const Token& token);

bool is_horizontal_space(std::string_view s);
bool is_vertical_space(std::string_view s);
bool is_newline(std::string_view s);
bool is_valid_keyword_or_ident_char(char c);
bool is_num(char c);



template <>
struct std::formatter<Token> : std::formatter<std::string> {
    template <typename FormatContext>
    auto format(const Token& token, FormatContext& ctx) const {
        std::string str = std::format("Token('{}')[{}:{}]", token_to_string(token), token.pos.line, token.pos.col);
        return std::formatter<std::string>::format(str, ctx);
    }
};

#endif
