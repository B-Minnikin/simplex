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
    [[nodiscard]] auto maxCornerPoints() const -> unsigned long long;

private:
    objectiveType objective;
    std::vector<Constraint<int>> constraints;

    [[nodiscard]] auto getM() const -> size_t;
    [[nodiscard]] auto getN() const -> int;

    static auto getFactorial(int start) -> unsigned long long;
};

#endif //LINEAR_PROGRAMMING_SIMPLEX_H
