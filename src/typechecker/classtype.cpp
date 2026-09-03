#include <chava/typechecker.hpp>
#include <format>

ClassType::ClassType(const ClassDef& classdef) : ClassType(classdef, std::nullopt) {}
ClassType::ClassType(const ClassDef& classdef, std::optional<std::shared_ptr<ClassType>> parent)
    : Type(std::string(classdef.value.class_name)), classdef(classdef), parent(parent) {
}

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

    // TODO: check if necessary? doesn't seem like it, we already add ClassType parent in constructor
    // if(classdef.value.extend_class_name) {
    //     std::cout << "type is extending type\n";
    //     const auto& extend_class_name = std::string(classdef.value.extend_class_name.value());
    //     auto parent_type = type_map.get_type(extend_class_name);
    //     if(!parent_type) {
    //         return std::unexpected(create_error(classdef, std::format("Extending type {} does not exist", extend_class_name)));
    //     }
    //     std::cout << "have valid parent type\n";
    //
    //     if(auto pres = std::dynamic_pointer_cast<PrimitiveType>(parent_type.value()); pres != nullptr) {
    //         return std::unexpected(create_error(classdef, std::format("Class {} cannot extend primitive type {}",
    //                                            classdef.value.class_name,
    //                                            pres->get_name())));
    //     }
    // }

    if(auto res = populate_fields(type_map); !res) return res;
    if(auto res = populate_constructor(type_map); !res) return res;
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
    for(const auto& md : classdef.value.method_defs) {
        const auto method_signature = MethodSignature::from(md.value, type_map);
        if(!method_signature) {
            return std::unexpected(method_signature.error());
        }

        if(const auto res = check_redeclaration(method_signature.value(), md.pos); !res) {
            return res;
        }
        if(parent) {
            if(const auto res = parent.value()->check_override(method_signature.value(), md.pos); !res) {
                return res;
            }
        }

        methods[method_signature->name()].insert(method_signature.value());
    }

    return {};
}

std::expected<void, std::string> ClassType::populate_constructor(TypeMap& type_map) {
    std::vector<std::shared_ptr<Type>> types;
    for(const auto& vd : classdef.value.constructor.value.params.value.vardecs) {
        const auto type_name = to_string(vd.value.type);
        const auto type = type_map.get_type(type_name);
        if(!type) {
            return std::unexpected(create_error(vd, std::format("Unknown type {} in constructor", type_name)));
        }

        types.push_back(type.value());
    }

    constructor_args = TypeList(types);
    return {};
}

std::expected<void, std::string> ClassType::check_redeclaration(const MethodSignature& method_signature, const Position& pos) {
    if(!methods.contains(method_signature.name())) { return {}; }
    const auto& same_name = methods[method_signature.name()];
    if(same_name.contains(method_signature)) {
        return std::unexpected(create_error(pos, std::format("Redefinition of method {}", method_signature.name())));
    }

    return {};
}

std::expected<void, std::string> ClassType::check_override(const MethodSignature& method_signature, const Position& pos) {
    if(!methods.contains(method_signature.name())) { return {}; }
    const auto& same_name = methods[method_signature.name()];

    for(const auto& ms : same_name) {
        if(method_signature.has_same_params(ms) && !method_signature.can_override(ms)) {
            return std::unexpected(create_error(pos, std::format("Method {} cannot override inherited method", method_signature.name())));
        }
    }
    
    if(parent) {
        return parent.value()->check_override(method_signature, pos);
    }

    return {};
}

bool ClassType::has_field(const std::string& field_name) {
    return fields.contains(field_name) ||
          (parent && parent.value()->has_field(field_name));
}

std::expected<void, std::string> ClassType::check_super_args(const TypeList& args, const Position& pos) {
    if(!parent) {
        return std::unexpected(create_error(pos, "Cannot call super constructor on base class"));
    }

    if(!args.can_assign_to(parent.value()->constructor_args)) {
        return std::unexpected(create_error(pos, std::format("Super arguments do not match {} class constructor", parent.value()->get_name())));
    }

    return {};
}
