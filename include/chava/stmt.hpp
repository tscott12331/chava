#ifndef STMT_HPP
#define STMT_HPP

#include <memory>
#include <optional>
#include <variant>
#include <chava/exp.hpp>
#include <chava/type.hpp>
#include <chava/parser_misc.hpp>
#include <vector>

struct ExpStmt;
struct VardecStmt;
struct AssignStmt;
struct WhileStmt;
struct BreakStmt;
struct ReturnStmt;
struct IfStmt;
struct BlockStmt;

using Stmt = std::variant<
    ExpStmt,
    VardecStmt,
    AssignStmt,
    std::shared_ptr<WhileStmt>,
    std::shared_ptr<BreakStmt>,
    std::shared_ptr<ReturnStmt>,
    std::shared_ptr<IfStmt>,
    std::shared_ptr<BlockStmt>
>;

struct ExpStmt {
    Expr exp;
};

struct VardecStmt {
    Vardec vardec;
};

struct AssignStmt {
    std::string_view var;
    Expr val;
};

struct WhileStmt {
    Expr guard;
    Stmt body;
};

struct BreakStmt {};

struct ReturnStmt {
    std::optional<Expr> val;
};

struct IfStmt {
    Expr guard;
    Stmt body;
    std::optional<Stmt> else_body;
};

struct BlockStmt {
    std::vector<Stmt> stmts;
};

#endif
