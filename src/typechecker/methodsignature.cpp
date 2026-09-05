#include <chava/typechecker.hpp>
#include <format>
#include <limits>
#include <ranges>

MethodSignature::MethodSignature(const std::string& name, const TypeList& params, std::shared_ptr<Type> ret_type) 
    : _name(name), _params(params), _ret_type(ret_type) {}

const std::string& MethodSignature::name() const {
    return _name;
}
const TypeList& MethodSignature::params() const {
    return _params;
}
const std::shared_ptr<Type> MethodSignature::ret_type() const {
    return _ret_type;
}

std::expected<MethodSignature, std::string> MethodSignature::from(const MethodDefValue& method_def, TypeMap& type_map) {
    const auto& method_name = std::string(method_def.method_name);
    const auto ret_type_name = to_string(method_def.ret_type);
    const auto ret_type = type_map.get_type(ret_type_name);
    if(!ret_type) {
        return std::unexpected(create_error(method_def.ret_type, std::format("Unknown return type {}", ret_type_name)));
    }


    std::vector<std::shared_ptr<Type>> param_types_list;
    for(const auto& vd : method_def.params.value.vardecs) {
        const auto param_type_name = to_string(vd.value.type);
        const auto param_type = type_map.get_type(param_type_name);
        if(!param_type) {
            return std::unexpected(create_error(vd.value.type, std::format("Unknown parameter type {}", param_type_name)));
        }

        param_types_list.push_back(param_type.value());
    }
    const auto type_list = TypeList(param_types_list);

    return MethodSignature(method_name, type_list, ret_type.value());
}


bool MethodSignature::has_same_params(const MethodSignature& other) const {
    return _params == other.params();
}

bool MethodSignature::can_override(const MethodSignature& other) const {
    if(!_params.can_assign_to(other.params())) {
        return false;
    }

    return _ret_type->is_subtype_of(other.ret_type());
}


TypeList::TypeList() : TypeList(std::vector<std::shared_ptr<Type>>{}) {}
TypeList::TypeList(const std::vector<std::shared_ptr<Type>>& types) : _types(types) {}
std::vector<std::shared_ptr<Type>> TypeList::types() const { return _types; }

bool TypeList::can_assign_to(const TypeList& other) const {
    if(_types.size() != other._types.size()) return false;

    for(const auto [i, t] : std::views::enumerate(_types)) {
        const auto ot = other._types.at(i);
        if(!t->is_subtype_of(ot)) {
            return false;
        }
    }

    return true;
}

TypeListPrecision TypeList::compare(const TypeList& other) const {
    if(_types.size() != other._types.size()) return TypeListPrecision::Uncomparable;

    uint min_depth_this = std::numeric_limits<uint>::max();
    uint min_depth_other = std::numeric_limits<uint>::max();
    for(const auto [i, t] : std::views::enumerate(_types)) {
        const auto ot = other._types.at(i);
        if(!t->is_subtype_of(ot) && !ot->is_subtype_of(t)) {
            return TypeListPrecision::Uncomparable;
        }

        // we only want to consider depths that differ between the types
        if(t->get_depth() == ot->get_depth()) continue;

        if(t->get_depth() < min_depth_this) min_depth_this = t->get_depth();
        if(ot->get_depth() < min_depth_other) min_depth_other = ot->get_depth();
    }

    return min_depth_this < min_depth_other
        ? TypeListPrecision::More
        : min_depth_this > min_depth_other
        ? TypeListPrecision::Less
        : TypeListPrecision::Ambiguous;
}
