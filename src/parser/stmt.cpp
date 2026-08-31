#include <chava/parser.hpp>
#include <chava/stmt.hpp>
#include <expected>

std::expected<Stmt, std::string> Parser::parse_stmt() {
    if(cursor >= tokens.size()) {
        return std::unexpected("Empty statement");
    }

    auto token = tokens.at(cursor);

    switch(token.type) {
        case TokenType::BreakToken:
            cursor += 1;
            return Stmt{
                .value=std::make_shared<BreakStmt>(BreakStmt{}),
                .pos=std::move(token.pos),
            };
        case TokenType::IntToken:
        case TokenType::BoolToken:
        case TokenType::VoidToken:
            return parse_vardec_stmt();
        case TokenType::IdentToken: {
            if(cursor + 1 >= tokens.size()) {
                return std::unexpected(unexpected_token(token));
            }

            auto next = tokens.at(cursor+1);
            if(next.type == TokenType::AssignToken) {
                return parse_assign_stmt();
            } else if(next.type == TokenType::IdentToken) {
                return parse_vardec_stmt();
            } else {
                return parse_exp_stmt();
            }
        }
        case TokenType::WhileToken:
            return parse_while_stmt();
        case TokenType::ReturnToken:
            return parse_return_stmt();
        case TokenType::IfToken:
            return parse_if_stmt();
        case TokenType::LBracketToken:
            return parse_block_stmt();
        default:
            return parse_exp_stmt();
    }
}

std::expected<Stmt, std::string> Parser::parse_assign_stmt() {
    auto var_token = get_token_of(TokenType::IdentToken);
    if(!var_token) {
        return std::unexpected(var_token.error());
    }
    cursor += 1;

    auto token = get_token_of(TokenType::AssignToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    auto val = parse_exp();
    if(!val) {
        return std::unexpected(val.error());
    }

    token = get_token_of(TokenType::SemiColonToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    return Stmt{
        .value=AssignStmt{
            .var=var_token->raw,
            .val=std::move(val.value()),
        },
        .pos=std::move(var_token->pos),
    };
}

std::expected<Stmt, std::string> Parser::parse_vardec_stmt() {
    auto vardec = parse_vardec();
    if(!vardec) {
        return std::unexpected(vardec.error());
    }

    auto token = get_token_of(TokenType::SemiColonToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    return Stmt{
            .value=VardecStmt{
            .vardec=std::move(vardec.value()),
        },
        .pos=vardec->pos,
    };
}

std::expected<Stmt, std::string> Parser::parse_exp_stmt() {
    auto exp = parse_exp();
    if(!exp) {
        return std::unexpected(exp.error());
    }

    auto token = get_token_of(TokenType::SemiColonToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    return Stmt{
        .value=ExpStmt{
            .exp=std::move(exp.value()),
        },
        .pos=exp->pos,
    };
}

std::expected<Stmt, std::string> Parser::parse_return_stmt() {
    auto return_token = get_token_of(TokenType::ReturnToken);
    if(!return_token) {
        return std::unexpected(return_token.error());
    }

    cursor += 1;

    auto token = get_token();
    if(!token) {
        return std::unexpected(token.error());
    }

    if(token->type == TokenType::SemiColonToken) {
        return Stmt{
            .value=std::make_shared<ReturnStmt>(ReturnStmt{
                .val=std::nullopt
            }),
            .pos=return_token->pos,
        };
    }

    auto val = parse_exp();
    if(!val) {
        return std::unexpected(val.error());
    }

    token = get_token_of(TokenType::SemiColonToken);
    if(!token) {
        return std::unexpected(token.error());
    }

    return Stmt{
        .value=std::make_shared<ReturnStmt>(ReturnStmt{
            .val=std::move(val.value())
        }),
        .pos=std::move(return_token->pos),
    };
}

std::expected<Stmt, std::string> Parser::parse_while_stmt() {
    auto while_token = get_token_of(TokenType::WhileToken);
    if(!while_token) {
        return std::unexpected(while_token.error());
    }
    cursor += 1;

    auto token = get_token_of(TokenType::LParenToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    auto guard = parse_exp();

    token = get_token_of(TokenType::RParenToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    auto body = parse_stmt();
    if(!body) {
        return std::unexpected(body.error());
    }

    return Stmt{
        .value=std::make_shared<WhileStmt>(WhileStmt{
            .guard=std::move(guard.value()),
            .body=std::move(body.value())
        }),
        .pos=std::move(while_token->pos),
    };
}

std::expected<Stmt, std::string> Parser::parse_if_stmt() {
    auto if_token = get_token_of(TokenType::IfToken);
    if(!if_token) {
        return std::unexpected(if_token.error());
    }
    cursor += 1;

    auto token = get_token_of(TokenType::LParenToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    auto guard = parse_exp();
    if(!guard) {
        return std::unexpected(guard.error());
    }

    token = get_token_of(TokenType::RParenToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    auto body = parse_stmt();
    if(!body) {
        return std::unexpected(body.error());
    }

    token = get_token_of(TokenType::ElseToken);
    std::optional<Stmt> else_body = std::nullopt;
    if(token) {
        cursor += 1;
        auto stmt = parse_stmt();
        if(!stmt) {
            return std::unexpected(stmt.error());
        }

        else_body = std::move(stmt.value());
    }

    return Stmt{
        .value=std::make_shared<IfStmt>(IfStmt{
            .guard=std::move(guard.value()),
            .body=std::move(body.value()),
            .else_body=std::move(else_body)
        }),
        .pos=std::move(if_token->pos),
    };
}

std::expected<Stmt, std::string> Parser::parse_block_stmt() {
    auto block_start_token = get_token_of(TokenType::LBracketToken);
    if(!block_start_token) {
        return std::unexpected(block_start_token.error());
    }
    cursor += 1;

    std::vector<Stmt> stmts;
    
    std::expected<Token, std::string> bracket_token;
    while((bracket_token = get_token()) && bracket_token && bracket_token->type != TokenType::RBracketToken) {
        auto stmt = parse_stmt();
        if(!stmt) {
            return std::unexpected(stmt.error());
        }

        stmts.push_back(std::move(stmt.value()));
    }

    bracket_token = get_token_of(TokenType::RBracketToken);
    if(!bracket_token) {
        return std::unexpected(bracket_token.error());
    }
    cursor += 1;

    return Stmt{
        .value=std::make_shared<BlockStmt>(BlockStmt{
            .stmts=std::move(stmts)
        }),
        .pos=std::move(block_start_token->pos),
    };
}

