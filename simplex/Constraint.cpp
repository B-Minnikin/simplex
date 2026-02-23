//
// Created by neb on 22/02/2026.
//

#include "Constraint.h"

template<typename T>
auto Constraint<T>::isEquation() -> bool {
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
