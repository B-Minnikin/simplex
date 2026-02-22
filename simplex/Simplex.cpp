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
