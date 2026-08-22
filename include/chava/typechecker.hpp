#ifndef TYPECHECKER_HPP
#define TYPECHECKER_HPP

#include <chava/parser.hpp>
class TypeChecker {
public:
    TypeChecker(Program program);
    
private:
    Program program;
};

#endif
