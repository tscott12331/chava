#include "chava/exp.hpp"
#include "chava/test/test.hpp"
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

Test::test_fn_ret test_parse_call_exp() {
    std::vector<std::string> results;

    Test::collect_assert(results, Test::assert_eq(
        Stmt{
            .value=ExpStmt{
                .exp=Exp{
                    .value=std::make_shared<MethodCallExp>(MethodCallExp(
                        Exp{
                            .value=VarExp("a"),
                            .pos=Position{.line=1,.col=1}
                        },
                        "b",
                        CommaExp{
                            .value=CommaExpValue{
                                .exps=std::vector<Exp>{
                                    Exp{
                                        .value=NumLitExp{
                                            .val=5
                                        },
                                        .pos=Position{.line=1,.col=5}
                                    }
                                 }
                            },
                            .pos=Position{.line=1,.col=5}
                        }
                    )),
                    .pos=Position{.line=1,.col=1}
                }
            },
            .pos=Position{.line=1,.col=1}
        },
        Parser::Parse(Tokenizer::Tokenize("a.b(5);").value()).value().stmts.at(0)
    ));

    return Test::get_result_ret(results);
}

Test::test_fn_ret test_parse_mult_exp() {
    std::vector<std::string> results;

    Test::collect_assert(results, Test::assert_eq(
        Stmt{
            .value=ExpStmt{
                .exp=Exp{
                    .value=std::make_shared<BinaryExp>(BinaryExp{
                        .left=Exp{
                            .value=NumLitExp{
                                .val=5
                            },
                            .pos=Position{.line=1,.col=1}
                        },
                        .op=Op::Mult,
                        .right=Exp{
                            .value=NumLitExp{
                                .val=9
                            },
                            .pos=Position{.line=1,.col=3}
                        }
                    }),
                    .pos=Position{.line=1,.col=1}
                }
            },
            .pos=Position{.line=1,.col=1}
        },
        Parser::Parse(Tokenizer::Tokenize("5*9;").value()).value().stmts.at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Stmt{
            .value=ExpStmt{
                .exp=Exp{
                    .value=std::make_shared<BinaryExp>(BinaryExp{
                        .left=Exp{
                            .value=NumLitExp{
                                .val=5
                            },
                            .pos=Position{.line=1,.col=1}
                        },
                        .op=Op::Div,
                        .right=Exp{
                            .value=NumLitExp{
                                .val=9
                            },
                            .pos=Position{.line=1,.col=3}
                        }
                    }),
                    .pos=Position{.line=1,.col=1}
                }
            },
            .pos=Position{.line=1,.col=1}
        },
        Parser::Parse(Tokenizer::Tokenize("5/9;").value()).value().stmts.at(0)
    ));

    return Test::get_result_ret(results);
}

Test::test_fn_ret test_parse_add_exp() {
    std::vector<std::string> results;

    Test::collect_assert(results, Test::assert_eq(
        Stmt{
            .value=ExpStmt{
                .exp=Exp{
                    .value=std::make_shared<BinaryExp>(BinaryExp{
                        .left=Exp{
                            .value=NumLitExp{
                                .val=5
                            },
                            .pos=Position{.line=1,.col=1}
                        },
                        .op=Op::Add,
                        .right=Exp{
                            .value=NumLitExp{
                                .val=9
                            },
                            .pos=Position{.line=1,.col=3}
                        }
                    }),
                    .pos=Position{.line=1,.col=1}
                }
            },
            .pos=Position{.line=1,.col=1}
        },
        Parser::Parse(Tokenizer::Tokenize("5+9;").value()).value().stmts.at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Stmt{
            .value=ExpStmt{
                .exp=Exp{
                    .value=std::make_shared<BinaryExp>(BinaryExp{
                        .left=Exp{
                            .value=NumLitExp{
                                .val=5
                            },
                            .pos=Position{.line=1,.col=1}
                        },
                        .op=Op::Sub,
                        .right=Exp{
                            .value=NumLitExp{
                                .val=9
                            },
                            .pos=Position{.line=1,.col=3}
                        }
                    }),
                    .pos=Position{.line=1,.col=1}
                }
            },
            .pos=Position{.line=1,.col=1}
        },
        Parser::Parse(Tokenizer::Tokenize("5-9;").value()).value().stmts.at(0)
    ));

    return Test::get_result_ret(results);
}

Test::test_fn_ret test_parse_comp_exp() {
    std::vector<std::string> results;

    Test::collect_assert(results, Test::assert_eq(
        Stmt{
            .value=ExpStmt{
                .exp=Exp{
                    .value=std::make_shared<BinaryExp>(BinaryExp{
                        .left=Exp{
                            .value=NumLitExp{
                                .val=5
                            },
                            .pos=Position{.line=1,.col=1}
                        },
                        .op=Op::Lt,
                        .right=Exp{
                            .value=NumLitExp{
                                .val=9
                            },
                            .pos=Position{.line=1,.col=3}
                        }
                    }),
                    .pos=Position{.line=1,.col=1}
                }
            },
            .pos=Position{.line=1,.col=1}
        },
        Parser::Parse(Tokenizer::Tokenize("5<9;").value()).value().stmts.at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Stmt{
            .value=ExpStmt{
                .exp=Exp{
                    .value=std::make_shared<BinaryExp>(BinaryExp{
                        .left=Exp{
                            .value=NumLitExp{
                                .val=5
                            },
                            .pos=Position{.line=1,.col=1}
                        },
                        .op=Op::Gt,
                        .right=Exp{
                            .value=NumLitExp{
                                .val=9
                            },
                            .pos=Position{.line=1,.col=3}
                        }
                    }),
                    .pos=Position{.line=1,.col=1}
                }
            },
            .pos=Position{.line=1,.col=1}
        },
        Parser::Parse(Tokenizer::Tokenize("5>9;").value()).value().stmts.at(0)
    ));

    return Test::get_result_ret(results);
}

Test::test_fn_ret test_parse_eq_exp() {
    std::vector<std::string> results;

    Test::collect_assert(results, Test::assert_eq(
        Stmt{
            .value=ExpStmt{
                .exp=Exp{
                    .value=std::make_shared<BinaryExp>(BinaryExp{
                        .left=Exp{
                            .value=NumLitExp{
                                .val=5
                            },
                            .pos=Position{.line=1,.col=1}
                        },
                        .op=Op::Eq,
                        .right=Exp{
                            .value=NumLitExp{
                                .val=9
                            },
                            .pos=Position{.line=1,.col=4}
                        }
                    }),
                    .pos=Position{.line=1,.col=1}
                }
            },
            .pos=Position{.line=1,.col=1}
        },
        Parser::Parse(Tokenizer::Tokenize("5==9;").value()).value().stmts.at(0)
    ));
    Test::collect_assert(results, Test::assert_eq(
        Stmt{
            .value=ExpStmt{
                .exp=Exp{
                    .value=std::make_shared<BinaryExp>(BinaryExp{
                        .left=Exp{
                            .value=NumLitExp{
                                .val=5
                            },
                            .pos=Position{.line=1,.col=1}
                        },
                        .op=Op::NotEq,
                        .right=Exp{
                            .value=NumLitExp{
                                .val=9
                            },
                            .pos=Position{.line=1,.col=4}
                        }
                    }),
                    .pos=Position{.line=1,.col=1}
                }
            },
            .pos=Position{.line=1,.col=1}
        },
        Parser::Parse(Tokenizer::Tokenize("5!=9;").value()).value().stmts.at(0)
    ));

    return Test::get_result_ret(results);
}

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
