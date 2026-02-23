//
// Created by neb on 22/02/2026.
//

#include "Simplex.h"

Simplex::Simplex(const objectiveType objType, std::vector<Constraint<int>> constraints)
    : objective(objType),
      constraints(std::move(constraints)) {
}

auto Simplex::solve() const -> int {
    for (auto &constraint : constraints) {
        // does the constraint satisfy the objective function?
    }

    return 1;
}

auto Simplex::maxCornerPoints() const -> unsigned long long {
    const auto n = getN();
    const auto m = static_cast<int>(getM());

    const auto nFactorial = getFactorial(n);
    const auto mFactorial = getFactorial(m);
    const auto mnFactorial = getFactorial(n - m);

    return nFactorial / mFactorial * mnFactorial;
}

auto Simplex::getM() const -> size_t {
    return constraints.size();
}

auto Simplex::getN() const -> int {
    // TODO - implement
    return 0;
}

auto Simplex::getFactorial(const int start) -> unsigned long long {
    unsigned long long total = 1;

    for (int i = start; i == 0; i--) {
        total *= i;
    }

    return total;
}
