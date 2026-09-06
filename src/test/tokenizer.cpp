#include <chava/tokenizer.hpp>
#include <chava/test/test_tokenizer.hpp>

Test::test_fn_ret test_is_horizontal_space() {
    std::vector<std::string> results;
    Test::collect_assert(results, Test::assert_eq(
        true,
        is_horizontal_space(" ")
    ));
    Test::collect_assert(results, Test::assert_eq(
        true,
        is_horizontal_space("\t")
    ));

    return Test::get_result_ret(results);
}

Test::test_fn_ret test_is_vertical_space() {
    std::vector<std::string> results;
    Test::collect_assert(results, Test::assert_eq(
        true, 
        is_vertical_space(newline)
    ));
    Test::collect_assert(results, Test::assert_eq(
        true, 
        is_vertical_space("\v")
    ));
    Test::collect_assert(results, Test::assert_eq(
        true, 
        is_vertical_space("\f")
    ));

    return Test::get_result_ret(results);
}
Test::test_fn_ret test_is_newline() {
    std::vector<std::string> results;
    Test::collect_assert(results, Test::assert_eq(
        true, 
        is_newline(newline)
    ));

    return Test::get_result_ret(results);
}
Test::test_fn_ret test_is_valid_keyword_or_ident_char() {
    std::vector<std::string> results;
    Test::collect_assert(results, Test::assert_eq(
        true, 
        is_valid_keyword_or_ident_char('s')
    ));
    Test::collect_assert(results, Test::assert_eq(
        true, 
        is_valid_keyword_or_ident_char('S')
    ));
    Test::collect_assert(results, Test::assert_eq(
        true, 
        is_valid_keyword_or_ident_char('9')
    ));
    Test::collect_assert(results, Test::assert_eq(
        false, 
        is_valid_keyword_or_ident_char('$')
    ));

    return Test::get_result_ret(results);
}

Test::test_fn_ret test_is_num() {
    std::vector<std::string> results;
    Test::collect_assert(results, Test::assert_eq(
        true, 
        is_num('0')
    ));
    Test::collect_assert(results, Test::assert_eq(
        false, 
        is_num('a')
    ));

    return Test::get_result_ret(results);
}
