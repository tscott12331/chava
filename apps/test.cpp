#include "chava/test/test.hpp"
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
    }) << std::endl;
}
