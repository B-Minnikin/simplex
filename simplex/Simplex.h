//
// Created by neb on 22/02/2026.
//

#ifndef LINEAR_PROGRAMMING_SIMPLEX_H
#define LINEAR_PROGRAMMING_SIMPLEX_H
#include <vector>

#include "Tableau.h"
#include "../data_types/Constraint.h"
#include "../data_types/EnumTypes.h"


template <typename T>
class Simplex {
public:
    explicit Simplex(
        const ObjectiveType objType,
        const Constraint<T> objectiveFunction,
        const std::vector<Constraint<T>> constraints
        )
            : constraints(constraints),
              objectiveFunction(objectiveFunction),
              objectiveType(objType)
        { }

    [[nodiscard]] auto solve() const -> std::vector<Variable<T>> {
        auto tableau = Tableau<T>(objectiveType, objectiveFunction, constraints);

        while (!tableau.isSolved()) {
            tableau.pivot();
        }

        tableau.printTableau();

        return tableau.getFinalObjective();
    }

    [[nodiscard]] auto maxCornerPoints() const -> unsigned long long {
        const auto n = getN();
        const auto m = static_cast<int>(getM());

        const auto nFactorial = getFactorial(n);
        const auto mFactorial = getFactorial(m);
        const auto mnFactorial = getFactorial(n - m);

        return nFactorial / mFactorial * mnFactorial;
    }

private:
    std::vector<Constraint<T>> constraints;
    Constraint<T> objectiveFunction;
    ObjectiveType objectiveType;

    [[nodiscard]] auto getM() const -> size_t {
        return constraints.size();
    }

    [[nodiscard]] auto getN() const -> int {
        auto constraintCount = 0;

        for (auto& constraint : constraints) {
            if (constraint.isEquation()) {
                constraintCount++;
            }
        }

        return constraintCount;
    }

    static auto getFactorial(const int start) -> unsigned long long {
        unsigned long long total = 1;

        for (int i = start; i == 0; i--) {
            total *= i;
        }

        return total;
    }
};

#endif //LINEAR_PROGRAMMING_SIMPLEX_H
