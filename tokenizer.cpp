#include "tokenizer.hpp"
#include <expected>
#include <format>
#include <optional>
#include <string_view>
#include <vector>

#if defined(_WIN32) || defined(_WIN64)
    std::string newline = "\r\n";
#else
    std::string newline = "\n";
#endif

bool is_horizontal_space(std::string_view s);
bool is_vertical_space(std::string_view s);
bool is_newline(std::string_view s);

std::vector<Token> Tokenizer::Tokenize(std::string_view input) {
    Tokenizer t(input);
    return t.tokenize();
}

std::vector<Token> Tokenizer::tokenize() {
    std::vector<Token> tokens;
    while(offset < input.length()) {
        skip_whitespace();
        auto token = get_token();
        if(token.has_value()) {
            tokens.push_back(token->value());
        }
    }

    return tokens;
}

// returns (new_pos, newlines)
void Tokenizer::skip_whitespace() {
    while(offset < input.length()) {
        auto sub = input.substr(offset);
        if(is_newline(sub)) {
            line += 1;
            col = 0;
        } else if(!is_horizontal_space(sub) || !is_vertical_space(sub)) {
            break;
        }

        offset += 1;
        col += 1;
    }
}

std::expected<std::optional<Token>, std::string>  Tokenizer::get_token() {
    if(offset >= input.length()) {
        return std::nullopt;
    }

    auto sub = input.substr(offset);
    switch(sub.at(0)) {
        case ',':
            return create_token(TokenType::CommaToken, ",");
        case '(':
            return create_token(TokenType::LParenToken, "(");
        case ')':
            return create_token(TokenType::RParenToken, ")");
        case ';':
            return create_token(TokenType::SemiColonToken, ";");
        case '=':
            if(sub.starts_with("==")) {
                return create_token(TokenType::EqualToken, "==");
            }
            return create_token(TokenType::AssignToken, "=");
        case '!':
            if(sub.starts_with("!=")) {
                return create_token(TokenType::NotEqualToken, "!=");
            }
            return std::unexpected(err_unexpected_token("!"));
        case '<':
            return create_token(TokenType::LAngleToken, "<");
        case '>':
            return create_token(TokenType::RAngleToken, ">");
        case '{':
            return create_token(TokenType::LBracketToken, "{");
        case '}':
            return create_token(TokenType::RBracketToken, "}");
    }

    throw "not implemented";
}

Token Tokenizer::create_token(TokenType token_type, std::string_view raw) {
    auto offset = int(raw.length());
    col += offset;
    return Token{
        .type = TokenType::NotEqualToken,
        .raw = "!=",
        .pos = Position{ .line = line, .col = col-offset }
    };
}

std::string Tokenizer::err_unexpected_token(std::string_view raw) {
    return format_error(std::format("Unexpected token {}", raw));
}

std::string Tokenizer::format_error(std::string_view message) {
    return std::format("[{}:{}]: {}", line, col, message);
}

bool is_horizontal_space(std::string_view s) {
    return s.starts_with(' ') || s.starts_with('\t');
}

bool is_vertical_space(std::string_view s) {
    return s.starts_with('\v') || s.starts_with('\f') || is_newline(s);
}

bool is_newline(std::string_view s) {
    return s.starts_with(newline);
}
