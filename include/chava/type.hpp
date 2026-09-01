#ifndef TYPE_HPP
#define TYPE_HPP

#include <chava/parser_misc.hpp>
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

using ParsedTypeVariant = std::variant<
    ParsedPrimitiveType,
    ParsedClassType
>;

using ParsedType = PositionWrapper<ParsedTypeVariant>;

std::string to_string(const ParsedType& parsed_type);
std::string to_string(const ParsedPrimitiveType& parsed_type);
std::string to_string(const ParsedClassType& parsed_type);

struct VardecValue {
    ParsedType type;
    std::string_view var;
};

using Vardec = PositionWrapper<VardecValue>;

#endif
