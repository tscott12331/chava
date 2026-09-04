#include "chava/class.hpp"
#include <chava/parser.hpp>
#include <expected>

std::expected<CommaVardec, std::string> Parser::parse_comma_vardec() {
    std::vector<Vardec> vardecs;

    auto vardec = parse_vardec();
    if(!vardec) {
        return CommaVardec{
            .value=CommaVardecValue{
                .vardecs=std::move(vardecs),
            }
        };
    }

    vardecs.push_back(vardec.value());

    auto token = get_token_of(TokenType::CommaToken);
    while(token) {
        cursor += 1;

        vardec = parse_vardec();
        if(!vardec) {
            return std::unexpected(vardec.error());
        }
        vardecs.push_back(std::move(vardec.value()));

        token = get_token_of(TokenType::CommaToken);
    }

    return CommaVardec{
        .value=CommaVardecValue{
            .vardecs=std::move(vardecs),
        },
        .pos=vardec->pos,
    };
}

std::expected<MethodDef, std::string> Parser::parse_method_def() {
    auto method_token = get_token_of(TokenType::MethodToken);
    if(!method_token) {
        return std::unexpected(method_token.error());
    }
    cursor += 1;

    auto method_name = get_token_of(TokenType::IdentToken);
    if(!method_name) {
        return std::unexpected(method_name.error());
    }
    cursor += 1;

    auto token = get_token_of(TokenType::LParenToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    auto params = parse_comma_vardec();
    if(!params) {
        return std::unexpected(params.error());
    }

    token = get_token_of(TokenType::RParenToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    auto ret_type = parse_type();
    if(!ret_type) {
        return std::unexpected(ret_type.error());
    }

    auto body = parse_block_stmt();
    if(!body) {
         return std::unexpected(body.error());
    }

    return MethodDef{
        .value=MethodDefValue{
            .method_name=method_name->raw,
            .params=std::move(params.value()),
            .ret_type=std::move(ret_type.value()),
            .body=std::move(body).value().to<std::shared_ptr<BlockStmt>>(),
        },
        .pos=method_token->pos,
    };
}

std::expected<Constructor, std::string> Parser::parse_constructor() {
    auto init_token = get_token_of(TokenType::InitToken);
    if(!init_token) {
        return std::unexpected(init_token.error());
    }
    cursor += 1;

    auto token = get_token_of(TokenType::LParenToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    auto params = parse_comma_vardec();
    if(!params) {
        return std::unexpected(params.error());
    }

    token = get_token_of(TokenType::RParenToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    token = get_token_of(TokenType::LBracketToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    std::optional<CommaExp> super_args = std::nullopt;
    token = get_token_of(TokenType::SuperToken);
    if(token) {
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
        super_args = std::move(args.value());

        token = get_token_of(TokenType::RParenToken);
        if(!token) {
            return std::unexpected(token.error());
        }
        cursor += 1;

        token = get_token_of(TokenType::SemiColonToken);
        if(!token) {
            return std::unexpected(token.error());
        }
        cursor += 1;
    }

    std::vector<Stmt> stmts;
    token = get_token();
    while(token && token.value().type != TokenType::RBracketToken) {
        auto stmt = parse_stmt();
        stmts.push_back(std::move(stmt.value()));

        token = get_token();
    }

    token = get_token_of(TokenType::RBracketToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    return Constructor{
        .value=ConstructorValue{
            .params=std::move(params.value()),
            .super_args=std::move(super_args),
            .stmts=std::move(stmts),
        },
        .pos=init_token->pos,
    };
}

std::expected<ClassDef, std::string> Parser::parse_classdef() {
    auto class_token = get_token_of(TokenType::ClassToken);
    if(!class_token) {
        return std::unexpected(class_token.error());
    }
    cursor += 1;

    auto class_name = get_token_of(TokenType::IdentToken);
    if(!class_name) {
        return std::unexpected(class_name.error());
    }
    cursor += 1;

    std::optional<std::string_view> extend_class_name = std::nullopt;
    auto token = get_token_of(TokenType::ExtendsToken);
    if(token) {
        cursor += 1;

        token = get_token_of(TokenType::IdentToken);
        if(!token) {
            return std::unexpected(token.error());
        }
        cursor += 1;

        extend_class_name = token->raw;
    }

    token = get_token_of(TokenType::LBracketToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    std::vector<PositionWrapper<VardecStmt>> vardecs;
    token = get_token();
    while(token && token->type != TokenType::InitToken) {
        auto vardec = parse_vardec_stmt();
        if(!vardec) {
            return std::unexpected(vardec.error());
        }

        vardecs.push_back(std::move(vardec->to<VardecStmt>()));

        token = get_token();
    }

    auto constructor = parse_constructor();
    if(!constructor) {
        return std::unexpected(constructor.error());
    }

    std::vector<MethodDef> method_defs;
    token = get_token();
    while(token && token->type != TokenType::RBracketToken) {
        auto method_def = parse_method_def();
        if(!method_def) {
            return std::unexpected(method_def.error());
        }
        method_defs.push_back(std::move(method_def.value()));
        token = get_token();
    }

    token = get_token_of(TokenType::RBracketToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    return ClassDef{
        .value=ClassDefValue{
            .class_name=class_name->raw,
            .extend_class_name=extend_class_name,
            .vardecs=std::move(vardecs),
            .constructor=std::move(constructor.value()),
            .method_defs=std::move(method_defs),
        },
        .pos=class_token->pos,
    };
}
