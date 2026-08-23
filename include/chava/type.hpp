#ifndef TYPE_HPP
#define TYPE_HPP

#include <string_view>
#include <variant>
enum class ParsedPrimitiveType {
    Int,
    Bool,
    Void
};

struct ParsedClassType {
    std::string_view class_name;
};

using ParsedType = std::variant<
    ParsedPrimitiveType,
    ParsedClassType
>;

#endif
