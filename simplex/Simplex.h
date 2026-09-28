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
void PrintTo(const Variable<T>& t, std::ostream* os) {
    *os << t.getCoefficient() << " * " << t.getSymbol();
}

template <typename T>
class Simplex {
public:
    explicit Simplex(
        const ObjectiveType objType,
        const Constraint<T> objectiveFunction,
        std::vector<Constraint<T>> constraints
        )
            : constraints(constraints),
              objectiveFunction(objectiveFunction),
              objectiveType(objType)
        { }

    [[nodiscard]] auto solve() -> std::vector<Variable<T>> {
        auto tableau = Tableau<T>(objectiveType, objectiveFunction, constraints);

        if (tableau.isRequiresTwoPhase(constraints)) {
            tableau.initPhaseOne(constraints);
            tableau.print();

            while (true) {
                const auto solution_status = tableau.isPhaseOneSolved();

                if (solution_status == Infeasible) {
                    std::cout << "Problem is infeasible\n";
                    return {};
                }

                if (solution_status == Degenerate) {
                    std::cout << "Problem is degenerate\n";
                    return {};
                }

                if (solution_status == Optimal) {
                    std::cout << "Phase 1 is optimal\n";
                    break;
                }

                const auto pivotColumnIndex = tableau.getMinimisedPivotColumn();
                if (!pivotColumnIndex) {
                    return {};
                }

                tableau.pivot(pivotColumnIndex.value());
                tableau.print();
            }

            tableau.prepareForPhaseTwo(constraints);
        }

        tableau.rebalanceBasicVariables();
        tableau.print();

        while (!tableau.isSolved()) {
            const auto pivotColumnIndex = tableau.getPivotColumn();
            if (!pivotColumnIndex) {
                // TODO - add error handling
                return {};
            }

            tableau.pivot(pivotColumnIndex.value());
            tableau.print();
        }

        return tableau.getFinalObjective();
    }

private:
    std::vector<Constraint<T>> constraints;
    Constraint<T> objectiveFunction;
    ObjectiveType objectiveType;
};

#endif //LINEAR_PROGRAMMING_SIMPLEX_H
