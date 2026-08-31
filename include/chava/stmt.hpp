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

using StmtVariant = std::variant<
    ExpStmt,
    VardecStmt,
    AssignStmt,
    std::shared_ptr<WhileStmt>,
    std::shared_ptr<BreakStmt>,
    std::shared_ptr<ReturnStmt>,
    std::shared_ptr<IfStmt>,
    std::shared_ptr<BlockStmt>
>;

using Stmt = PositionWrapper<StmtVariant>;

struct ExpStmt {
    Exp exp;
};

struct VardecStmt {
    Vardec vardec;
};

struct AssignStmt {
    std::string_view var;
    Exp val;
};

struct WhileStmt {
    Exp guard;
    Stmt body;
};

struct BreakStmt {};

struct ReturnStmt {
    std::optional<Exp> val;
};

struct IfStmt {
    Exp guard;
    Stmt body;
    std::optional<Stmt> else_body;
};

struct BlockStmt {
    std::vector<Stmt> stmts;
};

#endif
