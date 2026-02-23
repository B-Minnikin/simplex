//
// Created by neb on 22/02/2026.
//

#ifndef LINEAR_PROGRAMMING_SIMPLEX_H
#define LINEAR_PROGRAMMING_SIMPLEX_H
#include <vector>

#include "Constraint.h"
#include "EnumTypes.h"

// objective function
// variables
// constraints

// add slack
// walk the boundary
// somehow work out the optimal solution

class Simplex {

public:
    explicit Simplex(objectiveType objType, std::vector<Constraint<int>> constraints);

    [[nodiscard]] auto solve() const -> int;

private:
    objectiveType objective;
    std::vector<Constraint<int>> constraints;
};

#endif //LINEAR_PROGRAMMING_SIMPLEX_H
