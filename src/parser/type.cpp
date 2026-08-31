#include "chava/common.hpp"
#include <chava/parser.hpp>
#include <expected>
#include <format>
#include <stdexcept>
#include <variant>

const std::string INT_NAME = "int";
const std::string BOOL_NAME = "bool";
const std::string VOID_NAME = "void";

std::expected<ParsedType, std::string> Parser::parse_type() {
    auto type_token = get_token();
    if(!type_token) {
        return std::unexpected(type_token.error());
    }
    cursor += 1;

    ParsedTypeVariant type;
    switch(type_token->type) {
        case TokenType::IntToken:
            type = ParsedPrimitiveType::Int;
            break;
        case TokenType::BoolToken:
            type = ParsedPrimitiveType::Bool;
            break;
        case TokenType::VoidToken:
            return std::unexpected("Can't create a variable with type void");
        case TokenType::IdentToken:
            type = ParsedClassType{
                .class_name=type_token->raw,
            };
            break;
        default:
            return std::unexpected(std::format("Can't use {} as a type", token_to_string(type_token.value())));
    }

    return ParsedType{
        .value=type,
        .pos=std::move(type_token->pos),
    };
}

std::expected<Vardec, std::string> Parser::parse_vardec() {
    auto type = parse_type();
    if(!type) {
        return std::unexpected(type.error());
    }

    if(auto primitive = std::get_if<ParsedPrimitiveType>(&type.value().value)) {
        if(*primitive == ParsedPrimitiveType::Void) {
            return std::unexpected("Cannot declare variable of type void");
        }
    }

    auto var_token = get_token_of(TokenType::IdentToken);
    if(!var_token) {
        return std::unexpected(var_token.error());
    }
    cursor += 1;

    return Vardec{
        .value=VardecValue{
            .type=type.value(),
            .var=var_token->raw,
        },
        .pos=type->pos,
    };
}

std::string to_string(const ParsedType& parsed_type) {
    return std::visit(overloaded {
        [](const ParsedPrimitiveType& prm) -> std::string {
            switch(prm) {
                case ParsedPrimitiveType::Int: return INT_NAME;
                case ParsedPrimitiveType::Bool: return BOOL_NAME;
                case ParsedPrimitiveType::Void: return VOID_NAME;
                default: throw std::logic_error("Unhandled primitive to_string type");
            }
        },
        [](const ParsedClassType& cls) -> std::string { return std::string(cls.class_name); },
    }, parsed_type.value);
}
