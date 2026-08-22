#ifndef EXP_HPP
#define EXP_HPP

#include <memory>
#include <variant>
#include <vector>
struct VarExp;
struct StrLitExp;
struct NumLitExp;
struct BoolLitExp;
struct ThisExp;
struct NewObjExp;
struct MethodCallExp;
struct BinaryExp;

enum class Op {
    Add,
    Sub,
    Mult,
    Div,

    Eq,
    NotEq,

    Lt,
    Gt,
};

using Expr = std::variant<
    VarExp,
    StrLitExp,
    NumLitExp,
    BoolLitExp,
    ThisExp,
    std::unique_ptr<NewObjExp>,
    std::unique_ptr<MethodCallExp>,
    std::unique_ptr<BinaryExp>
>;

struct CommaExp {
    std::vector<Expr> exps;
};

struct VarExp {
    std::string_view var;
};

struct StrLitExp {
    std::string_view str;
};

struct NumLitExp {
    int val;
};

struct BoolLitExp {
    bool val;
};

struct ThisExp {};

struct NewObjExp {
    std::string_view class_name;
    CommaExp args;
};

struct MethodCallExp {
    Expr target;
    std::string_view method_name;
    CommaExp args;
};

struct BinaryExp {
    Expr left;
    Op op;
    Expr right;
};

#endif
