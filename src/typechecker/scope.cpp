#include <chava/typechecker.hpp>
#include <format>
#include <memory>
Scope::Scope() {}
Scope::Scope(std::shared_ptr<Scope> parent) : parent(parent) {}

#define THIS "this"

std::expected<void, std::string> Scope::define(std::string& var_name, std::shared_ptr<Type> type) {
    if(scope.contains(var_name)) {
        return std::unexpected(std::format("{} is already defined in this local scope", var_name));
    }

    scope[var_name] = type;

    return {}; 
}

std::expected<std::shared_ptr<Type>, std::string> Scope::get_var_type(std::string& var_name) {
    if(!scope.contains(var_name)) {
        if(parent.has_value()) {
            return parent.value()->get_var_type(var_name);
        }
        return std::unexpected(std::format("{} is not defined", var_name));
    }

    return scope[var_name];
}

std::expected<std::shared_ptr<Type>, std::string> Scope::get_var_type(std::string_view var_name) {
    auto str_var_name = std::string(var_name);
    return get_var_type(str_var_name);
}

std::expected<std::shared_ptr<Type>, std::string> Scope::get_this() {
    return get_var_type(THIS);
}
