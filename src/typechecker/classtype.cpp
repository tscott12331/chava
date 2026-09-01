#include <chava/typechecker.hpp>
#include <format>

ClassType::ClassType(const ClassDef& classdef) : Type(std::string(classdef.value.class_name)), 
                                           classdef(classdef) {}

bool ClassType::is_subtype_of(std::shared_ptr<Type> other) {
    if(other->get_name() == get_name()) {
        return true;
    }
    
    if(parent.has_value()) {
        return parent.value()->is_subtype_of(other);
    }

    return false;
}

std::expected<std::shared_ptr<Type>, std::string> ClassType::resolve_method_call_ret(MethodCallExp& method_call) {
    return std::unexpected("not implemented");
}

std::expected<void, std::string> ClassType::populate(TypeMap& type_map) {
    if(is_populated) {
        return {};
    }

    if(classdef.value.extend_class_name) {
        const auto& extend_class_name = std::string(classdef.value.extend_class_name.value());
        auto parent_type = type_map.get_type(extend_class_name);
        if(!parent_type) {
            return std::unexpected(create_error(classdef, std::format("Extending type {} does not exist", extend_class_name)));
        }

        if(auto pres = std::dynamic_pointer_cast<PrimitiveType>(parent_type.value()); pres) {
            return std::unexpected(create_error(classdef, std::format("Class {} cannot extend primitive type {}",
                                               classdef.value.class_name,
                                               pres->get_name())));
        }
    }

    if(auto res = populate_fields(type_map); !res) return res;
    if(auto res = populate_methods(type_map); !res) return res;

    is_populated = true;
    return {};
}

std::expected<void, std::string> ClassType::populate_fields(TypeMap& type_map) {
    for(const auto vardec_stmt : classdef.value.vardecs) {
        const auto& vardec = vardec_stmt.value.vardec.value;
        const auto& field_name = std::string(vardec.var);
        if(has_field(field_name)) {
            return std::unexpected(create_error(vardec_stmt, std::format("Redeclaration of field {} in class {}",
                                               field_name,
                                               classdef.value.class_name)));
        }

        const auto& type_name = to_string(vardec.type);
        const auto& type = type_map.get_type(type_name);
        if(!type) {
            return std::unexpected(create_error(vardec.type, std::format("Type {} doesn't exist", type_name)));
        }

        fields[field_name] = type.value();
    }

    return {};
}

std::expected<void, std::string> ClassType::populate_methods(TypeMap& type_map) {
    return std::unexpected("not implemented");
}

bool ClassType::has_field(const std::string& field_name) {
    return fields.contains(field_name) ||
          (parent && parent.value()->has_field(field_name));
}
