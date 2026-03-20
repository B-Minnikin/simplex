//
// Created by neb on 22/02/2026.
//

#ifndef LINEAR_PROGRAMMING_VARIABLE_H
#define LINEAR_PROGRAMMING_VARIABLE_H
#include <string>

#include "EnumTypes.h"
#include "Primitive.h"

template <typename T>
struct VarParams {
    T coefficient = 1.0;
    std::string symbol = "-";
    VarKind kind = Var;
};


template <typename T>
class Variable : public Primitive {
public:
    explicit Variable(VarParams<T> params)
        : coefficient(params.coefficient),
          symbol(std::move(params.symbol)),
          kind(params.kind) { }

    auto operator+=(const Variable val) -> void {
        coefficient += val.getCoefficient();
    }

    auto operator*=(T val) -> void {
        coefficient *= val;
    }

    auto operator==(const Variable &other) const -> bool {
        return other.getSymbol() == symbol
            && std::abs(other.getCoefficient() - coefficient) < 1e-9;
    }

    [[nodiscard]] auto getCoefficient() const -> T {
        return coefficient;
    }

    auto setCoefficient(T c) -> void {
        coefficient = c;
    }

    auto negateCoefficient() -> void {
        coefficient *= -1;
    }

    [[nodiscard]] auto getSymbol() const -> std::string {
        return symbol;
    }

    [[nodiscard]] auto getKind() const -> VarKind {
        return kind;
    }

    auto setKind(const VarKind otherKind) -> void {
        kind = otherKind;
    }

protected:
    T coefficient;
    std::string symbol;
    VarKind kind;
};


#endif //LINEAR_PROGRAMMING_VARIABLE_H
