#include <chava/typechecker.hpp>
#include <expected>
#include <format>
#include <memory>
#include <stdexcept>
#include <variant>
#include <vector>

bool types_equal(const std::shared_ptr<Type> left, const std::shared_ptr<Type> right) {
    return *left == *right;
}

std::string create_error(const Position& pos, std::string_view message) {
    return std::format("[{}:{}]: {}", pos.line, pos.col, message);
}

const ClassDef ObjectClass = ClassDef{.value=ClassDefValue{.class_name="Object"}};
const ClassDef StringClass = ClassDef{.value=ClassDefValue{.class_name="String"}};

const std::shared_ptr<ClassType> TypeChecker::bi_object = std::make_shared<ClassType>(ObjectClass);
const std::shared_ptr<ClassType> TypeChecker::bi_string = std::make_shared<ClassType>(StringClass);

const std::shared_ptr<PrimitiveType> TypeChecker::pr_int = std::make_shared<PrimitiveType>(PrimitiveType(INT_NAME));
const std::shared_ptr<PrimitiveType> TypeChecker::pr_bool = std::make_shared<PrimitiveType>(PrimitiveType(BOOL_NAME));
const std::shared_ptr<PrimitiveType> TypeChecker::pr_void = std::make_shared<PrimitiveType>(PrimitiveType(VOID_NAME));

std::vector<std::shared_ptr<Type>> built_ins = {
    TypeChecker::pr_int,
    TypeChecker::pr_bool,
    TypeChecker::pr_void,

    TypeChecker::bi_string,
    TypeChecker::bi_object,
};

PrimitiveType::PrimitiveType(std::string name) : Type(name) {}

const std::string& Type::get_name() const {
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
    scope = std::make_shared<Scope>(Scope());
}

std::expected<void, std::string> TypeChecker::Typecheck(Program &program) {
    auto typechecker = TypeChecker(program);
    return typechecker.typecheck();
}

std::expected<void, std::string> TypeChecker::typecheck() {
    // TODO: class typechecking contract
    std::unordered_map<std::string, ClassDef> known_classes;
    for(const auto& classdef : program.classdefs) {
        const auto class_name = std::string(classdef.value.class_name);
        if(known_classes.contains(class_name)) {
            return std::unexpected(create_error(classdef, std::format("Redefined class {}", class_name)));
        }
        known_classes[class_name] = classdef;
    }

    if(const auto res = initialize_typemap(known_classes); !res) {
        return res;
    }

    for(const auto [_, t] : type_map.types()) {
        if(const auto ct = std::dynamic_pointer_cast<ClassType>(t); ct != nullptr) {
            if(const auto pop_res = ct->populate(type_map); !pop_res) {
                return pop_res;
            }
        }
    }

    for(const auto& classdef : program.classdefs) {
        if(auto res = check_class(classdef); !res) {
            return std::unexpected(res.error());
        }
    }

    for(const auto& stmt : program.stmts) {
        // go?
        if(auto res = check_stmt(stmt); !res) {
            return std::unexpected(res.error());
        }
    }

    return {};
}

std::expected<void, std::string> TypeChecker::initialize_typemap(std::unordered_map<std::string, ClassDef>& known_classes) {
    for(const auto [_, cd] : known_classes) {
        std::unordered_set<std::string> created_types;
        const auto class_name = std::string(cd.value.class_name);
        const auto define_res = define_type(class_name, known_classes, created_types);
        if(!define_res) {
            return std::unexpected(define_res.error());
        }
    }
    
    return {};
}

std::expected<std::shared_ptr<ClassType>, std::string> TypeChecker::define_type(const std::string& class_name, std::unordered_map<std::string, ClassDef>& known_classes, std::unordered_set<std::string>& created_types) {
    const auto cdi = known_classes.find(class_name);
    if(cdi == known_classes.end()) {
        return std::unexpected(std::format("Attempting to defined unknown class {}", class_name));
    }

    const auto& cd = cdi->second;
    if(created_types.contains(class_name)) {
        return std::unexpected(create_error(cd, "Cyclical inheritance detected"));
    }

    if(const auto& bi_res = type_map.get_built_in(class_name); bi_res) {
        return std::unexpected(create_error(cd, std::format("Attempting to redefine built-in type {}", bi_res.value()->get_name())));
    }

    if(const auto& df_res = type_map.get_type(class_name); df_res) {
        if(const auto df_ct = std::dynamic_pointer_cast<ClassType>(df_res.value()); df_ct != nullptr) {
            return df_ct;
        }
        return std::unexpected(create_error(cd, std::format("Attempting to redefine primitive type {}", df_res.value()->get_name())));
    }

    created_types.insert(class_name);
    
    std::optional<std::shared_ptr<ClassType>> parent = std::nullopt;
    if(cd.value.extend_class_name) {
        const auto extend_name = std::string(cd.value.extend_class_name.value());
        const auto existing = type_map.get_type(extend_name);

        if(existing) {
            const auto existing_class = std::dynamic_pointer_cast<ClassType>(existing.value());
            if(existing_class == nullptr) {
                return std::unexpected(create_error(cd, std::format("Cannot extend class with primitive type {}", existing.value()->get_name())));
            }
            parent = existing_class;
        } else if(const auto parent_res = define_type(extend_name, known_classes, created_types); !parent_res) {
            return parent_res;
        } else {
            parent = parent_res.value();
        }
    }

    // ClassType(const ClassDef& classdef, std::optional<std::shared_ptr<Type>> parent);
    const auto type = ClassType(cd, parent);
    const auto shared_type = std::make_shared<ClassType>(type);
    if(const auto define_res = type_map.define(shared_type); !define_res) {
        return std::unexpected(define_res.error());
    }

    return shared_type;
}

std::expected<void, std::string> TypeChecker::check_class(const ClassDef& classdef) {
    const auto class_name = std::string(classdef.value.class_name);
    const auto type = type_map.get_type(std::string(class_name));
    if(!type) {
        return std::unexpected(create_error(classdef, std::format("{} is not a valid type", class_name)));
    }
    const auto class_type = std::dynamic_pointer_cast<ClassType>(type.value());
    if(class_type == nullptr) {
        return std::unexpected(create_error(classdef, "Cannot create a class out of a primitive type"));
    }

    enter_scope();
    for(const auto& vds : classdef.value.vardecs) {
        if(const auto res = scope->define(vds.value.vardec, type_map); !res) {
            return res;
        }
    }

    if(const auto& res = check_constructor(classdef.value.constructor, class_type); !res) {
        return res;
    }

    for(const auto& mn : classdef.value.method_defs) {
        if(const auto& res = check_method(mn, class_type); !res) {
            return res;
        }
    }

    exit_scope();
    return {};
}

std::expected<void, std::string> TypeChecker::check_constructor(const Constructor& constructor, std::shared_ptr<ClassType> type) {
    enter_scope();
    if(const auto res = add_params_to_scope(constructor.value.params); !res) {
        return res;
    }

    if(constructor.value.super_args) {
        const auto super_type_list = args_to_type_list(constructor.value.super_args.value());
        if(!super_type_list) {
            return std::unexpected(super_type_list.error());
        }
        if(const auto& res = type->check_super_args(super_type_list.value(), constructor.value.super_args->pos); !res) {
            return res;
        }
    }

    for(const auto& stmt : constructor.value.stmts) {
        if(const auto& res = check_stmt(stmt); !res) {
            return res;
        }
    }

    exit_scope();
    return {};
}

// TODO: need return path checking
std::expected<void, std::string> TypeChecker::check_method(const MethodDef& method_def, std::shared_ptr<ClassType> type) {
    const auto ret_type_name = to_string(method_def.value.ret_type);
    const auto ret_type = type_map.get_type(ret_type_name);
    if(!ret_type) {
        return std::unexpected(create_error(method_def.value.ret_type, std::format("Unknown return type {}", ret_type_name)));
    }

    enter_scope(ret_type);
    
    if(const auto& res = add_params_to_scope(method_def.value.params); !res) {
        return res;
    }


    if(const auto& res = check_block_stmt(method_def.value.body); !res) {
        return res;
    }

    exit_scope();

    return {};
}

std::expected<void, std::string> TypeChecker::add_params_to_scope(const CommaVardec& params) {
    for(const auto& vd : params.value.vardecs) {
        if(const auto res = scope->define(vd, type_map); !res) {
            return res;
        }
    }

    return {};
}

std::expected<TypeList, std::string> TypeChecker::args_to_type_list(const CommaExp& args) {
    // const auto tl = TypeList
    std::vector<std::shared_ptr<Type>> types;
    for(const auto& exp : args.value.exps) {
        const auto type = resolve_exp_type(exp);
        if(!type) {
            return std::unexpected(type.error());
        }

        types.push_back(type.value());
    }

    return TypeList(types);
}



std::expected<void, std::string> TypeChecker::check_stmt(const Stmt& stmt) {
    auto res = std::visit(overloaded {
        [this](const ExpStmt& stmt) -> std::expected<void, std::string> { return check_exp(stmt.exp); },
        [this, stmt](const VardecStmt&) -> std::expected<void, std::string> { return check_vardec_stmt(stmt.to<VardecStmt>()); },
        [this, stmt](const AssignStmt&) -> std::expected<void, std::string> { return check_assign_stmt(stmt.to<AssignStmt>()); },
        [this, stmt](const std::shared_ptr<WhileStmt>&) -> std::expected<void, std::string> { return check_while_stmt(stmt.to<std::shared_ptr<WhileStmt>>()); },
        [this, stmt](const std::shared_ptr<BreakStmt>&) -> std::expected<void, std::string> { return check_break_stmt(stmt.to<std::shared_ptr<BreakStmt>>()); },
        [this, stmt](const std::shared_ptr<ReturnStmt>&) -> std::expected<void, std::string> { return check_return_stmt(stmt.to<std::shared_ptr<ReturnStmt>>()); },
        [this, stmt](const std::shared_ptr<IfStmt>&) -> std::expected<void, std::string> { return check_if_stmt(stmt.to<std::shared_ptr<IfStmt>>()); },
        [this, stmt](const std::shared_ptr<BlockStmt>&) -> std::expected<void, std::string> { return check_block_stmt(stmt.to<std::shared_ptr<BlockStmt>>()); },
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

std::expected<void, std::string> TypeChecker::check_while_stmt(const PositionWrapper<std::shared_ptr<WhileStmt>>& stmt) {
    if(auto guard = check_guard(stmt.value->guard); !guard) {
        return std::unexpected(guard.error());
    }

    if(auto body_res = check_stmt(stmt.value->body); !body_res) {
        return std::unexpected(body_res.error());
    }

    return {};
}

std::expected<void, std::string> TypeChecker::check_vardec_stmt(const PositionWrapper<VardecStmt>& stmt) {
    return check_vardec(stmt.value.vardec);
}

std::expected<void, std::string> TypeChecker::check_vardec(const Vardec& vardec) {
    auto type_name = to_string(vardec.value.type);
    auto t = type_map.get_type(type_name);
    if(!t) {
        return std::unexpected(create_error(vardec, std::format("Unknown type {} in vardec", type_name)));
    }
    if(types_equal(t.value(), TypeChecker::pr_void)) {
        return std::unexpected(create_error(vardec, std::format("Cannot declare a variable of type {}", TypeChecker::pr_void->get_name())));
    }

    return scope->define(vardec, type_map);
}

std::expected<void, std::string> TypeChecker::check_assign_stmt(const PositionWrapper<AssignStmt>& stmt) {
    auto var_type = scope->get_var_type(stmt.value.var, stmt.pos);
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

std::expected<void, std::string> TypeChecker::check_if_stmt(const PositionWrapper<std::shared_ptr<IfStmt>>& stmt) {
    if(auto guard_res = check_guard(stmt.value->guard); !guard_res) {
        return std::unexpected(guard_res.error());
    }

    if(auto body_res = check_stmt(stmt.value->body); !body_res) {
        return std::unexpected(body_res.error());
    }

    if(stmt.value->else_body) {
        return check_stmt(stmt.value->else_body.value());
    }

    return {};
}

std::expected<void, std::string> TypeChecker::check_block_stmt(const PositionWrapper<std::shared_ptr<BlockStmt>>& stmt) {
    enter_scope();
    for(const auto& s : stmt.value->stmts) {
        if(auto stmt_res = check_stmt(s); !stmt_res) {
            return std::unexpected(stmt_res.error());
        }
    }
    exit_scope();

    return {};
}

std::expected<void, std::string> TypeChecker::check_return_stmt(const PositionWrapper<std::shared_ptr<ReturnStmt>>& stmt) {
    if(!stmt.value->val) {
        if(scope->ret_type()) {
            return std::unexpected(create_error(stmt, std::format("Missing return value for method return type {}", scope->ret_type().value()->get_name())));
        }
        return {};
    }

    const auto ret_val = resolve_exp_type(stmt.value->val.value());
    if(!ret_val) {
        return std::unexpected(ret_val.error());
    }
    if(!scope->ret_type()) {
        return std::unexpected(create_error(stmt.value->val.value(), "Cannot return type on void method"));
    }

    if(!ret_val.value()->is_subtype_of(scope->ret_type().value())) {
        return std::unexpected(create_error(stmt.value->val.value(), 
                                std::format("Return value {} is not assignable to return type {}",
                                            ret_val.value()->get_name(),
                                            scope->ret_type().value()->get_name())));
    }

    return {};
}

std::expected<void, std::string> TypeChecker::check_break_stmt(const PositionWrapper<std::shared_ptr<BreakStmt>>& stmt) {
    if(!scope->is_while()) {
        return std::unexpected(create_error(stmt, "Cannot break outside of loop context"));
    }
    return {};
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
        [this, exp](const std::shared_ptr<NewObjExp>&) -> std::expected<std::shared_ptr<Type>, std::string> { return resolve_new_obj_exp(exp.to<std::shared_ptr<NewObjExp>>()); },
        [this, exp](const std::shared_ptr<MethodCallExp>&) -> std::expected<std::shared_ptr<Type>, std::string> { return resolve_method_call_exp(exp.to<std::shared_ptr<MethodCallExp>>()); },
        [this, exp](const std::shared_ptr<BinaryExp>&) -> std::expected<std::shared_ptr<Type>, std::string> { return resolve_binary_exp(exp.to<std::shared_ptr<BinaryExp>>()); },
    }, exp.value);
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_var_exp(const PositionWrapper<VarExp>& exp) {
    return scope->get_var_type(exp.value.var, exp.pos);
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_this_exp(const PositionWrapper<ThisExp>& exp) {
    return scope->get_this(exp);
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_new_obj_exp(const PositionWrapper<std::shared_ptr<NewObjExp>>& exp) {
    auto obj_type = type_map.get_type(std::string(exp.value->class_name));
    if(!obj_type.has_value()) {
        return std::unexpected(create_error(exp, std::format("New object class {} doesn't exist", exp.value->class_name)));
    }

    const auto class_type = std::dynamic_pointer_cast<ClassType>(obj_type.value());
    if(class_type == nullptr) {
        return std::unexpected(create_error(exp, std::format("Cannot instantiate object of non class type {}", obj_type.value()->get_name())));
    }

    // TODO: check constructor
    const auto args = args_to_type_list(exp.value->args);
    if(const auto& res = class_type->check_constructor_args(args.value(), exp.pos); !res) {
        return std::unexpected(res.error());
    }

    return class_type;
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_method_call_exp(const PositionWrapper<std::shared_ptr<MethodCallExp>>& exp) {
    return std::unexpected("not implemented");
}

std::expected<std::shared_ptr<Type>, std::string> TypeChecker::resolve_binary_exp(const PositionWrapper<std::shared_ptr<BinaryExp>>& exp) {
    auto left = resolve_exp_type(exp.value->left);
    if(!left) {
        return std::unexpected(left.error());
    }
    auto right = resolve_exp_type(exp.value->right);
    if(!right) {
        return std::unexpected(right.error());
    }

    switch(exp.value->op) {
        case Op::Add:
        case Op::Sub:
            return resolve_add_exp(left.value(), exp.value->op, right.value())
                .transform_error([exp](std::string err) { return create_error(exp, err); });
        case Op::Mult:
        case Op::Div:
            return resolve_mult_exp(left.value(), exp.value->op, right.value())
                .transform_error([exp](std::string err) { return create_error(exp, err); });
        case Op::Lt:
        case Op::Gt:
            return resolve_comp_exp(left.value(), exp.value->op, right.value())
                .transform_error([exp](std::string err) { return create_error(exp, err); });
        case Op::Eq:
        case Op::NotEq:
            return resolve_eq_exp(left.value(), exp.value->op, right.value())
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

void TypeChecker::enter_scope(std::optional<std::shared_ptr<Type>> ret_type, bool is_while) {
    scope = std::make_shared<Scope>(Scope(scope));
}

void TypeChecker::exit_scope() {
    if(!scope->parent) {
        throw std::logic_error("Exiting from no scope");
    }

    scope = scope->parent.value();
}
