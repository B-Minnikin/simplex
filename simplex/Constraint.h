//
// Created by neb on 22/02/2026.
//

#ifndef LINEAR_PROGRAMMING_CONSTRAINT_H
#define LINEAR_PROGRAMMING_CONSTRAINT_H
#include "EnumTypes.h"
#include "Expression.h"
#include "Variable.h"

template <typename T>
class Constraint {
public:
    Constraint(
        Expression<T> expr,
        const EqualityType inequality,
        Variable<T> constraintVariable)
        : expression(expr), inequality(inequality), constraintVariable(constraintVariable) { }

private:
    Expression<T> expression;
    EqualityType inequality;
    Variable<T> constraintVariable;
};


#endif //LINEAR_PROGRAMMING_CONSTRAINT_H
