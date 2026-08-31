#ifndef EXP_HPP
#define EXP_HPP

#include "chava/parser_misc.hpp"
#include <memory>
#include <chava/type.hpp>
#include <optional>
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

using ExpVariant = std::variant<
    VarExp,
    StrLitExp,
    NumLitExp,
    BoolLitExp,
    ThisExp,
    std::shared_ptr<NewObjExp>,
    std::shared_ptr<MethodCallExp>,
    std::shared_ptr<BinaryExp>
>;

using Exp = PositionWrapper<ExpVariant>;

struct CommaExp {
    std::vector<Exp> exps;
};

struct VarExp {
    std::string_view var;

    VarExp(std::string_view var) : var(var) {};

    void annotate_is_field(bool is_field);
private:
    bool is_field = false;
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

struct BinaryExp {
    Exp left;
    Op op;
    Exp right;
};

struct MethodCallExp {
    Exp target;
    std::string_view method_name;
    CommaExp args;

    MethodCallExp(Exp& target, std::string_view method_name, CommaExp& args) : 
                    target(std::move(target)), method_name(method_name), args(std::move(args)) {}
    // annotation
    void annotate_ret_type(ParsedType ret_type);

private:
    std::optional<ParsedType> ret_type = std::nullopt;
};


#endif
