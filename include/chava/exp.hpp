#ifndef EXP_HPP
#define EXP_HPP

#include <chava/parser_misc.hpp>
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
struct CommaExpValue;

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

struct ThisExp {
    bool operator==(const ThisExp& other) const = default;
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

using CommaExp = PositionWrapper<CommaExpValue>;

struct VarExp {
    std::string_view var;

    VarExp(std::string_view var) : var(var) {};

    void annotate_is_field(bool is_field);

    bool operator==(const VarExp& other) const = default;
private:
    bool is_field = false;
};

struct StrLitExp {
    std::string_view str;

    bool operator==(const StrLitExp& other) const = default;
};

struct NumLitExp {
    int val;

    bool operator==(const NumLitExp& other) const = default;
};

struct BoolLitExp {
    bool val;

    bool operator==(const BoolLitExp& other) const = default;
};

struct CommaExpValue {
    std::vector<Exp> exps;

    bool operator==(const CommaExpValue& other) const = default;
};

struct NewObjExp {
    std::string_view class_name;
    CommaExp args;

    bool operator==(const NewObjExp& other) const = default;
};

struct BinaryExp {
    Exp left;
    Op op;
    Exp right;

    bool operator==(const BinaryExp& other) const = default;
};

struct MethodCallExp {
    Exp target;
    std::string_view method_name;
    CommaExp args;

    MethodCallExp(Exp& target, std::string_view method_name, CommaExp& args) : 
                    target(std::move(target)), method_name(method_name), args(std::move(args)) {}
    // annotation
    void annotate_ret_type(const std::string& string);

    bool operator==(const MethodCallExp& other) const = default;
private:
    std::optional<std::string> ret_type = std::nullopt;
};

bool operator==(const ExpVariant& left, const ExpVariant& right);

#endif
