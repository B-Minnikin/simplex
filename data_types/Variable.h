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
    explicit Variable(T var, std::string symbol)
        : symbol(std::move(symbol)),
          var(var) { }

private:
    std::string symbol; // TODO - handle this
    T var;
};


#endif //LINEAR_PROGRAMMING_VARIABLE_H
