#ifndef TYPECHECKER_HPP
#define TYPECHECKER_HPP

#include "chava/stmt.hpp"
#include <chava/parser.hpp>
#include <expected>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class Type {
public:
    Type(std::string name) : name(name) {};

    std::string_view get_name();
    virtual std::expected<std::shared_ptr<Type>, std::string> resolve_method_call_ret(MethodCallExp& method_call) = 0;
    virtual bool is_subtype_of(std::shared_ptr<Type> other) = 0;

    bool operator==(const Type& other) const {
        return name == other.name;
    }


    virtual ~Type() = default;

private:
    std::string name;
};

bool types_equal(const std::shared_ptr<Type> left, const std::shared_ptr<Type> right);
bool is_int(std::shared_ptr<Type> type);
bool is_bool(std::shared_ptr<Type> type);
bool is_void(std::shared_ptr<Type> type);


class PrimitiveType : public Type {
public:
    PrimitiveType(std::string name);

    std::expected<std::shared_ptr<Type>, std::string> resolve_method_call_ret(MethodCallExp& method_call) override;
    bool is_subtype_of(std::shared_ptr<Type> other) override;
};

class ClassType : public Type {
    // TODO: implement constructor
public:
    ClassType(std::string name);
    std::expected<std::shared_ptr<Type>, std::string> resolve_method_call_ret(MethodCallExp& method_call) override;
    bool is_subtype_of(std::shared_ptr<Type> other) override;

private:
    std::optional<std::shared_ptr<Type>> parent;
    std::unordered_map<std::string, std::shared_ptr<Type>> fields;
};

class TypeMap {
public:
    TypeMap();
    TypeMap(const std::vector<std::shared_ptr<Type>>& types);

    std::optional<std::shared_ptr<Type>> get_type(std::string_view);
private:
    std::unordered_map<std::string, std::shared_ptr<Type>> known_types;
};

class Scope {
public:
    Scope();
    Scope(std::shared_ptr<Scope> parent);
    std::expected<void, std::string> define(std::string& var_name, std::shared_ptr<Type> type);
    std::expected<std::shared_ptr<Type>, std::string> get_var_type(std::string& var_name);
    std::expected<std::shared_ptr<Type>, std::string> get_var_type(std::string_view var_name);

    std::expected<std::shared_ptr<Type>, std::string> get_this();
private:
    std::optional<std::shared_ptr<Scope>> parent;
    std::unordered_map<std::string, std::shared_ptr<Type>> scope;
};

class MethodSignature {
public:
    MethodSignature(MethodDef& method_def);
private:
    std::string name;
    std::vector<std::shared_ptr<Type>> param_types;
    Type& ret_type;
};


class TypeChecker {
public:
    TypeChecker(Program& program);
    static std::expected<void, std::string> Typecheck(Program& program);
    std::expected<void, std::string> typecheck();

    static const std::shared_ptr<ClassType> bi_object;
    static const std::shared_ptr<ClassType> bi_string;

    static const std::shared_ptr<PrimitiveType> pr_int;
    static const std::shared_ptr<PrimitiveType> pr_bool;
    static const std::shared_ptr<PrimitiveType> pr_void;
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
    std::expected<std::shared_ptr<Type>, std::string> resolve_var_exp(VarExp& exp);
    std::expected<std::shared_ptr<Type>, std::string> resolve_this_exp();
    std::expected<std::shared_ptr<Type>, std::string> resolve_new_obj_exp(std::shared_ptr<NewObjExp>& exp);
    std::expected<std::shared_ptr<Type>, std::string> resolve_method_call_exp(std::shared_ptr<MethodCallExp>& exp);
    std::expected<std::shared_ptr<Type>, std::string> resolve_binary_exp(std::shared_ptr<BinaryExp>& exp);
    std::expected<std::shared_ptr<Type>, std::string> resolve_add_exp(std::shared_ptr<Type> left, Op op, std::shared_ptr<Type> right);
    std::expected<std::shared_ptr<Type>, std::string> resolve_mult_exp(std::shared_ptr<Type> left, Op op, std::shared_ptr<Type> right);
    std::expected<std::shared_ptr<Type>, std::string> resolve_comp_exp(std::shared_ptr<Type> left, Op op, std::shared_ptr<Type> right);
    std::expected<std::shared_ptr<Type>, std::string> resolve_eq_exp(std::shared_ptr<Type> left, Op op, std::shared_ptr<Type> right);
};

#endif
