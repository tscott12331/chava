#include <chava/parser.hpp>
#include <chava/exp.hpp>
#include <chava/tokenizer.hpp>
#include <charconv>
#include <cstring>
#include <expected>
#include <memory>
#include <string_view>
#include <system_error>
std::expected<Expr, std::string_view> Parser::parse_exp() {
    auto token = get_token();
    if(!token) {
        return std::unexpected(token.error());
    }

    return parse_eq_exp();
}

std::expected<Expr, std::string_view> Parser::parse_eq_exp() {
    auto left = parse_comp_exp();
    if(!left) {
        return std::unexpected(left.error());
    }

    auto token = get_token();
    while(token) {
        Op op;
        switch(token->type) {
            case TokenType::NotEqualToken:
                op = Op::NotEq;
                break;
            case TokenType::EqualToken:
                op = Op::Eq;
                break;
            default:
                return std::move(left.value());
        }
        cursor += 1;

        auto right = parse_comp_exp();
        if(!right) {
            return std::unexpected(right.error());
        }

        left = std::make_unique<BinaryExp>(BinaryExp{
            .left=std::move(left.value()),
            .op=op,
            .right=std::move(right.value()),
        });

        token = get_token();
    }

    return std::move(left.value());
}

std::expected<Expr, std::string_view> Parser::parse_comp_exp() {
    auto left = parse_add_exp();
    if(!left) {
        return std::unexpected(left.error());
    }

    auto token = get_token();
    if(!token) {
        return std::move(left.value());
    }
    Op op;
    switch(token->type) {
        case TokenType::LAngleToken:
            op = Op::Lt;
            break;
        case TokenType::RAngleToken:
            op = Op::Gt;
            break;
        default:
            return std::move(left.value());
    }
    cursor += 1;

    auto right = parse_add_exp();
    if(!right) {
        return std::unexpected(right.error());
    }

    return std::make_unique<BinaryExp>(BinaryExp{
        .left=std::move(left.value()),
        .op=op,
        .right=std::move(right.value()),
    });
}

std::expected<Expr, std::string_view> Parser::parse_add_exp() {
    auto left = parse_mult_exp();
    if(!left) {
        return std::unexpected(left.error());
    }

    auto token = get_token();
    while(token) {
        Op op;
        switch(token->type) {
            case TokenType::PlusToken:
                op = Op::Add;
                break;
            case TokenType::DashToken:
                op = Op::Sub;
                break;
            default:
                return std::move(left.value());
        }
        cursor += 1;

        auto right = parse_mult_exp();
        if(!right) {
            return std::unexpected(right.error());
        }

        left = std::make_unique<BinaryExp>(BinaryExp{
            .left=std::move(left.value()),
            .op=op,
            .right=std::move(right.value()),
        });

        token = get_token();
    }

    return std::move(left.value());
}

std::expected<Expr, std::string_view> Parser::parse_mult_exp() {
    auto left = parse_call_exp();
    if(!left) {
        return std::unexpected(left.error());
    }

    auto token = get_token();
    while(token) {
        Op op;
        switch(token->type) {
            case TokenType::StarToken:
                op = Op::Mult;
                break;
            case TokenType::FSlashToken:
                op = Op::Div;
                break;
            default:
                return std::move(left.value());
        }
        cursor += 1;

        auto right = parse_call_exp();
        if(!right) {
            return std::unexpected(right.error());
        }

        left = std::make_unique<BinaryExp>(BinaryExp{
            .left=std::move(left.value()),
            .op=op,
            .right=std::move(right.value()),
        });

        token = get_token();
    }

    return std::move(left.value());
}

std::expected<Expr, std::string_view> Parser::parse_call_exp() {
    auto target = parse_prim_exp();
    if(!target) {
        return std::unexpected(target.error());
    }

    auto token = get_token_of(TokenType::DotToken);

    while(token) {
        cursor += 1;
        auto method_name = get_token_of(TokenType::IdentToken);
        if(!method_name) {
            return std::unexpected(method_name.error());
        }
        cursor += 1;

        token = get_token_of(TokenType::LParenToken);
        if(!token) {
            return std::unexpected(token.error());
        }
        cursor += 1;

        auto args = parse_comma_exp();
        if(!args) {
            return std::unexpected(args.error());
        }

        token = get_token_of(TokenType::RParenToken);
        cursor += 1;

        target = std::make_unique<MethodCallExp>(MethodCallExp{
            .target=std::move(target.value()),
            .method_name=method_name->raw,
            .args=std::move(args.value()),
        });

        token = get_token_of(TokenType::DotToken);
    }

    return std::move(target.value());
}

std::expected<Expr, std::string_view> Parser::parse_prim_exp() {
    auto token = get_token();
    if(!token) {
        return std::unexpected(token.error());
    }

    switch(token->type) {
        case TokenType::IdentToken:
            return parse_var_exp();
        case TokenType::NumberToken:
            return parse_num_lit_exp();
        case TokenType::StringToken:
            return parse_str_lit_exp();
        case TokenType::LParenToken:
            return parse_paren_exp();
        case TokenType::ThisToken:
            return parse_this_exp();
        case TokenType::BoolToken:
            return parse_bool_lit_exp();
        case TokenType::NewToken:
            return parse_new_obj_exp();
        default:
            return std::unexpected(unexpected_token(token.value()));
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

std::expected<StrLitExp, std::string_view> Parser::parse_str_lit_exp() {
    auto token = get_token_of(TokenType::StringToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;
    return StrLitExp{
        .str = token->raw,
    };
}

std::expected<Expr, std::string_view> Parser::parse_paren_exp() {
    auto token = get_token_of(TokenType::LParenToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    auto expr = parse_exp();
    if(!expr) {
        return std::unexpected(expr.error());
    }

    token = get_token_of(TokenType::RParenToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    return std::move(expr.value());
}

std::expected<ThisExp, std::string_view> Parser::parse_this_exp() {
    auto token = get_token_of(TokenType::ThisToken);
    if(!token) {
        return std::unexpected(token.error());
    }

    return ThisExp{};
}

std::expected<BoolLitExp, std::string_view> Parser::parse_bool_lit_exp() {
    auto token = get_token();
    if(!token) {
        return std::unexpected(token.error());
    }

    bool val;
    switch(token->type) {
        case TokenType::TrueToken:
            val = true;
            break;
        case TokenType::FalseToken:
            val = false;
            break;
        default:
            return std::unexpected(unexpected_token(token.value()));
    }

    return BoolLitExp{
        .val=val,
    };
}

std::expected<CommaExp, std::string_view> Parser::parse_comma_exp() {
    auto exp = parse_exp();
    std::vector<Expr> exps;
    if(!exp) {
        // assume empty CommaExp
        // if real error, higher up levels will fail and report error
        return CommaExp{
            .exps=std::move(exps),
        };
    }

    auto token = get_token_of(TokenType::CommaToken);
    while(token) {
        cursor += 1;
        exp = parse_exp();
        if(!exp) {
            return std::unexpected(exp.error());
        }

        exps.push_back(std::move(exp.value()));

        token = get_token_of(TokenType::CommaToken);
    }

    return CommaExp{
        .exps=std::move(exps),
    };
}

std::expected<std::unique_ptr<NewObjExp>, std::string_view> Parser::parse_new_obj_exp() {
    auto token = get_token_of(TokenType::NewToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    token = get_token_of(TokenType::IdentToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;
    auto class_name = token->raw;
    
    token = get_token_of(TokenType::LParenToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    auto args = parse_comma_exp();
    if(!args) {
        return std::unexpected(args.error());
    }

    token = get_token_of(TokenType::RParenToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    return std::make_unique<NewObjExp>(NewObjExp{
        .class_name=class_name,
        .args=std::move(args.value()),
    });
}
