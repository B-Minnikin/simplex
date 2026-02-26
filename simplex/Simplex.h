//
// Created by neb on 22/02/2026.
//

#ifndef LINEAR_PROGRAMMING_SIMPLEX_H
#define LINEAR_PROGRAMMING_SIMPLEX_H
#include <memory>
#include <vector>

#include "../data_types/Constraint.h"
#include "../data_types/EnumTypes.h"

// objective function
// variables
// constraints

// add slack
// walk the boundary
// somehow work out the optimal solution

template <typename T>
class Simplex {
public:
    explicit Simplex(
        ObjectiveType objType,
        Constraint<T> &objective,
        std::vector<Constraint<T>> &constraints
    );

    [[nodiscard]] auto solve() const -> std::vector<T>;
    [[nodiscard]] auto maxCornerPoints() const -> unsigned long long;

    auto addSlack() -> void;

private:
    ObjectiveType objectiveType;
    std::shared_ptr<Constraint<T>> objectiveFunction;
    std::shared_ptr<std::vector<Constraint<T>>> constraints;

    [[nodiscard]] auto getM() const -> size_t;
    [[nodiscard]] auto getN() const -> int;

    static auto getFactorial(int start) -> unsigned long long;
};

#endif //LINEAR_PROGRAMMING_SIMPLEX_H
