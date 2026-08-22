#include <chava/parser.hpp>
#include <expected>
#include <format>

std::expected<Type, std::string> Parser::parse_type() {
    auto type_token = get_token();
    if(!type_token) {
        return std::unexpected(type_token.error());
    }
    cursor += 1;

    Type type;
    switch(type_token->type) {
        case TokenType::IntToken:
            type = PrimitiveType::Int;
            break;
        case TokenType::BoolToken:
            type = PrimitiveType::Bool;
            break;
        case TokenType::VoidToken:
            return std::unexpected("Can't create a variable with type void");
        case TokenType::IdentToken:
            type = ClassType{
                .class_name=type_token->raw,
            };
            break;
        default:
            return std::unexpected(std::format("Can't use {} as a type", token_to_string(type_token.value())));
    }

    return type;
}

std::expected<Vardec, std::string> Parser::parse_vardec() {
    auto type = parse_type();
    if(!type) {
        return std::unexpected(type.error());
    }

    if(auto primitive = std::get_if<PrimitiveType>(&type.value())) {
        if(*primitive == PrimitiveType::Void) {
            return std::unexpected("Cannot declare variable of type void");
        }
    }

    auto var_token = get_token_of(TokenType::IdentToken);
    if(!var_token) {
        return std::unexpected(var_token.error());
    }
    cursor += 1;

    return Vardec{
        .type=type.value(),
        .var=var_token->raw,
    };
}
