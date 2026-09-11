#ifndef TEST_PARSER_HPP
#define TEST_PARSER_HPP

#include <chava/test/test.hpp>

Test::test_fn_ret test_parse_vardec();

// exp
Test::test_fn_ret test_parse_prim_exp();
Test::test_fn_ret test_parse_call_exp();
Test::test_fn_ret test_parse_mult_exp();
Test::test_fn_ret test_parse_add_exp();
Test::test_fn_ret test_parse_comp_exp();
Test::test_fn_ret test_parse_eq_exp();

// stmt
Test::test_fn_ret test_parse_assign_stmt();
Test::test_fn_ret test_parse_vardec_stmt();
Test::test_fn_ret test_parse_exp_stmt();
Test::test_fn_ret test_parse_while_stmt();
Test::test_fn_ret test_parse_return_stmt();
Test::test_fn_ret test_parse_if_stmt();
Test::test_fn_ret test_parse_block_stmt();

// class
Test::test_fn_ret test_parse_classdef();

#endif
