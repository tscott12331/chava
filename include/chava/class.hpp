#ifndef CLASS_HPP
#define CLASS_HPP

#include <chava/stmt.hpp>
#include <chava/parser_misc.hpp>
#include <chava/exp.hpp>
#include <optional>
#include <string_view>
#include <vector>

struct CommaVardec {
    std::vector<Vardec> vardecs;
};

struct MethodDef {
    std::string_view method_name;
    CommaVardec params;
    Type ret_type;
    std::unique_ptr<BlockStmt> body;
};

struct Constructor {
    CommaVardec params;
    std::optional<CommaExp> super_args;
    std::vector<Stmt> stmts;
};

struct ClassDef {
    std::string_view class_name;
    std::optional<std::string_view> extend_class_name;
    std::vector<VardecStmt> vardecs;
    Constructor constructor;
    std::vector<MethodDef> method_defs;
};

#endif
