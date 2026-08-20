#include <chava/parser.hpp>
#include <chava/stmt.hpp>
#include <expected>

std::expected<Stmt, std::string_view> Parser::parse_stmt() {
    if(cursor >= tokens.size()) {
        return std::unexpected("Empty statement");
    }

    auto token = tokens.at(cursor);

    switch(token.type) {
        case TokenType::BreakToken:
            cursor += 1;
            return std::make_unique<BreakStmt>(BreakStmt{});
        case TokenType::IntToken:
        case TokenType::BoolToken:
        case TokenType::VoidToken:
            return parse_vardec_stmt();
        case TokenType::WhileToken:
            return parse_while_stmt();
        case TokenType::ReturnToken:
            return parse_return_stmt();
        case TokenType::IfToken:
            return parse_if_stmt();
        case TokenType::LBracketToken:
            return parse_block_stmt();
        default:
            return std::unexpected(unexpected_token(token));
    }
}

std::expected<VardecStmt, std::string_view> Parser::parse_vardec_stmt() {
    auto vardec = parse_vardec();
    if(!vardec) {
        return std::unexpected(vardec.error());
    }

    auto token = get_token_of(TokenType::SemiColonToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    return VardecStmt{
        .vardec=std::move(vardec.value()),
    };
}

std::expected<std::unique_ptr<ReturnStmt>, std::string_view> Parser::parse_return_stmt() {
    auto token = get_token_of(TokenType::ReturnToken);
    if(!token) {
        return std::unexpected(token.error());
    }

    cursor += 1;

    token = get_token();
    if(!token) {
        return std::unexpected(token.error());
    }

    if(token->type == TokenType::SemiColonToken) {
        return std::make_unique<ReturnStmt>(ReturnStmt{
            .val=std::nullopt
        });
    }

    auto val = parse_exp();
    if(!val) {
        return std::unexpected(val.error());
    }

    token = get_token_of(TokenType::SemiColonToken);
    if(!token) {
        return std::unexpected(token.error());
    }

    return std::make_unique<ReturnStmt>(ReturnStmt{
        .val=std::move(val.value())
    });
}

std::expected<std::unique_ptr<WhileStmt>, std::string_view> Parser::parse_while_stmt() {
    auto token = get_token_of(TokenType::WhileToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    token = get_token_of(TokenType::LParenToken);
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

    return std::make_unique<WhileStmt>(WhileStmt{
        .guard=std::move(guard.value()),
        .body=std::move(body.value())
    });
}

std::expected<std::unique_ptr<IfStmt>, std::string_view> Parser::parse_if_stmt() {
    auto token = get_token_of(TokenType::IfToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    token = get_token_of(TokenType::LParenToken);
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

    return std::make_unique<IfStmt>(IfStmt{
        .guard=std::move(guard.value()),
        .body=std::move(body.value()),
        .else_body=std::move(else_body)
    });
}

std::expected<std::unique_ptr<BlockStmt>, std::string_view> Parser::parse_block_stmt() {
    auto token = get_token_of(TokenType::LBracketToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    std::vector<Stmt> stmts;
    
    while((token = get_token()) && token && token->type != TokenType::RBracketToken) {
        auto stmt = parse_stmt();
        if(!stmt) {
            return std::unexpected(stmt.error());
        }

        stmts.push_back(std::move(stmt.value()));
    }

    token = get_token_of(TokenType::RBracketToken);
    if(!token) {
        return std::unexpected(token.error());
    }
    cursor += 1;

    return std::make_unique<BlockStmt>(BlockStmt{
        .stmts=std::move(stmts)
    });
}

