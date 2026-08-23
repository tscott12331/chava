#include <chava/typechecker.hpp>
#include <expected>
#include <vector>

std::vector<Type> built_ins = {
    PrimitiveType("int"),
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
    type_map = TypeMap();
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
