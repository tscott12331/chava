#ifndef PARSER_HPP
#define PARSER_HPP

#include "chava/common.hpp"
#include <chava/parser_misc.hpp>
#include <chava/type.hpp>
#include <chava/exp.hpp>
#include <chava/stmt.hpp>
#include <chava/tokenizer.hpp>
#include <chava/class.hpp>
#include <expected>
#include <format>
#include <variant>
#include <vector>

struct Program {
    std::vector<ClassDef> classdefs;
    std::vector<Stmt> stmts;
};

class Parser {
public:
    Parser(std::vector<Token> tokens);
    static std::expected<Program, std::string> Parse(std::vector<Token> tokens);
    std::expected<Program, std::string> parse();
private:
    int cursor;

    std::vector<Token> tokens;

    std::expected<std::vector<ClassDef>, std::string> parse_classdefs();
    std::expected<std::vector<Stmt>, std::string> parse_stmts();

    std::expected<Stmt, std::string> parse_stmt();

    std::expected<Exp, std::string> parse_exp();
    std::expected<Exp, std::string> parse_eq_exp();
    std::expected<Exp, std::string> parse_comp_exp();
    std::expected<Exp, std::string> parse_add_exp();
    std::expected<Exp, std::string> parse_mult_exp();
    std::expected<Exp, std::string> parse_call_exp();
    std::expected<Exp, std::string> parse_prim_exp();

    std::expected<ParsedType, std::string> parse_type();
    std::expected<Vardec, std::string> parse_vardec();

    std::expected<CommaVardec, std::string> parse_comma_vardec();
    std::expected<CommaExp, std::string> parse_comma_exp();

    std::expected<Stmt, std::string> parse_assign_stmt();
    std::expected<Stmt, std::string> parse_vardec_stmt();
    std::expected<Stmt, std::string> parse_exp_stmt();
    std::expected<Stmt, std::string> parse_while_stmt();
    std::expected<Stmt, std::string> parse_return_stmt();
    std::expected<Stmt, std::string> parse_if_stmt();
    std::expected<Stmt, std::string> parse_block_stmt();

    std::expected<Exp, std::string> parse_var_exp();
    std::expected<Exp, std::string> parse_num_lit_exp();
    std::expected<Exp, std::string> parse_str_lit_exp();
    std::expected<Exp, std::string> parse_paren_exp();
    std::expected<Exp, std::string> parse_this_exp();
    std::expected<Exp, std::string> parse_bool_lit_exp();
    // TODO: add println support (need to decide if i want token or just general function parsing)
    std::expected<Exp, std::string> parse_new_obj_exp();

    std::expected<MethodDef, std::string> parse_method_def();
    std::expected<Constructor, std::string> parse_constructor();
    std::expected<ClassDef, std::string> parse_classdef();

    std::expected<Token, std::string> get_token_of(TokenType token_type);
    std::expected<Token, std::string> get_token();

    std::string unexpected_token(Token &token);
};

// formatter specializations are simple for now
template <>
struct std::formatter<Stmt> : std::formatter<std::string> {
    template <typename FormatContext>
    auto format(const Stmt& stmt, FormatContext& ctx) const {
        auto str = std::visit(overloaded {
            [](const ExpStmt& exp_stmt) -> std::string { return "ExpStmt"; },
            [](const VardecStmt& exp_stmt) -> std::string { return "VardecStmt"; },
            [](const AssignStmt& exp_stmt) -> std::string { return "AssignStmt"; },
            [](const std::shared_ptr<WhileStmt>& exp_stmt) -> std::string { return "WhileStmt"; },
            [](const std::shared_ptr<BreakStmt>& exp_stmt) -> std::string { return "BreakStmt"; },
            [](const std::shared_ptr<ReturnStmt>& exp_stmt) -> std::string { return "ReturnStmt"; },
            [](const std::shared_ptr<IfStmt>& exp_stmt) -> std::string { return "IfStmt"; },
            [](const std::shared_ptr<BlockStmt>& exp_stmt) -> std::string { return "BlockStmt"; },
        }, stmt.value);

        return std::formatter<std::string>::format(str, ctx);
    }
};

#endif
