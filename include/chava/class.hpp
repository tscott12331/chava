#ifndef CLASS_HPP
#define CLASS_HPP

#include <chava/stmt.hpp>
#include <chava/parser_misc.hpp>
#include <chava/exp.hpp>
#include <optional>
#include <string_view>
#include <vector>

struct CommaVardecValue {
    std::vector<Vardec> vardecs;

    bool operator==(const CommaVardecValue& other) const = default;
};

using CommaVardec = PositionWrapper<CommaVardecValue>;

struct MethodDefValue {
    std::string_view method_name;
    CommaVardec params;
    ParsedType ret_type;
    PositionWrapper<std::shared_ptr<BlockStmt>> body;

    bool operator==(const MethodDefValue& other) const = default;
};

using MethodDef = PositionWrapper<MethodDefValue>;

struct ConstructorValue {
    CommaVardec params;
    std::optional<CommaExp> super_args;
    std::vector<Stmt> stmts;

    bool operator==(const ConstructorValue& other) const = default;
};

using Constructor = PositionWrapper<ConstructorValue>;

struct ClassDefValue {
    std::string_view class_name;
    std::optional<std::string_view> extend_class_name;
    std::vector<PositionWrapper<VardecStmt>> vardecs;
    Constructor constructor;
    std::vector<MethodDef> method_defs;

    bool operator==(const ClassDefValue& other) const = default;
};

using ClassDef = PositionWrapper<ClassDefValue>;

#endif
