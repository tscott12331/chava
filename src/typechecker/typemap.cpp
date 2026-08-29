#include <chava/typechecker.hpp>

TypeMap::TypeMap() {}
TypeMap::TypeMap(const std::vector<std::shared_ptr<Type>>& types) {
    for(const auto t : types) {
        auto owned_name = std::string(t->get_name());
        known_types[owned_name] = t;
    }
}

std::optional<std::shared_ptr<Type>> TypeMap::get_type(const std::string& name) {
    if(auto it = known_types.find(name); it != known_types.end()) {
        return it->second;
    }

    return std::nullopt;
}
