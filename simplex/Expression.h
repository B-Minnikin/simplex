//
// Created by neb on 22/02/2026.
//

#ifndef LINEAR_PROGRAMMING_EXPRESSION_H
#define LINEAR_PROGRAMMING_EXPRESSION_H
#include <map>

#include "Constraint.h"
#include "Variable.h"

enum inequalityType {
    lt,
    lte,
    gt,
    gte,
};

template <typename T>

class Expression {
public:
    Expression(const int coefficient, const Variable<T> variable) {
        expressions.push(variable, coefficient);
    }

    Expression(std::map<Variable<T>, int> otherExpressions) {
        // expressions.push(variable, coefficient);
        // expressions.merge(otherExpressions);

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
        return Constraint(this, constraintVariable);
    }

    auto operator<=(Variable<T> constraintVariable) -> Constraint<T> {
        return Constraint(this, constraintVariable);
    }

    auto operator>(Variable<T> constraintVariable) -> Constraint<T> {
        return Constraint(this, constraintVariable);
    }

    auto operator>=(Variable<T> constraintVariable) -> Constraint<T> {
        return Constraint(this, constraintVariable);
    }

private:
    std::map<Variable<T>, int> expressions = {};
    // int coefficient;
    // T variable;
};

template <typename T>
auto operator*(int coefficient, Variable<T> var) -> Expression<T> {
    return Expression<T>(coefficient, var);
}


#endif //LINEAR_PROGRAMMING_EXPRESSION_H
