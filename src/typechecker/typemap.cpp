#include <chava/typechecker.hpp>

TypeMap::TypeMap() {}
TypeMap::TypeMap(const std::vector<std::shared_ptr<Type>>& initial_types) {
    for(const auto t : initial_types) {
        auto owned_name = std::string(t->get_name());
        _types[owned_name] = t;
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
