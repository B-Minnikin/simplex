//
// Created by neb on 22/02/2026.
//

#ifndef LINEAR_PROGRAMMING_EXPRESSION_H
#define LINEAR_PROGRAMMING_EXPRESSION_H
#include <map>

#include "Variable.h"
#include "EnumTypes.h"

template <typename T>
class Constraint;

template <typename T>
class Expression {
public:
    explicit Expression(const Variable<T> var) { // TODO - move in?
        expressions.push(var.getId(), var);
    }

    explicit Expression(std::vector<Variable<T>> otherExpressions) {
        for (const auto &var : otherExpressions) {
            expressions[var.getId()] = var;
        }
    }

    auto operator+(const Expression &expr) -> Expression<T> {
        expressions.merge(expr);

        return this;
    }

    // TODO - test this
    auto operator+(const Variable<T> &var) -> Expression<T> {
        expressions.push(var.getId(), var); // TODO - check for clashes - what should behaviour be?

        return this;
    }

    auto operator-(const Expression &expr) -> Expression<T> {
        expressions.merge(expr);

        return this;
    }

    auto operator<(Variable<T> constraintVariable) -> Constraint<T> { // TODO - move?
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

    [[nodiscard]] auto findVariable(int id) const -> Variable<T>* {
        if (!expressions.contains(id)) {
            return nullptr;
        }

        return expressions.at(id);
    }

    auto flipAllSigns() -> void {
        for (auto& expr : expressions) {
            expr.second *= -1;
        }
    }

private:
    std::map<int, Variable<T>> expressions = {};
};

template <typename T>
auto operator*(int coefficient, Variable<T> var) -> Expression<T> {
    return Expression<T>(coefficient, var);
}


#endif //LINEAR_PROGRAMMING_EXPRESSION_H
