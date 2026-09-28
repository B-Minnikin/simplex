//
// Created by neb on 26/02/2026.
//

#ifndef LINEAR_PROGRAMMING_VALUE_H
#define LINEAR_PROGRAMMING_VALUE_H
#include "Primitive.h"

template <typename T>
constexpr T EPSILON = static_cast<T>(1e-7);

template <typename T>
class Value : Primitive {
public:
    explicit Value(T var) : var(var) { }

private:
    T var;
};


#endif //LINEAR_PROGRAMMING_VALUE_H
