#ifndef PARSER_HPP
#define PARSER_HPP

#include <chava/parser_misc.hpp>
#include <chava/type.hpp>
#include <chava/exp.hpp>
#include <chava/stmt.hpp>
#include <chava/tokenizer.hpp>
#include <chava/class.hpp>
#include <expected>
#include <string_view>
#include <vector>

struct Program {
    std::vector<ClassDef> classdefs;
    std::vector<Stmt> stmts;
};

class Parser {
public:
    Parser(std::vector<Token> tokens);
    static std::expected<Program, std::string_view> Parse(std::vector<Token> tokens);
    std::expected<Program, std::string_view> parse();
private:
    int cursor;

    std::vector<Token> tokens;

    std::expected<std::vector<ClassDef>, std::string_view> parse_classdefs();
    std::expected<std::vector<Stmt>, std::string_view> parse_stmts();

    std::expected<Stmt, std::string_view> parse_stmt();

    std::expected<Expr, std::string_view> parse_exp();
    std::expected<Expr, std::string_view> parse_eq_exp();
    std::expected<Expr, std::string_view> parse_comp_exp();
    std::expected<Expr, std::string_view> parse_add_exp();
    std::expected<Expr, std::string_view> parse_mult_exp();
    std::expected<Expr, std::string_view> parse_call_exp();
    std::expected<Expr, std::string_view> parse_prim_exp();

    std::expected<Type, std::string_view> parse_type();
    std::expected<Vardec, std::string_view> parse_vardec();

    std::expected<CommaVardec, std::string_view> parse_comma_vardec();
    std::expected<CommaExp, std::string_view> parse_comma_exp();

    std::expected<VardecStmt, std::string_view> parse_vardec_stmt();
    std::expected<std::unique_ptr<WhileStmt>, std::string_view> parse_while_stmt();
    std::expected<std::unique_ptr<ReturnStmt>, std::string_view> parse_return_stmt();
    std::expected<std::unique_ptr<IfStmt>, std::string_view> parse_if_stmt();
    std::expected<std::unique_ptr<BlockStmt>, std::string_view> parse_block_stmt();

    std::expected<VarExp, std::string_view> parse_var_exp();
    std::expected<NumLitExp, std::string_view> parse_num_lit_exp();
    std::expected<StrLitExp, std::string_view> parse_str_lit_exp();
    std::expected<Expr, std::string_view> parse_paren_exp();
    std::expected<ThisExp, std::string_view> parse_this_exp();
    std::expected<BoolLitExp, std::string_view> parse_bool_lit_exp();
    // TODO: add println support (need to decide if i want token or just general function parsing)
    std::expected<std::unique_ptr<NewObjExp>, std::string_view> parse_new_obj_exp();

    std::expected<MethodDef, std::string_view> parse_method_def();
    std::expected<Constructor, std::string_view> parse_constructor();
    std::expected<ClassDef, std::string_view> parse_classdef();

    std::expected<Token, std::string_view> get_token_of(TokenType token_type);
    std::expected<Token, std::string_view> get_token();

    std::string unexpected_token(Token &token);
};

#endif
