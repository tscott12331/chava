#include "tokenizer.hpp"
#include <cctype>
#include <expected>
#include <format>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#if defined(_WIN32) || defined(_WIN64)
    std::string newline = "\r\n";
#else
    std::string newline = "\n";
#endif

bool is_horizontal_space(std::string_view s);
bool is_vertical_space(std::string_view s);
bool is_newline(std::string_view s);
bool is_valid_keyword_or_ident_char(char c);
bool is_num(char c);

std::unordered_map<std::string_view, TokenType> keyword_map = {
    {"int", TokenType::IntToken},
    {"bool", TokenType::BoolToken},
    {"void", TokenType::VoidToken},
    {"this", TokenType::ThisToken},
    {"true", TokenType::TrueToken},
    {"false", TokenType::FalseToken},
    {"new", TokenType::NewToken},
    {"while", TokenType::WhileToken},
    {"break", TokenType::BreakToken},
    {"return", TokenType::ReturnToken},
    {"if", TokenType::IfToken},
    {"else", TokenType::ElseToken},
    {"method", TokenType::MethodToken},
    {"init", TokenType::InitToken},
    {"super", TokenType::SuperToken},
    {"class", TokenType::ClassToken},
    {"extends", TokenType::ExtendsToken},
};

std::vector<Token> Tokenizer::Tokenize(std::string_view input) {
    Tokenizer t(input);
    return t.tokenize();
}

Tokenizer::Tokenizer(std::string_view input) : input(input), offset(0), line(1), col(1) {}

std::vector<Token> Tokenizer::tokenize() {
    std::vector<Token> tokens;
    while(offset < input.length()) {
        skip_whitespace();
        auto token = get_token();
        if(token) {
            if(*token) {
                tokens.push_back(token->value());
            }
        } else {
            std::cout << token.error();
            break;
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
        } else if(!is_horizontal_space(sub) && !is_vertical_space(sub)) {
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
        // symbols
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
        case '.':
            return create_token(TokenType::DotToken, ".");
        case '+':
            return create_token(TokenType::PlusToken, "+");
        case '-':
            return create_token(TokenType::DashToken, "-");
        case '/':
            return create_token(TokenType::FSlashToken, "/");
        case '*':
            return create_token(TokenType::StarToken, "*");
        default:
            return get_num_keyword_or_identifier();
    }
}

std::expected<std::optional<Token>, std::string> Tokenizer::get_num_keyword_or_identifier() {
    if(is_num(input.at(offset))) {
        return get_num();
    }

    if(!isalpha(input.at(offset))) {
        return std::unexpected(err_unexpected_token(input.substr(offset, 1)));
    }

    auto index = offset;
    while(index < input.length() && is_valid_keyword_or_ident_char(input.at(index))) {
        index += 1;
    }

    auto word = input.substr(offset, index-offset);
    if(keyword_map.contains(word)) {
        auto token_type = keyword_map[word];
        return create_token(token_type, word);
    }

    return create_token(TokenType::IdentToken, word);
}

std::expected<std::optional<Token>, std::string> Tokenizer::get_num() {
    if(!is_num(input.at(offset))) {
        return std::unexpected(err_unexpected_token(input.substr(offset, 1)));
    }

    auto index = offset;
    while(index < input.length() && is_num(input.at(index))) {
        index += 1;
    }

    const auto raw_num = input.substr(offset, index-offset);
    return create_token(TokenType::NumberToken, raw_num);
}

Token Tokenizer::create_token(TokenType token_type, std::string_view raw) {
    auto wordLen = int(raw.length());
    col += wordLen;
    offset += wordLen;
    return Token{
        .type = token_type,
        .raw = raw,
        .pos = Position{ .line = line, .col = col-wordLen }
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

bool is_num(char c) {
    return c >= '0' && c <= '9';
}

bool is_valid_keyword_or_ident_char(char c) {
    return isalnum(c) || c == '_';
}


std::string_view token_to_string(Token token) {
    switch(token.type) {
        case TokenType::IntToken:
            return "int";
        case TokenType::BoolToken:
            return "bool";
        case TokenType::VoidToken:
            return "void";

        // case TokenType::control: flow
        case TokenType::IfToken:
            return "if";
        case TokenType::ElseToken:
            return "else";
        case TokenType::WhileToken:
            return "while";
        case TokenType::BreakToken:
            return "break";
        case TokenType::ReturnToken:
            return "return";

        // case TokenType::ops:
        case TokenType::PlusToken:
            return "+";
        case TokenType::DashToken:
            return "-";
        case TokenType::FSlashToken:
            return "/";
        case TokenType::StarToken:
            return "*";

        // case TokenType::literals:
        case TokenType::TrueToken:
            return "true";
        case TokenType::FalseToken:
            return "false";
        case TokenType::IdentToken:
        case TokenType::NumberToken:
            return token.raw;

        // case TokenType::class:
        case TokenType::ClassToken:
            return "class";
        case TokenType::ExtendsToken:
            return "extends";
        case TokenType::InitToken:
            return "init";
        case TokenType::MethodToken:
            return "method";
        case TokenType::NewToken:
            return "new";
        case TokenType::ThisToken:
            return "this";
        case TokenType::SuperToken:
            return "super";

        // case TokenType::syntax:
        case TokenType::CommaToken:
            return ",";
        case TokenType::LParenToken:
            return "(";
        case TokenType::RParenToken:
            return ")";
        case TokenType::SemiColonToken:
            return ";";
        case TokenType::AssignToken:
            return "=";
        case TokenType::EqualToken:
            return "==";
        case TokenType::NotEqualToken:
            return "!=";
        case TokenType::LAngleToken:
            return "<";
        case TokenType::RAngleToken:
            return ">";
        case TokenType::LBracketToken:
            return "{";
        case TokenType::RBracketToken:
            return "}";
        case TokenType::DotToken:
            return ".";
        default:
            return "[unknown token]";
    }
}
