//
// Created by neb on 19/03/2026.
//

#ifndef LINEAR_PROGRAMMING_OVERLOADS_H
#define LINEAR_PROGRAMMING_OVERLOADS_H
#include <string>

#include "Expression.h"
#include "Variable.h"

template <typename T = double>
[[nodiscard]] auto var(const std::string &symbol) -> Variable<T> {
    return Variable<T>({ .symbol = symbol, .kind = Var });
}

template <typename T = double>
[[nodiscard]] auto objectiveVar(const std::string &symbol) -> Variable<T> {
    return Variable<T>({ .symbol = symbol, .kind = Objective });
}

template <typename T = double>
[[nodiscard]] auto var(const std::string &symbol, VarKind kind) -> Variable<T> {
    return Variable<T>({ .symbol = symbol, .kind = kind });
}

template <typename T>
[[nodiscard]] auto operator*(T coefficient, Variable<T> v) -> Variable<T> {
    v.setCoefficient(coefficient);
    return v;
}

template <typename T = double>
[[nodiscard]] auto operator+(Variable<T> lv, Variable<T> rv) -> Expression<T> {
    return Expression<T>({ lv, rv });
}

template <typename T = double>
[[nodiscard]] auto operator <=(Expression<T> expr, T c) -> Constraint<T> {
    const auto var = Variable<T>({ .coefficient = c, .kind = Solution });

    return Constraint<T>(expr, std::move(var), lte);
}

template <typename T = double>
[[nodiscard]] auto operator <(Expression<T> expr, T c) -> Constraint<T> {
    const auto var = Variable<T>({ .coefficient = c, .kind = Solution });

    return Constraint<T>(expr, std::move(var), lt);
}

#endif //LINEAR_PROGRAMMING_OVERLOADS_H
