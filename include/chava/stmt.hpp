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

    bool operator==(const ExpStmt& other) const = default;
};

struct VardecStmt {
    Vardec vardec;

    bool operator==(const VardecStmt& other) const = default;
};

struct AssignStmt {
    std::string_view var;
    Exp val;

    bool operator==(const AssignStmt& other) const = default;
};

struct WhileStmt {
    Exp guard;
    Stmt body;

    bool operator==(const WhileStmt& other) const = default;
};

struct BreakStmt {
    bool operator==(const BreakStmt& other) const = default;
};

struct ReturnStmt {
    std::optional<Exp> val;

    bool operator==(const ReturnStmt& other) const = default;
};

struct IfStmt {
    Exp guard;
    Stmt body;
    std::optional<Stmt> else_body;

    bool operator==(const IfStmt& other) const = default;
};

struct BlockStmt {
    std::vector<Stmt> stmts;

    bool operator==(const BlockStmt& other) const = default;
};

bool operator==(const StmtVariant& left, const StmtVariant& right);

// formatter specializations are simple for now
// template <>
// struct std::formatter<PositionWrapper<ExpStmt>> : std::formatter<std::string> {
//     template <typename FormatContext>
//     auto format(const PositionWrapper<ExpStmt>& stmt, FormatContext& ctx) const {
//         return std::formatter<std::string>::format("ExpStmt", ctx);
//     }
// };

#endif
