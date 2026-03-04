//
// Created by neb on 23/02/2026.
//

#ifndef LINEAR_PROGRAMMING_TABLEAU_H
#define LINEAR_PROGRAMMING_TABLEAU_H
#include <memory>
#include <unordered_map>
#include <vector>

#include "../data_types/Expression.h"


template <typename T>
class Tableau {
public:
    Tableau(
        ObjectiveType objectiveType,
        Constraint<T>& objectiveFunction,
        std::vector<Constraint<T>>& constraints
    );

    [[nodiscard]] auto isSolved() const -> bool {
        return getSmallestObjectiveCoefficient() >= 0;
    }

    [[nodiscard]] auto getFinalObjective() const -> std::vector<Variable<T>>;

    auto pivot() const -> void;

private:
    std::vector<Variable<T>> vars = {};
    std::vector<T> tableauCoefficients;
    std::unordered_map<int, int> varMap = {};

    int primaryVariableCount = 0;
    int columnCount;
    int rowCount;

    auto printTableau() const -> void;

    auto reformulateObjective(std::shared_ptr<Constraint<T>> objective) -> void;
    auto extractVariables(const Expression<T> &expr) const -> void;
    [[nodiscard]] auto isColumnBasic(int columnIndex) const -> bool;
    [[nodiscard]] auto getSmallestObjectiveCoefficient() const -> T;
    [[nodiscard]] auto getPivotColumn() const -> int;
    [[nodiscard]] auto getPivotRow(int columnIndex) const -> int;
    auto performPivot(int columnIndex, int rowIndex) const -> void;
    auto zeroOtherRows(int columnIndex, int rowIndex) const -> void;
    [[nodiscard]] auto getBasicColumnSolution(int columnIndex) const -> T;
};


#endif //LINEAR_PROGRAMMING_TABLEAU_H
