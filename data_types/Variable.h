//
// Created by neb on 22/02/2026.
//

#ifndef LINEAR_PROGRAMMING_VARIABLE_H
#define LINEAR_PROGRAMMING_VARIABLE_H
#include <string>

#include "Primitive.h"


template <typename T>
class Variable : Primitive {
public:
    explicit Variable(std::string symbol)
        : coefficient(1),
          symbol(std::move(symbol))
           { }

    explicit Variable(T coefficient, std::string symbol)
        : coefficient(coefficient),
          symbol(std::move(symbol))
           { }

    [[nodiscard]] auto getCoefficient() const -> T {
        return coefficient;
    }

    [[nodiscard]] auto getSymbol() const -> std::string {
        return symbol;
    }

private:
    T coefficient;
    std::string symbol;
};


#endif //LINEAR_PROGRAMMING_VARIABLE_H
