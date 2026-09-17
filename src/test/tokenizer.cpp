#include "chava/test/test.hpp"
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

Test::test_fn_ret test_type_tokens() {
    std::vector<std::string> results;
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::IntToken,
            .raw="int",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("int")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::BoolToken,
            .raw="bool",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("bool")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::VoidToken,
            .raw="void",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("void")->at(0)
    ));

    return Test::get_result_ret(results);
}

Test::test_fn_ret test_control_flow_tokens() {
    std::vector<std::string> results;
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::IfToken,
            .raw="if",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("if")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::ElseToken,
            .raw="else",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("else")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::WhileToken,
            .raw="while",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("while")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::BreakToken,
            .raw="break",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("break")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::ReturnToken,
            .raw="return",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("return")->at(0)
    ));

    return Test::get_result_ret(results);
}
Test::test_fn_ret test_op_tokens() {
    std::vector<std::string> results;
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::PlusToken,
            .raw="+",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("+")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::DashToken,
            .raw="-",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("-")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::FSlashToken,
            .raw="/",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("/")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::StarToken,
            .raw="*",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("*")->at(0)
    ));

    return Test::get_result_ret(results);
}
Test::test_fn_ret test_literal_tokens() {
    std::vector<std::string> results;
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::TrueToken,
            .raw="true",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("true")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::FalseToken,
            .raw="false",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("false")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::IdentToken,
            .raw="identifier",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("identifier")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::NumberToken,
            .raw="1337",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("1337")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::StringToken,
            .raw="\"1337\"",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("\"1337\"")->at(0)
    ));

    return Test::get_result_ret(results);
}
Test::test_fn_ret test_class_tokens() {
    std::vector<std::string> results;
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::ClassToken,
            .raw="class",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("class")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::ExtendsToken,
            .raw="extends",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("extends")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::InitToken,
            .raw="init",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("init")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::MethodToken,
            .raw="method",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("method")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::NewToken,
            .raw="new",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("new")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::ThisToken,
            .raw="this",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("this")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::SuperToken,
            .raw="super",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("super")->at(0)
    ));

    return Test::get_result_ret(results);
}
Test::test_fn_ret test_symbol_tokens() {
    std::vector<std::string> results;
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::CommaToken,
            .raw=",",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize(",")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::LParenToken,
            .raw="(",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("(")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::RParenToken,
            .raw=")",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize(")")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::SemiColonToken,
            .raw=";",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize(";")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::AssignToken,
            .raw="=",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("=")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::EqualToken,
            .raw="==",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("==")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::NotEqualToken,
            .raw="!=",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("!=")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::LAngleToken,
            .raw="<",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("<")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::RAngleToken,
            .raw=">",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize(">")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::LBracketToken,
            .raw="{",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("{")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::RBracketToken,
            .raw="}",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize("}")->at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Token{
            .type=TokenType::DotToken,
            .raw=".",
            .pos=Position{.line=1, .col=1}
        },
        Tokenizer::Tokenize(".")->at(0)
    ));

    return Test::get_result_ret(results);
}


std::string tokens_to_string(std::vector<Token>& tokens) {
    std::string res = "[";
    for(const auto& token : tokens) {
        res += token_to_string(token);
    }
    res += "]";
    return res;
}
