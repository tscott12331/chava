#include <chava/parser.hpp>
#include <chava/exp.hpp>
#include <chava/tokenizer.hpp>
#include <charconv>
#include <cstring>
#include <expected>
#include <memory>
#include <string_view>
#include <system_error>

std::expected<Exp, std::string> Parser::parse_exp() {
    auto token = get_token();
    if(!token) {
        return std::unexpected(token.error());
    }

    return parse_eq_exp();
}

std::expected<Exp, std::string> Parser::parse_eq_exp() {
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

        left = Exp{
                .value=std::make_shared<BinaryExp>(BinaryExp{
                .left=std::move(left.value()),
                .op=op,
                .right=std::move(right.value()),
            }),
            .pos=left->pos,
        };

        token = get_token();
    }

    return std::move(left.value());
}

std::expected<Exp, std::string> Parser::parse_comp_exp() {
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

    return Exp{
        .value=std::make_shared<BinaryExp>(BinaryExp{
            .left=std::move(left.value()),
            .op=op,
            .right=std::move(right.value()),
        }),
        .pos=left->pos,
    };
}

std::expected<Exp, std::string> Parser::parse_add_exp() {
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

        left = Exp{
                .value=std::make_shared<BinaryExp>(BinaryExp{
                .left=std::move(left.value()),
                .op=op,
                .right=std::move(right.value()),
            }),
            .pos=left->pos,
        };

        token = get_token();
    }

    return std::move(left.value());
}

std::expected<Exp, std::string> Parser::parse_mult_exp() {
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

        left = Exp{
                .value=std::make_shared<BinaryExp>(BinaryExp{
                .left=std::move(left.value()),
                .op=op,
                .right=std::move(right.value()),
            }),
            .pos=left->pos,
        };

        token = get_token();
    }

    return std::move(left.value());
}

std::expected<Exp, std::string> Parser::parse_call_exp() {
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

        target = Exp{
            .value=std::make_shared<MethodCallExp>(MethodCallExp(
                target.value(),
                method_name->raw,
                args.value()
            )),
            .pos=target->pos,
        };

        token = get_token_of(TokenType::DotToken);
    }

    return std::move(target.value());
}

std::expected<Exp, std::string> Parser::parse_prim_exp() {
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
        case TokenType::TrueToken:
        case TokenType::FalseToken:
            return parse_bool_lit_exp();
        case TokenType::NewToken:
            return parse_new_obj_exp();
        default:
            return std::unexpected(unexpected_token(token.value()));
    }
}

std::expected<Exp, std::string> Parser::parse_var_exp() {
    auto token = get_token_of(TokenType::IdentToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    return Exp{
        .value=VarExp(token->raw),
        .pos=std::move(token->pos),
    };
}

std::expected<Exp, std::string> Parser::parse_num_lit_exp() {
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

    return Exp{
        .value=NumLitExp{
            .val=val
        },
        .pos=token->pos,
    };
}

std::expected<Exp, std::string> Parser::parse_str_lit_exp() {
    auto token = get_token_of(TokenType::StringToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;
    return Exp{
        .value=StrLitExp{
            .str = token->raw,
        },
        .pos=token->pos,
    };
}

std::expected<Exp, std::string> Parser::parse_paren_exp() {
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

std::expected<Exp, std::string> Parser::parse_this_exp() {
    auto token = get_token_of(TokenType::ThisToken);
    if(!token) {
        return std::unexpected(token.error());
    }

    return Exp{
        .value=ThisExp{},
        .pos=token->pos,
    };
}

std::expected<Exp, std::string> Parser::parse_bool_lit_exp() {
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
    cursor += 1;

    return Exp{
        .value=BoolLitExp{
            .val=val,
        },
        .pos=token->pos,
    };
}

std::expected<CommaExp, std::string> Parser::parse_comma_exp() {
    auto exp = parse_exp();
    std::vector<Exp> exps;
    if(!exp) {
        // assume empty CommaExp
        // if real error, higher up levels will fail and report error
        return CommaExp {
            .value=CommaExpValue{
                .exps=std::move(exps),
            },
            .pos=exp->pos,
        };
    }

    exps.push_back(std::move(exp.value()));

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

    return CommaExp {
        .value=CommaExpValue{
            .exps=std::move(exps),
        },
        .pos=exp->pos,
    };
}

std::expected<Exp, std::string> Parser::parse_new_obj_exp() {
    auto new_token = get_token_of(TokenType::NewToken);
    if(!new_token) {
        return std::unexpected(new_token.error());
    }
    cursor += 1;

    auto token = get_token_of(TokenType::IdentToken);
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

    return Exp{
        .value=std::make_shared<NewObjExp>(NewObjExp{
            .class_name=class_name,
            .args=std::move(args.value()),
        }),
        .pos=new_token->pos,
    };
}


void MethodCallExp::annotate_ret_type(ParsedType ret_type) {
    this->ret_type = ret_type;
}

void VarExp::annotate_is_field(bool is_field) {
    this->is_field = is_field;
}
