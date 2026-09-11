#include "chava/test/test.hpp"
#include "chava/test/test_parser.hpp"
#include <iostream>
#include <chava/test/test_tokenizer.hpp>
#include <ostream>
int main() {
    std::cout << Test::summarize_tests(std::vector<Test::test_result>{
        Test::run_test(test_is_horizontal_space, "horizontal space detection"),
        Test::run_test(test_is_vertical_space, "vertical space detection"),
        Test::run_test(test_is_newline, "newline detection"),
        Test::run_test(test_is_valid_keyword_or_ident_char, "keyword/identifier character detection"),
        Test::run_test(test_is_num, "number detection"),
        Test::run_test(test_type_tokens, "type tokens"),
        Test::run_test(test_control_flow_tokens, "control flow tokens"),
        Test::run_test(test_op_tokens, "op tokens"),
        Test::run_test(test_literal_tokens, "literal tokens"),
        Test::run_test(test_class_tokens, "class tokens"),
        Test::run_test(test_symbol_tokens, "symbol tokens"),

        Test::run_test(test_parse_prim_exp, "primary expression parsing"),
        Test::run_test(test_parse_call_exp, "method call expression parsing"),
    }) << std::endl;
}
