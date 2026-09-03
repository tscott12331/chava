#include <chava/typechecker.hpp>
#include <format>
#include <memory>
Scope::Scope() {}
Scope::Scope(std::shared_ptr<Scope> parent, std::optional<std::shared_ptr<Type>> ret_type, bool is_while) 
            : parent(parent), _ret_type(ret_type), _is_while(is_while) {}

constexpr auto THIS = "this";

std::expected<void, std::string> Scope::define(const std::string& var_name, std::shared_ptr<Type> type, const Position& pos) {
    if(scope.contains(var_name)) {
        return std::unexpected(create_error(pos, std::format("{} is already defined in this local scope", var_name)));
    }

    scope[var_name] = type;

    return {}; 
}

std::expected<void, std::string> Scope::define(const Vardec& vardec, TypeMap& type_map) {
    const auto var_name = std::string(vardec.value.var);
    const auto type_name = to_string(vardec.value.type);
    const auto type = type_map.get_type(type_name);
    if(!type) {
        return std::unexpected(create_error(vardec, std::format("Type {} does not exist", type_name)));
    }

    return define(var_name, type.value(), vardec.pos);
}

std::expected<std::shared_ptr<Type>, std::string> Scope::get_var_type(std::string& var_name, const Position& pos) {
    if(!scope.contains(var_name)) {
        if(parent.has_value()) {
            return parent.value()->get_var_type(var_name, pos);
        }
        return std::unexpected(create_error(pos, std::format("{} is not defined", var_name)));
    }

    return scope[var_name];
}

std::expected<std::shared_ptr<Type>, std::string> Scope::get_var_type(std::string_view var_name, const Position& pos) {
    auto str_var_name = std::string(var_name);
    return get_var_type(str_var_name, pos);
}

std::expected<std::shared_ptr<Type>, std::string> Scope::get_this(const PositionWrapper<ThisExp>& exp) {
    return get_var_type(THIS, exp.pos);
}

std::optional<std::shared_ptr<Type>> Scope::ret_type() const {
    return _ret_type;
}
