#include "chava/class.hpp"
#include <chava/type.hpp>
#include <chava/stmt.hpp>
#include <chava/typechecker.hpp>
#include <expected>
#include <format>
#include <memory>
#include <variant>
#include <vector>

bool types_equal(const std::shared_ptr<Type> left, const std::shared_ptr<Type> right) {
    return *left == *right;
}

template<typename T>
std::string create_error(PositionWrapper<T> node, std::string_view message) {
    return create_error(node.pos, message);
}

std::string create_error(const Position& pos, std::string_view message) {
    return std::format("[{}:{}]: {}", pos.line, pos.col, message);
}

std::vector<std::shared_ptr<Type>> built_ins = {
    std::make_shared<PrimitiveType>(PrimitiveType("int")),
    std::make_shared<PrimitiveType>(PrimitiveType("bool")),
    std::make_shared<PrimitiveType>(PrimitiveType("void")),

    // TODO: add built in classes (Object, String)
};

const ClassDef ObjectClass = ClassDef{};
const ClassDef StringClass = ClassDef{};

const std::shared_ptr<ClassType> TypeChecker::bi_object = std::make_shared<ClassType>(ObjectClass);
const std::shared_ptr<ClassType> TypeChecker::bi_string = std::make_shared<ClassType>(StringClass);

const std::shared_ptr<PrimitiveType> TypeChecker::pr_int = std::make_shared<PrimitiveType>(PrimitiveType("int"));
const std::shared_ptr<PrimitiveType> TypeChecker::pr_bool = std::make_shared<PrimitiveType>(PrimitiveType("bool"));
const std::shared_ptr<PrimitiveType> TypeChecker::pr_void = std::make_shared<PrimitiveType>(PrimitiveType("void"));

PrimitiveType::PrimitiveType(std::string name) : Type(name) {}

std::string_view Type::get_name() {
    return name;
}

std::expected<std::shared_ptr<Type>, std::string> PrimitiveType::resolve_method_call_ret(MethodCallExp& method_call) {
    return std::unexpected("not implemented");
}

bool PrimitiveType::is_subtype_of(std::shared_ptr<Type> other) {
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

std::expected<void, std::string> TypeChecker::check_stmt(const Stmt& stmt) {
    auto res = std::visit(overloaded {
        [this](const ExpStmt& stmt) -> std::expected<void, std::string> { return check_exp(stmt.exp); },
        [this, stmt](const VardecStmt&) -> std::expected<void, std::string> { return check_vardec_stmt(stmt.to<VardecStmt>()); },
        [this, stmt](const AssignStmt&) -> std::expected<void, std::string> { return check_assign_stmt(stmt.to<AssignStmt>()); },
        [this, stmt](const std::shared_ptr<WhileStmt>&) -> std::expected<void, std::string> { return check_while_stmt(stmt.to<WhileStmt>()); },
        [this](const std::shared_ptr<BreakStmt>&) -> std::expected<void, std::string> { return {}; },
        [this, stmt](const std::shared_ptr<ReturnStmt>&) -> std::expected<void, std::string> { return check_return_stmt(stmt.to<ReturnStmt>()); },
        [this, stmt](const std::shared_ptr<IfStmt>&) -> std::expected<void, std::string> { return check_if_stmt(stmt.to<IfStmt>()); },
        [this, stmt](const std::shared_ptr<BlockStmt>&) -> std::expected<void, std::string> { return check_block_stmt(stmt.to<BlockStmt>()); },
    }, stmt.value);

    if(!res) {
        return std::unexpected(res.error());
    }

    return {};
}

std::expected<void, std::string> TypeChecker::check_exp(const Exp& exp) {
    auto type = resolve_exp_type(exp);
    if(!type) {
        return std::unexpected(type.error());
    }
    return {};
}

std::expected<void, std::string> TypeChecker::check_while_stmt(const PositionWrapper<WhileStmt>& stmt) {
    if(auto guard = check_guard(stmt.value.guard); !guard) {
        return std::unexpected(guard.error());
    }

    if(auto body_res = check_stmt(stmt.value.body); !body_res) {
        return std::unexpected(body_res.error());
    }

    return {};
}

std::expected<void, std::string> TypeChecker::check_vardec_stmt(const PositionWrapper<VardecStmt>& stmt) {
    return check_vardec(stmt.value.vardec);
}

std::expected<void, std::string> TypeChecker::check_vardec(const Vardec& vardec) {
    auto type = std::visit(overloaded {
        [this, vardec](ParsedPrimitiveType type) -> std::expected<std::shared_ptr<Type>, std::string> {
            switch(type) {
                case ParsedPrimitiveType::Int:
                case ParsedPrimitiveType::Bool:
                case ParsedPrimitiveType::Void:
                    if(auto t = type_map.get_type(to_string(type))) {
                        return t.value();
                    }
                    return std::unexpected(create_error(vardec.value.type, "Unhandled type in vardec"));
                default:
                    return std::unexpected(create_error(vardec.value.type, "Unhandled primitive type in vardec"));
            }
        },
        [this, vardec](ParsedClassType class_type) -> std::expected<std::shared_ptr<Type>, std::string> {
            return type_map.get_type(std::string(class_type.class_name))
                .transform(
                    [](std::shared_ptr<Type> t) -> std::expected<std::shared_ptr<Type>, std::string> 
                    {
                        return t;
                    })
                .value_or(std::unexpected(create_error(vardec.value.type, "Invalid type in vardec")));
        },
    }, vardec.value.type.value);

    if(!type) {
        return std::unexpected(type.error());
    }

    return scope.define(vardec);
}

std::expected<void, std::string> TypeChecker::check_assign_stmt(const PositionWrapper<AssignStmt>& stmt) {
    auto var_type = scope.get_var_type(stmt.value.var, stmt.pos);
    if(!var_type) {
        return std::unexpected(var_type.error());
    }

    auto val_type = resolve_exp_type(stmt.value.val);
    if(!val_type) {
        return std::unexpected(val_type.error());
    }

    if(!val_type.value()->is_subtype_of(var_type.value())) {
        return std::unexpected(create_error(stmt, std::format("Cannot assign value of type {} to variable of type {}",
                                            val_type.value()->get_name(),
                                            var_type.value()->get_name())));
    }

    return {};
}

std::expected<void, std::string> TypeChecker::check_if_stmt(const PositionWrapper<IfStmt>& stmt) {
    if(auto guard_res = check_guard(stmt.value.guard); !guard_res) {
        return std::unexpected(guard_res.error());
    }

    if(auto body_res = check_stmt(stmt.value.body); !body_res) {
        return std::unexpected(body_res.error());
    }

    if(stmt.value.else_body) {
        return check_stmt(stmt.value.else_body.value());
    }

    return {};
}

std::expected<void, std::string> TypeChecker::check_block_stmt(const PositionWrapper<BlockStmt>& stmt) {
    scope = Scope(scope);
    for(auto s : stmt.value.stmts) {
        if(auto stmt_res = check_stmt(s); !stmt_res) {
            return std::unexpected(stmt_res.error());
        }
    }

    return {};
}

std::expected<void, std::string> TypeChecker::check_return_stmt(const PositionWrapper<ReturnStmt>& stmt) {
    return std::unexpected("not implemented");
}

std::expected<void, std::string> TypeChecker::check_guard(const Exp& guard) {
    auto type = resolve_exp_type(guard);
    if(!type) {
        return std::unexpected(type.error());
    }

    if(auto bool_type = std::dynamic_pointer_cast<PrimitiveType>(type.value()); bool_type == nullptr) {
        return std::unexpected(create_error(guard, "Guard was not of type bool"));
    }

    return {};
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_exp_type(const Exp& exp) {
    return std::visit(overloaded {
        [this, exp](const VarExp&) -> std::expected<std::shared_ptr<Type>, std::string> { return resolve_var_exp(exp.to<VarExp>()); },
        [](const StrLitExp& exp) -> std::expected<std::shared_ptr<Type>, std::string> { return TypeChecker::bi_string; },
        [](const NumLitExp& exp) -> std::expected<std::shared_ptr<Type>, std::string> { return TypeChecker::pr_int; },
        [](const BoolLitExp& exp) -> std::expected<std::shared_ptr<Type>, std::string> { return TypeChecker::pr_bool; },
        [this, exp](const ThisExp&) -> std::expected<std::shared_ptr<Type>, std::string> { return resolve_this_exp(exp.to<ThisExp>()); },
        [this, exp](const std::shared_ptr<NewObjExp>&) -> std::expected<std::shared_ptr<Type>, std::string> { return resolve_new_obj_exp(exp.to<NewObjExp>()); },
        [this, exp](const std::shared_ptr<MethodCallExp>&) -> std::expected<std::shared_ptr<Type>, std::string> { return resolve_method_call_exp(exp.to<MethodCallExp>()); },
        [this, exp](const std::shared_ptr<BinaryExp>&) -> std::expected<std::shared_ptr<Type>, std::string> { return resolve_binary_exp(exp.to<BinaryExp>()); },
    }, exp.value);
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_var_exp(const PositionWrapper<VarExp>& exp) {
    return scope.get_var_type(exp.value.var, exp.pos);
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_this_exp(const PositionWrapper<ThisExp>& exp) {
    return scope.get_this(exp);
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_new_obj_exp(const PositionWrapper<NewObjExp>& exp) {
    auto obj_type = type_map.get_type(std::string(exp.value.class_name));
    if(!obj_type.has_value()) {
        return std::unexpected(create_error(exp, std::format("New object class {} doesn't exist", exp.value.class_name)));
    }

    // TODO: check constructor

    return std::unexpected("not implemented");
    // return obj_type.value();
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_method_call_exp(const PositionWrapper<MethodCallExp>& exp) {
    return std::unexpected("not implemented");
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_binary_exp(const PositionWrapper<BinaryExp>& exp) {
    auto left = resolve_exp_type(exp.value.left);
    if(!left) {
        return std::unexpected(left.error());
    }
    auto right = resolve_exp_type(exp.value.right);
    if(!right) {
        return std::unexpected(right.error());
    }

    switch(exp.value.op) {
        case Op::Add:
        case Op::Sub:
            return resolve_add_exp(left.value(), exp.value.op, right.value())
                .transform_error([exp](std::string err) { return create_error(exp, err); });
        case Op::Mult:
        case Op::Div:
            return resolve_mult_exp(left.value(), exp.value.op, right.value())
                .transform_error([exp](std::string err) { return create_error(exp, err); });
        case Op::Lt:
        case Op::Gt:
            return resolve_comp_exp(left.value(), exp.value.op, right.value())
                .transform_error([exp](std::string err) { return create_error(exp, err); });
        case Op::Eq:
        case Op::NotEq:
            return resolve_eq_exp(left.value(), exp.value.op, right.value())
                .transform_error([exp](std::string err) { return create_error(exp, err); });
        default:
            return std::unexpected(create_error(exp, "Unhandled operator"));
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
