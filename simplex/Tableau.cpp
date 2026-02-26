//
// Created by neb on 23/02/2026.
//

#include "Tableau.h"

#include <set>
#include <cstdint>
#include <format>

// simple tableau flat vector
// keep the number of elements

// column is basic if only one coefficient is 1 and the rest are zero

// reformulate objective
// add slacks - init with 1 for row
// create tableau with coefficients
// select smallest coefficient from objective row
// this is the pivot
// divide each row equation value by pivot coefficient
// select smallest -> coefficient is pivot element


template<typename T>
Tableau<T>::Tableau(const ObjectiveType objectiveType, Constraint<T> &objectiveFunction, std::vector<Constraint<T>> &constraints) {
    reformulateObjective();

    // Maximise by negating
    if (objectiveType == Minimise) {
        objectiveFunction.negateExpression();
    }

    // column count = each expression var + each slack (constraint count) + eq value
    // TODO - change this to a map
    // std::set<int> vars = {};
    auto slackCount = 0;

    extractVariables(vars, objectiveFunction);

    // Create the slack variables
    for (constexpr int i = 1; auto &constraint : constraints) {
        if (constraint.isEquation()) {
            slackCount++;
            constraint.addVariable(Variable<T>(0, std::format("s{}", i)));
        }

        extractVariables(vars, constraint.getExpression());
    }

    columnCount = 1 + static_cast<int>(vars.size()) + slackCount;
    rowCount = 1 + static_cast<int>(constraints.size());
    coreTableau (columnCount * rowCount);

    // formulate the objective equation
    // for (int i = 0; i < columnCount; i++) {
    //
    // }
    //
    // for (int i = 0; i < rowCount; i++) {
    //
    // }

    // make the map
    // for (auto &s : vars) {
    //     nonBasicIndices[s] = 1; // TODO
    // }

    // Make the first objective result row
    for (int i = 0; auto &var : vars) {

    }

    for (int i = 0; i < columnCount * rowCount; i++) {
        // if column 0 -> results
        if (i % columnCount == 0) {
            if (i == 0) {
                coreTableau[i] = objectiveFunction.getVariable(); // TODO - check
            }

            auto varIndex = i % columnCount - 1;
            coreTableau[i] = vars[varIndex].var;
        }

        // if row 0 -> objective

        // if variable does not exist in set -> give coefficient of zero
    }
}

template<typename T>
auto Tableau<T>::pivot() const -> void {

    // TODO - fill out and work out what to return

    // column is basic if only one coefficient is 1 and the rest are zero

    // add slacks - init with 1 for row
    // create tableau with coefficients

    // select smallest coefficient from objective row
    // this is the pivot

    // divide each row equation value by pivot coefficient
    // select smallest -> coefficient is pivot element

    const auto pivotColumnIndex = getPivotColumn();
    auto pivotRowIndex = getPivotRow(pivotColumnIndex);

}

template<typename T>
auto Tableau<T>::printTableau() const -> void {
    // TODO - implement
    // print symbol headers
    // iterate over every element
    // print coefficient
}

template<typename T>
auto Tableau<T>::reformulateObjective(std::shared_ptr<Constraint<T>> objective) -> void {
    objective.zeroEquation();
}

template<typename T>
auto Tableau<T>::extractVariables(const Expression<T> &expr) const -> void {
    for (auto &pair : expr.expressions) {
        vars.insert(pair.first.id, pair.first); // TODO - think about references / pointers
    }
}

template<typename T>
auto Tableau<T>::isColumnBasic(const int columnIndex) const -> bool {
    int oneCount = 0;

    for (int i = 0; i < rowCount; i++) {
        // TODO - need to check this
        if (const int coefficient = coreTableau[columnCount * i + columnIndex]; coefficient == 1) {
            oneCount++;

            if (oneCount > 1) {
                return false;
            }
        } else {
            if (coefficient != 0) {
                return false;
            }
        }
    }

    return true;
}

template<typename T>
auto Tableau<T>::getPivotColumn() const -> int {
    // objective function should be in first row
    // find index with the smallest number

    int smallestIndex = -1;
    int smallestCoefficient = INT32_MAX; // TODO type ?

    // trim first column because it's the solutions
    for (int i = 1; i < columnCount; i++) {
        if (coreTableau[i] < smallestCoefficient) {
            coreTableau[i] = smallestCoefficient;
            smallestIndex = i;
        }
    }

    return smallestIndex;
}

template<typename T>
auto Tableau<T>::getPivotRow(const int columnIndex) const -> int {
    // divide each column coefficient by coefficient in the solution column

    auto smallestRowIndex = -1;
    auto smallestResultColumnValue = -1;

    for (int i = 0; i < rowCount; i++) {
        auto valueIndex = rowCount * i + columnIndex;
        auto solutionIndex = rowCount * i;

        auto val = coreTableau[solutionIndex] / coreTableau[valueIndex];
        if (val < smallestResultColumnValue) {
            smallestRowIndex = i;
            smallestResultColumnValue = val;
        }
    }

    return smallestRowIndex;
}

template<typename T>
auto Tableau<T>::performPivot(const int columnIndex, const int rowIndex) const -> void {
    // get the pivot element value
    // divide by itself
    // divide all elements in the row by that value

    auto pivotElementIndex = rowCount * rowIndex + columnIndex;
    auto pivotElement = coreTableau[pivotElementIndex];
    // TODO - do I need to check for zero?

    // coreTableau[pivotElementIndex] /= pivotElement;

    for (int i = 0; i < columnCount; i++) {
        auto thisElementIndex = rowCount * rowIndex + i;
        coreTableau[thisElementIndex] /= pivotElement;
    }
}

template<typename T>
auto Tableau<T>::zeroOtherRows(const int columnIndex, const int rowIndex) const -> void {
    auto pivotElementIndex = rowCount * rowIndex + columnIndex;
    auto pivotElement = coreTableau[pivotElementIndex];

    for (int i = 0; i < rowCount; i++) {
        if (i == rowIndex) {
            continue;
        }

        auto thisElementIndex = rowCount * i + columnIndex;
        auto thisElement = coreTableau[thisElementIndex];

        // if zero, leave as is

        // TODO - rename this
        auto zeroCoefficient = thisElement * -1 * pivotElement;

        for (int j = 0; j < columnIndex; j++) {
            auto thisIndex = rowCount * i + j;
            coreTableau[thisIndex] + zeroCoefficient;
        }

        // coreTableau[thisElementIndex] = thisElement + thisElement * -1 * pivotElement;
        // this row - itself * pivot
    }
}

/*
 *   0 1 2
 *   3 4 5
 *   6 7 8
 */

template<typename T>
auto Tableau<T>::getBasicColumnSolution(const int columnIndex) const -> T {
    // find the row with a value of 1
    // once row is found, get the solution from column 0, row x

    for (int i = 0; i < rowCount; i++) {
        auto fieldIndex = i * rowCount + columnIndex;

        if (coreTableau[fieldIndex] == 1) {
            auto solutionIndex = rowCount * i;
            return coreTableau[solutionIndex];
        }
    }

    return -1;
}
