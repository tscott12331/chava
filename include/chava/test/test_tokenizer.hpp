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

#endif
