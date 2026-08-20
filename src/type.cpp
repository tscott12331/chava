#include <chava/parser.hpp>
#include <chava/type.hpp>
#include <expected>
#include <format>
#include <string_view>

std::expected<Type, std::string_view> Parser::parse_type() {
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

std::expected<Vardec, std::string_view> Parser::parse_vardec() {
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

std::expected<CommaVardec, std::string_view> Parser::parse_comma_vardec() {
    std::vector<Vardec> vardecs;

    auto vardec = parse_vardec();
    if(!vardec) {
        return CommaVardec{
            .vardecs=std::move(vardecs),
        };
    }

    auto token = get_token_of(TokenType::CommaToken);
    while(token) {
        cursor += 1;

        vardec = parse_vardec();
        if(!vardec) {
            return std::unexpected(vardec.error());
        }
        vardecs.push_back(std::move(vardec.value()));

        token = get_token_of(TokenType::CommaToken);
    }

    return CommaVardec{
        .vardecs=std::move(vardecs),
    };
}
