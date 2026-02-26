//
// Created by neb on 23/02/2026.
//

#ifndef LINEAR_PROGRAMMING_TABLEAU_H
#define LINEAR_PROGRAMMING_TABLEAU_H
#include <memory>
#include <unordered_map>
#include <vector>

#include "../data_types/Expression.h"

// TODO - investigate variable alignment order

template <typename T>
class Tableau {
public:
    Tableau(
        ObjectiveType objectiveType,
        Constraint<T>& objectiveFunction,
        std::vector<Constraint<T>>& constraints
    );

    auto pivot() const -> void;

private:
    int columnCount;
    int rowCount;
    std::vector<T> coreTableau;

    std::unordered_map<int, Variable<T>> vars = {};

    auto printTableau() const -> void;

    auto reformulateObjective(std::shared_ptr<Constraint<T>> objective) -> void;
    auto extractVariables(const Expression<T> &expr) const -> void;
    [[nodiscard]] auto isColumnBasic(int columnIndex) const -> bool;
    [[nodiscard]] auto getPivotColumn() const -> int;
    [[nodiscard]] auto getPivotRow(int columnIndex) const -> int;
    auto performPivot(int columnIndex, int rowIndex) const -> void;
    auto zeroOtherRows(int columnIndex, int rowIndex) const -> void;
    [[nodiscard]] auto getBasicColumnSolution(int columnIndex) const -> T;
};


#endif //LINEAR_PROGRAMMING_TABLEAU_H
