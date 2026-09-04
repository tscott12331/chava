#include <chava/parser.hpp>
#include <expected>
#include <format>
#include <stdexcept>
#include <variant>

std::expected<ParsedType, std::string> Parser::parse_type() {
    auto type_token = get_token();
    if(!type_token) {
        return std::unexpected(type_token.error());
    }

    ParsedTypeVariant type;
    switch(type_token->type) {
        case TokenType::IntToken:
            type = ParsedPrimitiveType::Int;
            break;
        case TokenType::BoolToken:
            type = ParsedPrimitiveType::Bool;
            break;
        case TokenType::VoidToken:
            type = ParsedPrimitiveType::Void;
            break;
        case TokenType::IdentToken:
            type = ParsedClassType{
                .class_name=type_token->raw,
            };
            break;
        default:
            return std::unexpected(std::format("Can't use {} as a type", token_to_string(type_token.value())));
    }

    cursor += 1;

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
        [](const ParsedPrimitiveType& prm) -> std::string { return to_string(prm); },
        [](const ParsedClassType& cls) -> std::string { return to_string(cls); },
    }, parsed_type.value);
}

std::string to_string(const ParsedPrimitiveType& parsed_type) {
    switch(parsed_type) {
        case ParsedPrimitiveType::Int: return INT_NAME;
        case ParsedPrimitiveType::Bool: return BOOL_NAME;
        case ParsedPrimitiveType::Void: return VOID_NAME;
        default: throw std::logic_error("Unhandled primitive to_string type");
    }
}

std::string to_string(const ParsedClassType& parsed_type) {
    return std::string(parsed_type.class_name);
}
