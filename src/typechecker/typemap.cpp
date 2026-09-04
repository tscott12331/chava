#include <chava/typechecker.hpp>
#include <format>

TypeMap::TypeMap() {}
TypeMap::TypeMap(const std::vector<std::shared_ptr<Type>>& initial_types) {
    for(const auto t : initial_types) {
        auto owned_name = std::string(t->get_name());
        _types[owned_name] = t;
        _built_ins[owned_name] = t;
    }
}

const std::unordered_map<std::string, std::shared_ptr<Type>>& TypeMap::types() const {
    return _types;
}

std::optional<std::shared_ptr<Type>> TypeMap::get_type(const std::string& name) {
    if(auto it = _types.find(name); it != _types.end()) {
        return it->second;
    }

    return std::nullopt;
}

std::optional<std::shared_ptr<Type>> TypeMap::get_built_in(const std::string& name) {
    if(auto it = _built_ins.find(name); it != _built_ins.end()) {
        return it->second;
    }

    return std::nullopt;
}
std::expected<void, std::string> TypeMap::define(std::shared_ptr<Type> type) {
    if(const auto& res = get_type(type->get_name()); res) {
        return std::unexpected(std::format("Type {} already defined", type->get_name()));
    }

    _types[type->get_name()] = type;
    return {};
}

const std::unordered_map<std::string, std::shared_ptr<Type>>& TypeMap::built_ins() const {
    return _built_ins;
}
