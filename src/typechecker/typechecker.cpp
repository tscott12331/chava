#include <chava/stmt.hpp>
#include <chava/typechecker.hpp>
#include <expected>
#include <format>
#include <iostream>
#include <memory>
#include <variant>
#include <vector>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

bool types_equal(const std::shared_ptr<Type> left, const std::shared_ptr<Type> right) {
    return *left == *right;
}

std::vector<std::shared_ptr<Type>> built_ins = {
    std::make_shared<PrimitiveType>(PrimitiveType("int")),
    std::make_shared<PrimitiveType>(PrimitiveType("bool")),
    std::make_shared<PrimitiveType>(PrimitiveType("void")),

    // TODO: add built in classes (Object, String)
};

const std::shared_ptr<ClassType> TypeChecker::bi_object = std::make_shared<ClassType>(ClassType("Object"));
const std::shared_ptr<ClassType> TypeChecker::bi_string = std::make_shared<ClassType>(ClassType("String"));

const std::shared_ptr<PrimitiveType> TypeChecker::pr_int = std::make_shared<PrimitiveType>(PrimitiveType("int"));
const std::shared_ptr<PrimitiveType> TypeChecker::pr_bool = std::make_shared<PrimitiveType>(PrimitiveType("bool"));
const std::shared_ptr<PrimitiveType> TypeChecker::pr_void = std::make_shared<PrimitiveType>(PrimitiveType("void"));

PrimitiveType::PrimitiveType(std::string name) : Type(name) {}
ClassType::ClassType(std::string name) : Type(name) {}

std::string_view Type::get_name() {
    return name;
}

std::expected<std::shared_ptr<Type>, std::string> PrimitiveType::resolve_method_call_ret(MethodCallExp& method_call) {
    return std::unexpected("not implemented");
}

bool PrimitiveType::is_subtype_of(std::shared_ptr<Type> other) {
    return other->get_name() == get_name();
}

std::expected<std::shared_ptr<Type>, std::string> ClassType::resolve_method_call_ret(MethodCallExp& method_call) {
    return std::unexpected("not implemented");
}

bool ClassType::is_subtype_of(std::shared_ptr<Type> other) {
    return other->get_name() == get_name();
}

TypeChecker::TypeChecker(Program& program) : program(program) {
    type_map = TypeMap(built_ins);
    scope = Scope();
}

std::expected<void, std::string> TypeChecker::Typecheck(Program &program) {
    auto typechecker = TypeChecker(program);
    return typechecker.typecheck();
}

std::expected<void, std::string> TypeChecker::typecheck() {
    // TODO: class typechecking contract

    for(auto &stmt : program.stmts) {
        // go?
        if(auto res = check_stmt(stmt); !res) {
            return std::unexpected(res.error());
        }
    }

    return {};
}

std::expected<void, std::string> TypeChecker::check_stmt(Stmt& stmt) {
    auto res = std::visit(overloaded {
        [this](ExpStmt& stmt) -> std::expected<void, std::string> {},
        [this](VardecStmt& stmt) -> std::expected<void, std::string> {},
        [this](AssignStmt& stmt) -> std::expected<void, std::string> {},
        [this](std::shared_ptr<WhileStmt>& stmt) -> std::expected<void, std::string> { return check_while_stmt(stmt); },
        [this](std::shared_ptr<BreakStmt>& stmt) -> std::expected<void, std::string> {},
        [this](std::shared_ptr<ReturnStmt>& stmt) -> std::expected<void, std::string> {},
        [this](std::shared_ptr<IfStmt>& stmt) -> std::expected<void, std::string> {},
        [this](std::shared_ptr<BlockStmt>& stmt) -> std::expected<void, std::string> { std::cout << stmt->stmts.size(); },
    }, stmt);

    if(!res) {
        return std::unexpected(res.error());
    }

    return {};
}

std::expected<void, std::string> TypeChecker::check_while_stmt(std::shared_ptr<WhileStmt> stmt) {
    if(auto guard = check_guard(stmt->guard); !guard) {
        return std::unexpected(guard.error());
    }

    if(auto body_res = check_stmt(stmt->body); !body_res) {
        return std::unexpected(body_res.error());
    }

    return {};
}

std::expected<void, std::string> TypeChecker::check_guard(Expr& guard) {
    auto type = resolve_exp_type(guard);
    if(!type) {
        return std::unexpected(type.error());
    }

    if(auto bool_type = std::dynamic_pointer_cast<PrimitiveType>(type.value()); bool_type == nullptr) {
        return std::unexpected("Guard was not of type bool");
    }

    return {};
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_exp_type(Expr& exp) {
    return std::visit(overloaded {
        [this](VarExp& exp) -> std::expected<std::shared_ptr<Type>, std::string> { return resolve_var_exp(exp); },
        [this](StrLitExp& exp) -> std::expected<std::shared_ptr<Type>, std::string> { return TypeChecker::bi_string; },
        [this](NumLitExp& exp) -> std::expected<std::shared_ptr<Type>, std::string> { return TypeChecker::pr_int; },
        [this](BoolLitExp& exp) -> std::expected<std::shared_ptr<Type>, std::string> { return TypeChecker::pr_bool; },
        [this](ThisExp& exp) -> std::expected<std::shared_ptr<Type>, std::string> { return resolve_this_exp(); },
        [this](std::shared_ptr<NewObjExp>& exp) -> std::expected<std::shared_ptr<Type>, std::string> { return resolve_new_obj_exp(exp); },
        [this](std::shared_ptr<MethodCallExp>& exp) -> std::expected<std::shared_ptr<Type>, std::string> {},
        [this](std::shared_ptr<BinaryExp>& exp) -> std::expected<std::shared_ptr<Type>, std::string> {},
    }, exp);
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_var_exp(VarExp& exp) {
    return scope.get_var_type(exp.var);
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_this_exp() {
    return scope.get_this();
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_new_obj_exp(std::shared_ptr<NewObjExp>& exp) {
    auto obj_type = type_map.get_type(exp->class_name);
    if(!obj_type.has_value()) {
        return std::unexpected(std::format("New object class {} doesn't exist", exp->class_name));
    }

    // TODO: check constructor

    return std::unexpected("not implemented");
    // return obj_type.value();
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_method_call_exp(std::shared_ptr<MethodCallExp>& exp) {
    return std::unexpected("not implemented");
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_binary_exp(std::shared_ptr<BinaryExp>& exp) {
    auto left = resolve_exp_type(exp->left);
    if(!left) {
        return std::unexpected(left.error());
    }
    auto right = resolve_exp_type(exp->right);
    if(!right) {
        return std::unexpected(right.error());
    }

    switch(exp->op) {
        case Op::Add:
        case Op::Sub:
            return resolve_add_exp(left.value(), exp->op, right.value());
        case Op::Mult:
        case Op::Div:
            return resolve_mult_exp(left.value(), exp->op, right.value());
        case Op::Lt:
        case Op::Gt:
            return resolve_comp_exp(left.value(), exp->op, right.value());
        case Op::Eq:
        case Op::NotEq:
            return resolve_eq_exp(left.value(), exp->op, right.value());
        default:
            return std::unexpected("Unhandled operator");
    }
}

bool check_str_concat(std::shared_ptr<Type> left, std::shared_ptr<Type> right) {
    if(types_equal(left, TypeChecker::bi_string)) {
        if(types_equal(right, TypeChecker::bi_string) || types_equal(right, TypeChecker::pr_int)) {
            return true;
        }

        return false;
    }
    if(types_equal(left, TypeChecker::pr_int) && types_equal(right, TypeChecker::bi_string)) {
        return true;
    }

    return false;
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_add_exp(std::shared_ptr<Type> left, Op op, std::shared_ptr<Type> right) {
    if(check_str_concat(left, right)) {
        return TypeChecker::bi_string;
    }
    if(check_str_concat(right, left)) {
        return TypeChecker::bi_string;
    }
    if(types_equal(left, TypeChecker::pr_int) && types_equal(right, TypeChecker::pr_int)) {
        return TypeChecker::pr_int;
    }

    return std::unexpected(std::format("Can't add {} to {}", left->get_name(), right->get_name()));
}
std::expected<std::shared_ptr<Type>, std::string> resolve_mult_exp(std::shared_ptr<Type> left, Op op, std::shared_ptr<Type> right);
std::expected<std::shared_ptr<Type>, std::string> resolve_comp_exp(std::shared_ptr<Type> left, Op op, std::shared_ptr<Type> right);
std::expected<std::shared_ptr<Type>, std::string> resolve_eq_exp(std::shared_ptr<Type> left, Op op, std::shared_ptr<Type> right);
