//
// Created by neb on 23/02/2026.
//

#include "Tableau.h"

#include <cmath>
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

const int OBJECTIVE_ROW_COUNT = 1;
const int SOLUTION_COLUMN_COUNT = 1;


template<typename T>
Tableau<T>::Tableau(const ObjectiveType objectiveType, Constraint<T> &objectiveFunction, std::vector<Constraint<T>> &constraints) {
    reformulateObjective();

    if (objectiveType == Minimise) {
        objectiveFunction.negateExpression();
    }

    auto slackCount = 0;

    extractVariables(objectiveFunction);

    // Create the slack variables
    for (constexpr int i = 1; auto &constraint : constraints) {
        if (constraint.isEquation()) {
            slackCount++;
            constraint.addVariable(Variable<T>(std::format("s{}", i)));
        }

        extractVariables(constraint.getExpression());
    }

    columnCount = SOLUTION_COLUMN_COUNT + static_cast<int>(vars.size()) + slackCount;
    rowCount = OBJECTIVE_ROW_COUNT + static_cast<int>(constraints.size());
    tableauCoefficients(columnCount * rowCount);

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
        // coreTableau[i] = var.second.coefficient;

        // from the objective, get the variable which matches the symbol

        // if last element, get the constraint variable
        if (i == vars.count() - 1) {
            tableauCoefficients[i] = objectiveFunction.getVariable().getCoefficient();
        }
    }

    auto x = vars[3];

    // TODO - change the solution column to last
    for (int i = columnCount; i < (columnCount - 1) * rowCount; i++) {
        // auto constraintIndex = (i / rowCount) - 1; // TODO - check
        // Math.floor(index - column count % column count)
        auto constraintIndex = std::floor((i - columnCount) % columnCount);

        // Last should be the solution
        if (i % columnCount == columnCount - 1) {
            // if (i == 0) {
            //     tableauCoefficients[i] = objectiveFunction.getVariable(); // TODO - check
            // }

            auto constraintCoefficient = constraints[constraintIndex].getVariable().getCoefficient();
            tableauCoefficients[i] = constraintCoefficient;

            continue;
        }

        // Normal column for each variable

        // get the var
        auto var = vars[constraintIndex];
        // check the id
        auto varId = var.getId();
        // find the id in the constraint expression
        auto thisConstraint = constraints[constraintIndex];
        auto matchingVar = thisConstraint.findVarible(varId);
        if (matchingVar == nullptr) {
            continue;
        }
        // get the coefficient from it
        auto thisCoefficient = matchingVar.getCoefficient();
        tableauCoefficients[i] = thisCoefficient;

        // if variable does not exist in set -> give coefficient of zero
    }
}

template<typename T>
auto Tableau<T>::getFinalObjective() const -> std::vector<Variable<T>> {
    std::vector<Variable<T>*> finalVariables = {};

    for (int primaryColumnIndex = 0; primaryColumnIndex < primaryVariableCount; primaryColumnIndex++) {
        auto thisVarIndex = primaryColumnIndex % (columnCount - primaryVariableCount);
        auto thisVar = vars[thisVarIndex];

        auto oneCount = 0;

        for (int rowIndex = 1; rowIndex < rowCount; rowIndex++) {
            auto elementIndex = rowIndex * columnCount + primaryColumnIndex;
            auto coefficient = tableauCoefficients[elementIndex];


            if (coefficient == 1) {
                oneCount += 1;

                auto solutionIndex = rowIndex * columnCount + columnCount - 1;
                auto solution = tableauCoefficients[solutionIndex];

                finalVariables.push_back(Variable<T>(solution, thisVar.getSymbol()));
            }

            if (oneCount > 1) {
                // TODO - warn
                break;
            }
        }
    }

    return finalVariables;
}

template<typename T>
auto Tableau<T>::pivot() const -> void {

    // TODO - fill out and work out what to return

    // column is basic if only one coefficient is 1 and the rest are zero

    // select smallest coefficient from objective row
    // this is the pivot

    // divide each row equation value by pivot coefficient
    // select smallest -> coefficient is pivot element

    const auto pivotColumnIndex = getPivotColumn();
    if (pivotColumnIndex == -1) {
        // TODO - implement
        // We should have reached the most optimal solution already
    }

    auto pivotRowIndex = getPivotRow(pivotColumnIndex);

    // TODO - perform pivot on row
    // TODO - perform pivot on other rows
    // TODO - loop around again and check for lowest coefficient from objective function
    // if no negative values found: solution found
    // return a new objective function which matches the solution coefficients
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
    // TODO - handle multiple instances in the same expression

    for (auto &pair : expr.expressions) {
        vars.insert(pair.first.id, pair.first); // TODO - think about references / pointers
    }

    primaryVariableCount = vars.count();
}

template<typename T>
auto Tableau<T>::isColumnBasic(const int columnIndex) const -> bool {
    int oneCount = 0;

    for (int i = 0; i < rowCount; i++) {
        // TODO - need to check this
        if (const int coefficient = tableauCoefficients[columnCount * i + columnIndex]; coefficient == 1) {
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
auto Tableau<T>::getSmallestObjectiveCoefficient() const -> T {
    // iterate over the objective expression
    // look for smallest number

    auto smallestCoefficient = std::numeric_limits<T>::max;

    // Do not include the solution column (last)
    for (int i = 0; i < columnCount; i++) {
        if (tableauCoefficients[i] < smallestCoefficient) {
            smallestCoefficient = tableauCoefficients[i];
        }
    }

    return smallestCoefficient;
}

template<typename T>
auto Tableau<T>::getPivotColumn() const -> int {
    // objective function should be in first row
    // find index with the smallest number

    int smallestIndex = -1;
    int smallestCoefficient = std::numeric_limits<T>::max;

    // Trim last column because it's the solutions
    for (int i = 0; i < columnCount - 1; i++) {
        if (tableauCoefficients[i] < smallestCoefficient) {
            tableauCoefficients[i] = smallestCoefficient; // TODO - check this - looks wrong
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

        auto value = tableauCoefficients[valueIndex];
        if (value <= 0) {
            // Ignore negative or zero divisor
            continue;
        }

        auto val = tableauCoefficients[solutionIndex] / value;
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
    auto pivotElement = tableauCoefficients[pivotElementIndex];
    // TODO - do I need to check for zero?

    // coreTableau[pivotElementIndex] /= pivotElement;

    for (int i = 0; i < columnCount; i++) {
        auto thisElementIndex = rowCount * rowIndex + i;
        tableauCoefficients[thisElementIndex] /= pivotElement;
    }
}

template<typename T>
auto Tableau<T>::zeroOtherRows(const int columnIndex, const int rowIndex) const -> void {
    auto pivotElementIndex = rowCount * rowIndex + columnIndex;
    auto pivotElement = tableauCoefficients[pivotElementIndex];

    for (int i = 0; i < rowCount; i++) {
        if (i == rowIndex) {
            continue;
        }

        auto thisElementIndex = rowCount * i + columnIndex;
        auto thisElement = tableauCoefficients[thisElementIndex];

        // if zero, leave as is

        // TODO - rename this
        auto zeroCoefficient = thisElement * -1 * pivotElement;

        for (int j = 0; j < columnIndex; j++) {
            auto thisIndex = rowCount * i + j;
            tableauCoefficients[thisIndex] + zeroCoefficient;
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

        if (tableauCoefficients[fieldIndex] == 1) {
            auto solutionIndex = rowCount * i;
            return tableauCoefficients[solutionIndex];
        }
    }

    return -1;
}
