#ifndef TYPE_HPP
#define TYPE_HPP

#include <string>
#include <chava/parser_misc.hpp>
#include <string_view>
#include <variant>

constexpr std::string INT_NAME = "int";
constexpr std::string BOOL_NAME = "bool";
constexpr std::string VOID_NAME = "void";

enum class ParsedPrimitiveType {
    Int,
    Bool,
    Void
};

struct ParsedClassType {
    std::string_view class_name;

    bool operator==(const ParsedClassType& other) const = default;
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

    bool operator==(const VardecValue& other) const = default;
};

using Vardec = PositionWrapper<VardecValue>;

#endif
