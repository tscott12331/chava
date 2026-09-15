#include "chava/test/test.hpp"
#include "chava/test/test_parser.hpp"
#include <iostream>
#include <chava/test/test_tokenizer.hpp>
#include <ostream>
int main() {
    std::cout << Test::summarize_tests(std::vector<Test::test_result>{
        // ** TOKENIZER **
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

        // ** PARSER **
        
        // exps
        Test::run_test(test_parse_prim_exp, "primary expression parsing"),
        Test::run_test(test_parse_call_exp, "method call expression parsing"),
        Test::run_test(test_parse_mult_exp, "mult expression parsing"),
        Test::run_test(test_parse_add_exp, "add expression parsing"),
        Test::run_test(test_parse_comp_exp, "comparison expression parsing"),
        Test::run_test(test_parse_eq_exp, "equality expression parsing"),

        // stmts
        Test::run_test(test_parse_assign_stmt, "assign statement parsing"),
        Test::run_test(test_parse_vardec_stmt, "vardec statement parsing"),
        Test::run_test(test_parse_exp_stmt, "exp statement parsing"),
        Test::run_test(test_parse_while_stmt, "while statement parsing"),
        Test::run_test(test_parse_return_stmt, "return statement parsing"),
        Test::run_test(test_parse_if_stmt, "if statement parsing"),
        Test::run_test(test_parse_block_stmt, "block statement parsing"),
    }) << std::endl;
}
