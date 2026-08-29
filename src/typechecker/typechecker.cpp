#include "chava/type.hpp"
#include <chava/stmt.hpp>
#include <chava/typechecker.hpp>
#include <expected>
#include <format>
#include <memory>
#include <variant>
#include <vector>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

const std::string INT_NAME = "int";
const std::string BOOl_NAME = "bool";
const std::string VOID_NAME = "void";

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
        [this](ExpStmt& stmt) -> std::expected<void, std::string> { return check_exp(stmt.exp); },
        [this](VardecStmt& stmt) -> std::expected<void, std::string> { return check_vardec_stmt(stmt); },
        [this](AssignStmt& stmt) -> std::expected<void, std::string> { return check_assign_stmt(stmt); },
        [this](std::shared_ptr<WhileStmt>& stmt) -> std::expected<void, std::string> { return check_while_stmt(stmt); },
        [this](std::shared_ptr<BreakStmt>& stmt) -> std::expected<void, std::string> { return {}; },
        [this](std::shared_ptr<ReturnStmt>& stmt) -> std::expected<void, std::string> { return check_return_stmt(stmt); },
        [this](std::shared_ptr<IfStmt>& stmt) -> std::expected<void, std::string> { return check_if_stmt(stmt); },
        [this](std::shared_ptr<BlockStmt>& stmt) -> std::expected<void, std::string> { return check_block_stmt(stmt); },
    }, stmt);

    if(!res) {
        return std::unexpected(res.error());
    }

    return {};
}

std::expected<void, std::string> TypeChecker::check_exp(Expr& exp) {
    auto type = resolve_exp_type(exp);
    if(!type) {
        return std::unexpected(type.error());
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

std::expected<void, std::string> TypeChecker::check_vardec_stmt(VardecStmt& stmt) {
    auto type = std::visit(overloaded {
        [this](ParsedPrimitiveType type) -> std::expected<std::shared_ptr<Type>, std::string> {
            switch(type) {
                case ParsedPrimitiveType::Int:
                    if(auto t = type_map.get_type(INT_NAME)) {
                        return t.value();
                    }
                    return std::unexpected("Invalid type in vardec");
                case ParsedPrimitiveType::Bool:
                    if(auto t = type_map.get_type(BOOl_NAME)) {
                        return t.value();
                    }
                    return std::unexpected("Invalid type in vardec");
                case ParsedPrimitiveType::Void:
                    if(auto t = type_map.get_type(VOID_NAME)) {
                        return t.value();
                    }
                    return std::unexpected("Invalid type in vardec");
                default:
                    return std::unexpected("Unhandled primitive type in vardec");
            }
        },
        [this](ParsedClassType class_type) -> std::expected<std::shared_ptr<Type>, std::string> {
            return type_map.get_type(std::string(class_type.class_name))
                .transform(
                    [](std::shared_ptr<Type> t) -> std::expected<std::shared_ptr<Type>, std::string> 
                    {
                        return t;
                    })
                .value_or(std::unexpected("Invalid type in vardec"));
        },
    }, stmt.vardec.type);

    if(!type) {
        return std::unexpected(type.error());
    }

    auto var_name = std::string(stmt.vardec.var);
    return scope.define(var_name, type.value());
}

std::expected<void, std::string> TypeChecker::check_assign_stmt(AssignStmt& stmt) {
    auto var_type = scope.get_var_type(stmt.var);
    if(!var_type) {
        return std::unexpected(var_type.error());
    }

    auto val_type = resolve_exp_type(stmt.val);
    if(!val_type) {
        return std::unexpected(val_type.error());
    }

    if(!val_type.value()->is_subtype_of(var_type.value())) {
        return std::unexpected(std::format("Cannot assign value of type {} to variable of type {}",
                                            val_type.value()->get_name(),
                                            var_type.value()->get_name()));
    }

    return {};
}

std::expected<void, std::string> TypeChecker::check_if_stmt(std::shared_ptr<IfStmt> stmt) {
    if(auto guard_res = check_guard(stmt->guard); !guard_res) {
        return std::unexpected(guard_res.error());
    }

    if(auto body_res = check_stmt(stmt->body); !body_res) {
        return std::unexpected(body_res.error());
    }

    if(stmt->else_body) {
        return check_stmt(stmt->else_body.value());
    }

    return {};
}

std::expected<void, std::string> TypeChecker::check_block_stmt(std::shared_ptr<BlockStmt> stmt) {
    scope = Scope(scope);
    for(auto s : stmt->stmts) {
        if(auto stmt_res = check_stmt(s); !stmt_res) {
            return std::unexpected(stmt_res.error());
        }
    }

    return {};
}

std::expected<void, std::string> TypeChecker::check_return_stmt(std::shared_ptr<ReturnStmt> stmt) {
    return std::unexpected("not implemented");
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
        [](StrLitExp& exp) -> std::expected<std::shared_ptr<Type>, std::string> { return TypeChecker::bi_string; },
        [](NumLitExp& exp) -> std::expected<std::shared_ptr<Type>, std::string> { return TypeChecker::pr_int; },
        [](BoolLitExp& exp) -> std::expected<std::shared_ptr<Type>, std::string> { return TypeChecker::pr_bool; },
        [this](ThisExp& exp) -> std::expected<std::shared_ptr<Type>, std::string> { return resolve_this_exp(); },
        [this](std::shared_ptr<NewObjExp>& exp) -> std::expected<std::shared_ptr<Type>, std::string> { return resolve_new_obj_exp(exp); },
        [this](std::shared_ptr<MethodCallExp>& exp) -> std::expected<std::shared_ptr<Type>, std::string> { return resolve_method_call_exp(exp); },
        [this](std::shared_ptr<BinaryExp>& exp) -> std::expected<std::shared_ptr<Type>, std::string> { return resolve_binary_exp(exp); },
    }, exp);
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_var_exp(VarExp& exp) {
    return scope.get_var_type(exp.var);
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_this_exp() {
    return scope.get_this();
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_new_obj_exp(std::shared_ptr<NewObjExp>& exp) {
    auto obj_type = type_map.get_type(std::string(exp->class_name));
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
    if(op != Op::Add) {
        if(types_equal(left, TypeChecker::pr_int) && types_equal(right, TypeChecker::pr_int)) {
            return TypeChecker::pr_int;
        }

        return std::unexpected(std::format("Can't subtract {} from {}", left->get_name(), right->get_name()));
    }

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

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_mult_exp(std::shared_ptr<Type> left, Op op, std::shared_ptr<Type> right) {
    if(!types_equal(left, TypeChecker::pr_int) || !types_equal(right, TypeChecker::pr_int)) {
        return std::unexpected(std::format("Can't mult/divide types {} and {}", left->get_name(), right->get_name()));
    }

    return TypeChecker::pr_int;
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_comp_exp(std::shared_ptr<Type> left, Op op, std::shared_ptr<Type> right) {
    if(!types_equal(left, TypeChecker::pr_int) || !types_equal(right, TypeChecker::pr_int)) {
        return std::unexpected(std::format("Can't mult/divide types {} and {}", left->get_name(), right->get_name()));
    }

    return TypeChecker::pr_bool;
}
std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_eq_exp(std::shared_ptr<Type> left, Op op, std::shared_ptr<Type> right) {
    if(!left->is_subtype_of(right) && !right->is_subtype_of(left)) {
        return std::unexpected(std::format("Cannot check equality of non related types {} and {}", left->get_name(), right->get_name()));
    }

    return TypeChecker::pr_bool;
}
