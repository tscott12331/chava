#ifndef TYPECHECKER_HPP
#define TYPECHECKER_HPP

#include "chava/stmt.hpp"
#include <chava/parser.hpp>
#include <expected>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class Type {
public:
    Type(std::string name) : name(name) {};

    std::string_view get_name();
    virtual std::expected<std::reference_wrapper<Type>, std::string> resolve_method_call_ret(MethodCallExp& method_call) = 0;
    virtual bool is_subtype_of(Type& other) = 0;

    virtual ~Type() = default;

private:
    std::string name;
};

class PrimitiveType : public Type {
public:
    PrimitiveType(std::string name);

    std::expected<std::reference_wrapper<Type>, std::string> resolve_method_call_ret(MethodCallExp& method_call) override;
    bool is_subtype_of(Type& other) override;
};

class ClassType : Type {
    std::expected<std::reference_wrapper<Type>, std::string> resolve_method_call_ret(MethodCallExp& method_call) override;
    bool is_subtype_of(Type& other) override;

    std::optional<std::reference_wrapper<Type>> parent;
    std::unordered_map<std::string, Type> fields;
};

class TypeMap {
public:
    TypeMap();
    TypeMap(const std::vector<std::shared_ptr<Type>>& types);

    std::optional<Type&> get_type(std::string_view);
private:
    std::unordered_map<std::string, Type> known_types;
};

class Scope {
public:
    Scope();
    Scope(Scope& parent);
    std::expected<void, std::string> define(std::string_view var_name, Type& type);
private:
    std::optional<std::reference_wrapper<Scope>> parent;
    std::unordered_map<std::string, Type> scope;
};

class MethodSignature {
public:
    MethodSignature(MethodDef& method_def);
private:
    std::string name;
    std::vector<Type> param_types;
    Type& ret_type;
};


class TypeChecker {
public:
    TypeChecker(Program& program);
    static std::expected<void, std::string> Typecheck(Program& program);
    std::expected<void, std::string> typecheck();
private:
    Program& program;
    TypeMap type_map;
    Scope scope;

    std::expected<void, std::string> check_class();
    std::expected<void, std::string> check_stmt(Stmt& stmt);
    std::expected<void, std::string> check_exp();

    std::expected<void, std::string> check_while_stmt(std::shared_ptr<WhileStmt> stmt);

    std::expected<void, std::string> check_guard(Expr& guard);

    std::expected<std::shared_ptr<Type>, std::string> resolve_exp_type(Expr& exp);
};

#endif
