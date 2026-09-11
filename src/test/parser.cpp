#include "chava/exp.hpp"
#include "chava/tokenizer.hpp"
#include <chava/parser.hpp>
#include <chava/stmt.hpp>
#include <chava/test/test_parser.hpp>
#include <memory>

Test::test_fn_ret test_parse_prim_exp() {
    std::vector<std::string> results;
    Test::collect_assert(results, Test::assert_eq(
        Stmt{
           .value=ExpStmt{
                .exp=Exp{
                    .value=VarExp("yo"),
                    .pos={.line=1, .col=1},
                }
            },
            .pos={.line=1,.col=1}
        },
        Parser::Parse(Tokenizer::Tokenize("yo;").value())->stmts.at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Stmt{
           .value=ExpStmt{
                .exp=Exp{
                    .value=NumLitExp{.val=5},
                    .pos={.line=1, .col=1},
                }
            },
            .pos={.line=1,.col=1}
        },
        Parser::Parse(Tokenizer::Tokenize("5;").value())->stmts.at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Stmt{
           .value=ExpStmt{
                .exp=Exp{
                    .value=StrLitExp{.str="\"hello\""},
                    .pos={.line=1, .col=1},
                }
            },
            .pos={.line=1,.col=1}
        },
        Parser::Parse(Tokenizer::Tokenize("\"hello\";").value())->stmts.at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Stmt{
           .value=ExpStmt{
                .exp=Exp{
                    .value=StrLitExp{.str="\"hello\""},
                    .pos={.line=1, .col=2},
                }
            },
            .pos={.line=1,.col=2}
        },
        Parser::Parse(Tokenizer::Tokenize("(\"hello\");").value())->stmts.at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Stmt{
           .value=ExpStmt{
                .exp=Exp{
                    .value=ThisExp{},
                    .pos={.line=1, .col=1},
                }
            },
            .pos={.line=1,.col=1}
        },
        Parser::Parse(Tokenizer::Tokenize("this;").value()).value().stmts.at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Stmt{
           .value=ExpStmt{
                .exp=Exp{
                    .value=BoolLitExp{.val=true},
                    .pos={.line=1, .col=1},
                }
            },
            .pos={.line=1,.col=1}
        },
        Parser::Parse(Tokenizer::Tokenize("true;").value())->stmts.at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Stmt{
           .value=ExpStmt{
                .exp=Exp{
                    .value=BoolLitExp{.val=false},
                    .pos={.line=1, .col=1},
                }
            },
            .pos={.line=1,.col=1}
        },
        Parser::Parse(Tokenizer::Tokenize("false;").value())->stmts.at(0)
    ));

    Test::collect_assert(results, Test::assert_eq(
        Stmt{
           .value=ExpStmt{
                .exp=Exp{
                    .value=std::make_shared<NewObjExp>(NewObjExp{
                        .class_name="A",
                        .args=CommaExp{
                            .value=CommaExpValue{
                                    .exps=std::vector<Exp>{
                                        Exp{
                                            .value=NumLitExp{
                                                .val=5
                                            },
                                            .pos=Position{.line=1, .col=7}
                                    }
                                }
                            },
                            .pos=Position{
                                .line=1,
                                .col=7
                            }
                        },
                    }),
                    .pos={.line=1, .col=1},
                }
            },
            .pos={.line=1,.col=1}
        },
        Parser::Parse(Tokenizer::Tokenize("new A(5);").value())->stmts.at(0)
    ));

    return Test::get_result_ret(results);
}
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
