#include "chava/exp.hpp"
#include "chava/parser.hpp"
#include "chava/tokenizer.hpp"
#include <charconv>
#include <cstring>
#include <expected>
#include <string_view>
#include <system_error>
std::expected<Expr, std::string_view> Parser::parse_exp() {
    auto token = get_token();
    if(!token) {
        return std::unexpected(token.error());
    }

    switch(token->type) {
        case TokenType::IdentToken:
            return parse_var_exp();
        case TokenType::NumberToken:
            return parse_num_lit_exp();

    }
}

std::expected<VarExp, std::string_view> Parser::parse_var_exp() {
    auto token = get_token_of(TokenType::IdentToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    return VarExp{
        .var=token->raw
    };
}

std::expected<NumLitExp, std::string_view> Parser::parse_num_lit_exp() {
    auto token = get_token_of(TokenType::NumberToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    int val{};
    auto [_, ec] = std::from_chars(token->raw.data(), token->raw.data()+token->raw.size(), val);
    if(ec != std::errc{}) {
        return std::unexpected(std::make_error_code(ec).message());
    }

    return NumLitExp{
        .val=val
    };
}
