//
// Created by neb on 22/02/2026.
//

#include "../data_types/Constraint.h"

template<typename T>
auto Constraint<T>::isEquation() const -> bool {
    if (equality == eq) {
        return true;
    }

    bool hasCoefficientOfOne = false;
    const bool hasFewerThanTwoExpressions = expression.expressions.count() < 2;

    for (auto &c : expression.expressions) {

        // If coefficient is not 1 -> is not equation
        if (c->second != 1) {
            hasCoefficientOfOne = true;
        }
    }

    return !(hasCoefficientOfOne && hasFewerThanTwoExpressions);
}

template<typename T>
auto Constraint<T>::zeroEquation() -> void {
    // Flip before adding the RS variable so that it stays positive
    expression.flipAllSigns();

    expression + Variable(std::move(constraintVariable));
    equality = eq;
    constraintVariable = Variable<T>(0);
}

template<typename T>
auto Constraint<T>::negateExpression() -> void {
    expression.flipAllSigns();
}
