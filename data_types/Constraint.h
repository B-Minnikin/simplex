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
        const EqualityType equality,
        Variable<T> constraintVariable)
        : expression(expr), equality(equality), constraintVariable(constraintVariable) { }

    [[nodiscard]] auto isEquation() const -> bool ;
    [[nodiscard]] auto getExpression() -> Expression<T>* { // TODO - raw pointer
        return expression;
    }
    [[nodiscard]] auto getEquality() const -> EqualityType {
        return equality;
    }
    [[nodiscard]] auto getVariable() const -> Variable<T>* {
        return constraintVariable;
    }
    [[nodiscard]] auto findVariable(int id) const -> Variable<T>* {
        return expression.findVariable(id);
    }

    auto addVariable(Variable<T> var) -> void {
        expression = expression + Expression(1, var);
    }

    auto zeroEquation() -> void;
    auto negateExpression() -> void;

private:
    Expression<T> expression;
    EqualityType equality;
    Variable<T> constraintVariable;
};


#endif //LINEAR_PROGRAMMING_CONSTRAINT_H
