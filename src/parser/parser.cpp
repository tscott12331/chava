#include <chava/tokenizer.hpp>
#include <chava/parser.hpp>
#include <format>
#include <vector>

std::string empty_statement();
std::string unexpected_token(Token& token);

Parser::Parser(std::vector<Token> tokens) : tokens(tokens), cursor(0) {}

std::expected<Program, std::string> Parser::Parse(std::vector<Token> tokens) {
    auto parser = Parser(tokens);
    return parser.parse();
}

std::expected<Program, std::string> Parser::parse() {
    auto classdefs_res = parse_classdefs();
    if(!classdefs_res) {
        return std::unexpected(classdefs_res.error());
    }

    auto stmts_res = parse_stmts();
    if(!stmts_res) {
        return std::unexpected(stmts_res.error());
    }

    return Program{
        .classdefs=std::move(classdefs_res.value()),
        .stmts=std::move(stmts_res.value()),
    };
}

std::expected<std::vector<ClassDef>, std::string> Parser::parse_classdefs() {
    std::vector<ClassDef> classdefs;
    auto token = get_token_of(TokenType::ClassToken);
    while(token) {
        auto classdef = parse_classdef();
        if(!classdef) {
            return std::unexpected(classdef.error());
        }

        classdefs.push_back(classdef.value());
        token = get_token_of(TokenType::ClassToken);
    }

    return std::move(classdefs);
}

std::expected<std::vector<Stmt>, std::string> Parser::parse_stmts() {
    std::vector<Stmt> stmts;
    auto token = get_token();
    while(token) {
        auto stmt = parse_stmt();
        if(!stmt) {
            return std::unexpected(stmt.error());
        }

        stmts.push_back(std::move(stmt.value()));

        token = get_token();
    }

    if(stmts.size() == 0) {
        return std::unexpected("A program must have at least one statement");
    }

    return std::move(stmts);
}

std::string empty_statement() {
    return "Empty statement";
}

std::expected<Token, std::string> Parser::get_token_of(TokenType token_type) {
    auto token = get_token();
    if(!token) {
        return std::unexpected(token.error());
    }

    if(token->type != token_type) {
        return std::unexpected(unexpected_token(*token));
    }

    return token;
}

std::expected<Token, std::string> Parser::get_token() {
    if(cursor >= tokens.size()) {
        return std::unexpected(empty_statement());
    }

    return tokens.at(cursor);
}

std::string Parser::unexpected_token(Token& token) {
    return std::format("[{}:{}]: unexpected token {}", token.pos.line, token.pos.col, token_to_string(token));
}
