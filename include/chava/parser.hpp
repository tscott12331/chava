#ifndef PARSER_HPP
#define PARSER_HPP

#include <chava/parser_misc.hpp>
#include <chava/type.hpp>
#include <chava/exp.hpp>
#include <chava/stmt.hpp>
#include <chava/tokenizer.hpp>
#include <chava/class.hpp>
#include <expected>
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

    std::expected<Expr, std::string> parse_exp();
    std::expected<Expr, std::string> parse_eq_exp();
    std::expected<Expr, std::string> parse_comp_exp();
    std::expected<Expr, std::string> parse_add_exp();
    std::expected<Expr, std::string> parse_mult_exp();
    std::expected<Expr, std::string> parse_call_exp();
    std::expected<Expr, std::string> parse_prim_exp();

    std::expected<ParsedType, std::string> parse_type();
    std::expected<Vardec, std::string> parse_vardec();

    std::expected<CommaVardec, std::string> parse_comma_vardec();
    std::expected<CommaExp, std::string> parse_comma_exp();

    std::expected<AssignStmt, std::string> parse_assign_stmt();
    std::expected<VardecStmt, std::string> parse_vardec_stmt();
    std::expected<ExpStmt, std::string> parse_exp_stmt();
    std::expected<std::shared_ptr<WhileStmt>, std::string> parse_while_stmt();
    std::expected<std::shared_ptr<ReturnStmt>, std::string> parse_return_stmt();
    std::expected<std::shared_ptr<IfStmt>, std::string> parse_if_stmt();
    std::expected<std::shared_ptr<BlockStmt>, std::string> parse_block_stmt();

    std::expected<VarExp, std::string> parse_var_exp();
    std::expected<NumLitExp, std::string> parse_num_lit_exp();
    std::expected<StrLitExp, std::string> parse_str_lit_exp();
    std::expected<Expr, std::string> parse_paren_exp();
    std::expected<ThisExp, std::string> parse_this_exp();
    std::expected<BoolLitExp, std::string> parse_bool_lit_exp();
    // TODO: add println support (need to decide if i want token or just general function parsing)
    std::expected<std::shared_ptr<NewObjExp>, std::string> parse_new_obj_exp();

    std::expected<MethodDef, std::string> parse_method_def();
    std::expected<Constructor, std::string> parse_constructor();
    std::expected<ClassDef, std::string> parse_classdef();

    std::expected<Token, std::string> get_token_of(TokenType token_type);
    std::expected<Token, std::string> get_token();

    std::string unexpected_token(Token &token);
};

#endif
