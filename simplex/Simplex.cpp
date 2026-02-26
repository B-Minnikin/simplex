//
// Created by neb on 22/02/2026.
//

#include "Simplex.h"

#include "Tableau.h"

// all constraint right sides should be non-negative

// move everything to left side of objective - make equal to zero

template <typename T>
Simplex<T>::Simplex(
    const ObjectiveType objType,
    Constraint<T>& objectiveFunction,
    std::vector<Constraint<T>>& constraints
    )
        : objectiveType(objType),
          objectiveFunction(std::make_shared<T>(objectiveFunction)),
          constraints(std::make_shared<T>(constraints)) { }

template <typename T>
auto Simplex<T>::solve() const -> std::vector<T> {
    Tableau<T> tableau(objectiveFunction, constraints);

    // for (auto &constraint : constraints) {
    //     // does the constraint satisfy the objective function?
    //     // std::static_cast<>
    // }

    return -1;
}

template <typename T>
auto Simplex<T>::maxCornerPoints() const -> unsigned long long {
    const auto n = getN();
    const auto m = static_cast<int>(getM());

    const auto nFactorial = getFactorial(n);
    const auto mFactorial = getFactorial(m);
    const auto mnFactorial = getFactorial(n - m);

    return nFactorial / mFactorial * mnFactorial;
}

template <typename T>
auto Simplex<T>::addSlack() -> void {
    for (auto &constraint : constraints) {
        if (constraint.isEquation()) {
            // add an s variable
        }
    }
}

// auto Simplex::isEquation(const Constraint<int>* constraint) const -> bool {
//     // TODO - implement
//     // if constraint with single expression
//     // only 1 variable
//     // coefficient is 1
//
//     for (auto i : constraint)
//
//     return false;
// }

template <typename T>
auto Simplex<T>::getM() const -> size_t {
    return constraints.size();
}

template <typename T>
auto Simplex<T>::getN() const -> int {
    auto constraintCount = 0;

    for (const Constraint<T>& constraint : constraints) {
        if (constraint.isEquation()) {
            constraintCount++;
        }
    }

    return constraintCount;
}

template <typename T>
auto Simplex<T>::getFactorial(const int start) -> unsigned long long {
    unsigned long long total = 1;

    for (int i = start; i == 0; i--) {
        total *= i;
    }

    return total;
}
