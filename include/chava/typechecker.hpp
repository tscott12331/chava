#ifndef TYPECHECKER_HPP
#define TYPECHECKER_HPP

#include <chava/parser.hpp>
#include <expected>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <ranges>

class Type {
public:
    Type(std::string name) : name(name) {};

    std::string_view get_name();
    uint get_depth();
    virtual std::expected<std::shared_ptr<Type>, std::string> resolve_method_call_ret(MethodCallExp& method_call) = 0;
    virtual bool is_subtype_of(std::shared_ptr<Type> other) = 0;

    bool operator==(const Type& other) const {
        return name == other.name;
    }


    virtual ~Type() = default;

private:
    std::string name;
    uint depth = 0;
};

class TypeMap {
public:
    TypeMap();
    TypeMap(const std::vector<std::shared_ptr<Type>>& types);

    std::optional<std::shared_ptr<Type>> get_type(const std::string&);
private:
    std::unordered_map<std::string, std::shared_ptr<Type>> known_types;
};

bool types_equal(const std::shared_ptr<Type> left, const std::shared_ptr<Type> right);
bool is_int(std::shared_ptr<Type> type);
bool is_bool(std::shared_ptr<Type> type);
bool is_void(std::shared_ptr<Type> type);

std::string create_error(const Position& pos, std::string_view message);

template<typename T>
std::string create_error(const PositionWrapper<T>& node, std::string_view message) {
    return create_error(node.pos, message);
}


class TypeList {
public:
    TypeList();
    TypeList(const std::vector<std::shared_ptr<Type>>& types);
    bool can_assign_to(const TypeList& other) const;
    std::vector<std::shared_ptr<Type>> types() const;

    bool operator==(const TypeList& other) const {
        if(_types.size() != other._types.size()) return false;

        for(const auto [i, t] : std::views::enumerate(_types)) {
            const auto& other_t = other._types.at(i);
            if(t != other_t) {
                return false;
            }
        }
        return true;
    }
private:
    std::vector<std::shared_ptr<Type>> _types;
};

class MethodSignature {
public:
    bool has_same_params(const MethodSignature& other) const;
    bool can_override(const MethodSignature& other) const;

    static std::expected<MethodSignature, std::string> from(const MethodDefValue& method_def, TypeMap& type_map);

    const std::string& name() const;
    const TypeList& params() const;
    const std::shared_ptr<Type> ret_type() const;

    bool operator==(MethodSignature& other) const {
        // we don't consider return type for method signature equality
        // because we only care to match signatures (esp hashes) 
        // when their name/types match
        return _name == other.name() && _params == other.params();
    }
private:
    std::string _name;
    TypeList _params;
    std::shared_ptr<Type> _ret_type;
    MethodSignature(const std::string& name, const TypeList& params, std::shared_ptr<Type> ret_type);
};

class MethodSignatureHash {
    std::size_t operator()(const MethodSignature& ms) {
        const auto name_hash = std::hash<std::string>{}(ms.name());
        std::size_t param_hash = 0;
        for(const auto pt : ms.params().types()) {
            const auto pt_hash = std::hash<std::string_view>{}(pt->get_name());
            param_hash ^= (pt_hash << 1);
        }

        // we don't consider return type for hashing
        return name_hash ^ (param_hash << 1);
    }
};

class PrimitiveType : public Type {
public:
    PrimitiveType(std::string name);

    std::expected<std::shared_ptr<Type>, std::string> resolve_method_call_ret(MethodCallExp& method_call) override;
    bool is_subtype_of(std::shared_ptr<Type> other) override;
};

class ClassType : public Type {
    // TODO: implement constructor
public:
    ClassType(const ClassDef& classdef);
    std::expected<std::shared_ptr<Type>, std::string> resolve_method_call_ret(MethodCallExp& method_call) override;
    bool is_subtype_of(std::shared_ptr<Type> other) override;

    std::expected<void, std::string> populate(TypeMap& type_map);
private:
    std::expected<void, std::string> populate_fields(TypeMap& type_map);
    std::expected<void, std::string> populate_methods(TypeMap& type_map);

    std::expected<void, std::string> check_redeclaration(const MethodSignature& method_signature, const Position& pos);
    std::expected<void, std::string> check_override(const MethodSignature& method_signature, const Position& pos);

    bool has_field(const std::string& field_name);

    std::optional<std::shared_ptr<ClassType>> parent;
    std::unordered_map<std::string, std::shared_ptr<Type>> fields;
    std::unordered_map<std::string, std::unordered_set<MethodSignature, MethodSignatureHash>> methods;
    // std::unordered_map<std::string, MethodSignature> methods;

    const ClassDef& classdef;
    bool is_populated = false;
};

class Scope {
public:
    Scope();
    Scope(std::shared_ptr<Scope> parent);
    std::expected<void, std::string> define(const std::string& var_name, std::shared_ptr<Type> type, const Position& pos);
    std::expected<void, std::string> define(const Vardec& vardec, TypeMap& type_map);
    std::expected<std::shared_ptr<Type>, std::string> get_var_type(std::string& var_name, const Position& pos);
    std::expected<std::shared_ptr<Type>, std::string> get_var_type(std::string_view var_name, const Position &pos);

    std::expected<std::shared_ptr<Type>, std::string> get_this(const PositionWrapper<ThisExp>& exp);
private:
    std::optional<std::shared_ptr<Scope>> parent;
    std::unordered_map<std::string, std::shared_ptr<Type>> scope;
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
    std::expected<void, std::string> check_stmt(const PositionWrapper<StmtVariant>& stmt);
    std::expected<void, std::string> check_exp(const Exp& exp);

    std::expected<void, std::string> check_while_stmt(const PositionWrapper<std::shared_ptr<WhileStmt>>& stmt);
    std::expected<void, std::string> check_vardec_stmt(const PositionWrapper<VardecStmt>& stmt);
    std::expected<void, std::string> check_vardec(const Vardec& vardec);
    std::expected<void, std::string> check_assign_stmt(const PositionWrapper<AssignStmt>& stmt);
    std::expected<void, std::string> check_if_stmt(const PositionWrapper<std::shared_ptr<IfStmt>>& stmt);
    std::expected<void, std::string> check_block_stmt(const PositionWrapper<std::shared_ptr<BlockStmt>>& stmt);
    std::expected<void, std::string> check_return_stmt(const PositionWrapper<std::shared_ptr<ReturnStmt>>& stmt);

    std::expected<void, std::string> check_guard(const Exp& guard);

    std::expected<std::shared_ptr<Type>, std::string> resolve_exp_type(const Exp& exp);
    std::expected<std::shared_ptr<Type>, std::string> resolve_var_exp(const PositionWrapper<VarExp>& exp);
    std::expected<std::shared_ptr<Type>, std::string> resolve_this_exp(const PositionWrapper<ThisExp>& exp);
    std::expected<std::shared_ptr<Type>, std::string> resolve_new_obj_exp(const PositionWrapper<std::shared_ptr<NewObjExp>>& exp);
    std::expected<std::shared_ptr<Type>, std::string> resolve_method_call_exp(const PositionWrapper<std::shared_ptr<MethodCallExp>>& exp);
    std::expected<std::shared_ptr<Type>, std::string> resolve_binary_exp(const PositionWrapper<std::shared_ptr<BinaryExp>>& exp);
    std::expected<std::shared_ptr<Type>, std::string> resolve_add_exp(std::shared_ptr<Type> left, Op op, std::shared_ptr<Type> right);
    std::expected<std::shared_ptr<Type>, std::string> resolve_mult_exp(std::shared_ptr<Type> left, Op op, std::shared_ptr<Type> right);
    std::expected<std::shared_ptr<Type>, std::string> resolve_comp_exp(std::shared_ptr<Type> left, Op op, std::shared_ptr<Type> right);
    std::expected<std::shared_ptr<Type>, std::string> resolve_eq_exp(std::shared_ptr<Type> left, Op op, std::shared_ptr<Type> right);
};

#endif
