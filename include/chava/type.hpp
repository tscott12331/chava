#ifndef TYPE_HPP
#define TYPE_HPP

#include <string_view>
#include <variant>
enum class PrimitiveType {
    Int,
    Bool,
    Void
};

struct ClassType {
    std::string_view class_name;
};

using Type = std::variant<
    PrimitiveType,
    ClassType
>;

#endif
