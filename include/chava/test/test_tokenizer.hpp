#ifndef TEST_TOKENIZER_HPP
#define TEST_TOKENIZER_HPP

#include <chava/tokenizer.hpp>
#include <chava/test/test.hpp>

Test::test_fn_ret test_is_horizontal_space();
Test::test_fn_ret test_is_vertical_space();
Test::test_fn_ret test_is_newline();
Test::test_fn_ret test_is_valid_keyword_or_ident_char();
Test::test_fn_ret test_is_num();

Test::test_fn_ret test_type_tokens();
Test::test_fn_ret test_control_flow_tokens();
Test::test_fn_ret test_op_tokens();
Test::test_fn_ret test_literal_tokens();
Test::test_fn_ret test_class_tokens();
Test::test_fn_ret test_symbol_tokens();

std::string tokens_to_string(std::vector<Token>& tokens);


template <>
struct std::formatter<Token> : std::formatter<std::string> {
    template <typename FormatContext>
    auto format(const Token& token, FormatContext& ctx) const {
        std::string str = std::format("Token('{}')[{}:{}]", token_to_string(token), token.pos.line, token.pos.col);
        return std::formatter<std::string>::format(str, ctx);
    }
};

// 2. Formatter for std::vector of that type
template <typename T>
struct std::formatter<std::vector<T>> : std::formatter<std::string> {
    template <typename FormatContext>
    auto format(const std::vector<T>& vec, FormatContext& ctx) const {
        std::string result = "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) result += ", ";
            // Format each element using std::format
            result += std::format("{}", vec[i]);
        }
        result += "]";
        return std::formatter<std::string>::format(result, ctx);
    }
};

#endif
