//
// Created by neb on 22/02/2026.
//

#ifndef LINEAR_PROGRAMMING_EXPRESSION_H
#define LINEAR_PROGRAMMING_EXPRESSION_H
#include <map>

#include "Constraint.h"
#include "Variable.h"
#include "EnumTypes.h"

template <typename T>
class Constraint;

template <typename T>

class Expression {
public:
    Expression(const int coefficient, const Variable<T> variable) {
        expressions.push(variable, coefficient);
    }

    explicit Expression(std::map<Variable<T>, int> otherExpressions) {
        expressions = otherExpressions;
    }

    auto operator+(const Expression& expr) -> Expression<T> {
        expressions.merge(expr);

        return this;
    }

    auto operator-(const Expression& expr) -> Expression<T> {
        expressions.merge(expr);

        return this;
    }

    auto operator<(Variable<T> constraintVariable) -> Constraint<T> {
        return Constraint(this, lt, constraintVariable);
    }

    auto operator<=(Variable<T> constraintVariable) -> Constraint<T> {
        return Constraint(this, lte, constraintVariable);
    }

    auto operator>(Variable<T> constraintVariable) -> Constraint<T> {
        return Constraint(this, gt, constraintVariable);
    }

    auto operator>=(Variable<T> constraintVariable) -> Constraint<T> {
        return Constraint(this, gte, constraintVariable);
    }

private:
    std::map<Variable<T>, int> expressions = {};
};

template <typename T>
auto operator*(int coefficient, Variable<T> var) -> Expression<T> {
    return Expression<T>(coefficient, var);
}


#endif //LINEAR_PROGRAMMING_EXPRESSION_H
