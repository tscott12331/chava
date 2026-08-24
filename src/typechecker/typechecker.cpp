#include "chava/stmt.hpp"
#include <chava/typechecker.hpp>
#include <expected>
#include <iostream>
#include <memory>
#include <variant>
#include <vector>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

std::vector<std::shared_ptr<Type>> built_ins = {
    std::make_shared<PrimitiveType>(PrimitiveType("int")),
    std::make_shared<PrimitiveType>(PrimitiveType("bool")),
    std::make_shared<PrimitiveType>(PrimitiveType("void")),

    // TODO: add built in classes (Object, String)
};

PrimitiveType::PrimitiveType(std::string name) : Type(name) {}

std::string_view Type::get_name() {
    return name;
}

std::expected<std::reference_wrapper<Type>, std::string> PrimitiveType::resolve_method_call_ret(MethodCallExp& method_call) {
    return std::unexpected("not implemented");
}

bool PrimitiveType::is_subtype_of(Type& other) {
    return other.get_name() == get_name();
}

std::expected<std::reference_wrapper<Type>, std::string> ClassType::resolve_method_call_ret(MethodCallExp& method_call) {
    return std::unexpected("not implemented");
}

bool ClassType::is_subtype_of(Type& other) {
    return other.get_name() == get_name();
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
