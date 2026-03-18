//
// Created by neb on 23/02/2026.
//

#ifndef LINEAR_PROGRAMMING_TABLEAU_H
#define LINEAR_PROGRAMMING_TABLEAU_H
#include <memory>
#include <unordered_map>
#include <vector>
#include <cmath>

#include "../data_types/Expression.h"


constexpr int OBJECTIVE_ROW_COUNT = 1;
constexpr int SOLUTION_COLUMN_COUNT = 1;


template <typename T>
class Tableau {
public:
    Tableau(
        const ObjectiveType objectiveType,
        Constraint<T> objectiveFunction,
        std::vector<Constraint<T>> constraints
    ) {
        reformulateObjective(objectiveFunction);

        if (objectiveType == Minimise) {
            objectiveFunction.negateExpression();
        }

        auto slackCount = 0;

        // TODO - include the constraint var?
        extractVariables(objectiveFunction.getExpression());
        primaryVariableCount = vars.size();

        // Create the slack variables
        for (auto i = 1; auto &constraint : constraints) {
            if (constraint.isEquation()) {
                slackCount++;
                // TODO - manage variable symbol clash
                constraint.addVariable(Variable<T>({ .symbol = std::format("s{}", i), .kind = Slack }));
            }

            extractVariables(constraint.getExpression());
            i++;
        }

        // Sort the variables
        auto objectiveVariable = objectiveFunction.getObjectiveVariable();
        if (!objectiveVariable) {
            // TODO
            return;
        }
        std::string symbol = objectiveVariable.value()->getSymbol();
        std::stable_partition(vars.begin(), vars.end(), [&symbol](const std::string &v) { return v != symbol; });

        // Create the RHS var
        extractVariables(Expression<T>(Variable<T>({ .symbol = "RHS", .kind = Solution })));

        columnCount = static_cast<int>(vars.size());
        rowCount = OBJECTIVE_ROW_COUNT + static_cast<int>(constraints.size());
        tableauCoefficients = std::vector<T>(columnCount * rowCount);

        // Make the first objective result row
        for (int i = 0; i < vars.size(); i++) {
            auto var = objectiveFunction.findVariable(vars[i]);
            if (!var) {
                tableauCoefficients[i] = 0;
                continue;
            }

            tableauCoefficients[i] = var.value()->getCoefficient();
        }

        for (int i = columnCount; i < tableauCoefficients.size(); i++) {
            auto constraintIndex = std::floor(i / columnCount) - 1;

            // Last should be the solution
            if (i % columnCount == columnCount - 1) {
                auto constraintCoefficient = constraints[constraintIndex].getVariable()->getCoefficient();
                tableauCoefficients[i] = constraintCoefficient;

                continue;
            }

            // auto var = vars[constraintIndex];
            // auto varIndex = vars[i % columnCount];
            auto varSymbol = vars[i % columnCount];
            // auto varId = var.getId();

            auto thisConstraint = constraints[constraintIndex];
            auto matchingVar = thisConstraint.findVariable(varSymbol);

            // Missing variables can be zeroed
            if (!matchingVar) {
                tableauCoefficients[i] = static_cast<T>(0);
                continue;
            }

            auto thisCoefficient = matchingVar.value()->getCoefficient();
            tableauCoefficients[i] = thisCoefficient;
        }
    }

    [[nodiscard]] auto isSolved() const -> bool {
        return getSmallestObjectiveCoefficient() >= 0;
    }

    [[nodiscard]] auto getFinalObjective() const -> std::vector<Variable<T>> {
        std::vector<Variable<T>> finalVariables = {};

        for (int primaryColumnIndex = 0; primaryColumnIndex < primaryVariableCount; primaryColumnIndex++) {
            const auto thisVarIndex = primaryColumnIndex % (columnCount - primaryVariableCount);
            auto thisVarSymbol = vars[thisVarIndex];

            auto oneCount = 0;

            for (int rowIndex = 0; rowIndex < rowCount; rowIndex++) {
                auto elementIndex = rowIndex * columnCount + primaryColumnIndex;
                auto coefficient = tableauCoefficients[elementIndex];


                if (coefficient == 1) {
                    oneCount += 1;

                    auto solutionIndex = rowIndex * columnCount + columnCount - 1;
                    auto solution = tableauCoefficients[solutionIndex];

                    finalVariables.push_back(Variable<T>({
                        .coefficient = solution,
                        .symbol = thisVarSymbol
                    }));
                }

                if (oneCount > 1) {
                    // TODO - warn
                    break;
                }
            }
        }

        return finalVariables;
    }

    auto pivot() -> void {

        // column is basic if only one coefficient is 1 and the rest are zero

        // select smallest coefficient from objective row
        // this is the pivot

        // divide each row equation value by pivot coefficient
        // select smallest -> coefficient is pivot element

        const auto pivotColumnIndex = getPivotColumn();
        if (pivotColumnIndex == -1) {
            // TODO - implement
            // We should have reached the most optimal solution already

            return;
        }

        const auto pivotRowIndex = getPivotRow(pivotColumnIndex);
        if (pivotRowIndex == -1) {
            // TODO - implement

            return;
        }

        handlePivotRow(pivotRowIndex, pivotColumnIndex, pivotRowIndex);
        for (auto i = 0; i < rowCount; i++) {
            if (i == pivotRowIndex) {
                continue;
            }

            handleRow(i, pivotColumnIndex, pivotRowIndex);
        }

        // TODO - iterate the other rows and calculate
        // for (int i = 1; i < rowCount; i++) {
        //     for (int j = 0; j < columnCount; j++) {
        //
        //     }
        //     const auto columnIndex = 0;
        //     const auto rowIndex = i;
        //
        //     if (rowIndex == pivotRowIndex) {
        //         continue;
        //     }
        //
        //     performPivot(columnIndex, rowIndex);
        // }

        // TODO - perform pivot on row
        // TODO - perform pivot on other rows
        // TODO - loop around again and check for lowest coefficient from objective function
        // if no negative values found: solution found
        // return a new objective function which matches the solution coefficients
    }

    auto printTableau() const -> void {
        if (tableauCoefficients.empty() || vars.empty()) return;

        const int cols = static_cast<int>(vars.size());
        const int rows = static_cast<int>(tableauCoefficients.size()) / cols;

        // Determine column widths: max of header length or formatted value length
        constexpr int precision = 4;
        std::vector<int> colWidths(cols);

        for (int j = 0; j < cols; ++j) {
            colWidths[j] = static_cast<int>(vars[j].size());
            for (int i = 0; i < rows; ++i) {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(precision) << tableauCoefficients[i * cols + j];
                colWidths[j] = std::max(colWidths[j], static_cast<int>(oss.str().size()));
            }
            colWidths[j] += 2; // padding
        }

        // Header
        for (int j = 0; j < cols; ++j)
            std::cout << std::setw(colWidths[j]) << std::right << vars[j];
        std::cout << '\n';

        // Separator
        int totalWidth = 0;
        for (int w : colWidths) totalWidth += w;
        std::cout << std::string(totalWidth, '-') << '\n';

        // Rows
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                std::cout << std::setw(colWidths[j]) << std::right
                          << std::fixed << std::setprecision(precision)
                          << tableauCoefficients[i * cols + j];
            }
            std::cout << '\n';

            // Separate objective function from constraints
            if (i == 0)
                std::cout << std::string(totalWidth, '-') << '\n';
        }

        std::cout << '\n';
    }

private:
    // TODO - split vars from var instances
    // these should be instances
    std::vector<std::string> vars = {};
    std::vector<T> tableauCoefficients;
    std::unordered_map<int, int> varMap = {}; // TODO - remove?

    int primaryVariableCount = 0;
    int columnCount;
    int rowCount;

    auto handlePivotRow(const int thisRowIndex, const int pivotColumnIndex, const int pivotRowIndex) -> void {
        T pivotElement = tableauCoefficients[pivotRowIndex * columnCount + pivotColumnIndex];

        for (auto i = 0; i < columnCount; i++) {
            auto columnIndex = thisRowIndex * columnCount + i;
            tableauCoefficients[columnIndex] /= pivotElement;
        }
    }

    auto handleRow(const int thisRowIndex, const int pivotColumnIndex, const int pivotRowIndex) -> void {
        T pivotElement = tableauCoefficients[pivotRowIndex * columnCount + pivotColumnIndex];

        auto comp = pivotElement * -1;

        auto shadowPivotIndex = thisRowIndex * columnCount + pivotColumnIndex;
        auto thisElement = tableauCoefficients[shadowPivotIndex];
        tableauCoefficients[shadowPivotIndex] += pivotElement * thisElement * -1;

        // To obtain a zero in the entry first above the pivot element, we multiply the second row by -1 and add it to row 1.
        // To obtain a zero in the element below the pivot, we multiply the second row by 40 and add it to the last row.

        for (auto i = 0; i < columnCount; i++) {
            if (i == pivotColumnIndex) {
                // We should have already handled the pivot column
                continue;
            }

            auto pivotRowMatchingIndex = pivotRowIndex * columnCount + i;
            T matchingPivotRowElement = tableauCoefficients[pivotRowMatchingIndex];

            auto columnIndex = thisRowIndex * columnCount + i;
            auto thisElementLoop = tableauCoefficients[columnIndex];

            tableauCoefficients[columnIndex] += matchingPivotRowElement * thisElement * -1;
        }
    }

    // TODO - rework
   static auto reformulateObjective(Constraint<T> &objectiveFunction) -> void {
        objectiveFunction.zeroEquation();
    }

    auto extractVariables(const Expression<T> &expr) -> void {
        // TODO - handle multiple instances in the same expression

        for (auto &var : expr.getInnerVariables()) {
            const auto varSymbol = var.getSymbol();
            auto it = std::find_if(vars.begin(), vars.end(), [varSymbol](const std::string &symbol) -> bool { return symbol == varSymbol; });

            if (it == vars.end()) {
                vars.push_back(varSymbol);
            }
        }
    }

    [[nodiscard]] auto isColumnBasic(int columnIndex) const -> bool {
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

    [[nodiscard]] auto getSmallestObjectiveCoefficient() const -> T {
        // iterate over the objective expression
        // look for smallest number

        auto smallestCoefficient = static_cast<T>(std::numeric_limits<T>::max());

        // Do not include the solution column (last)
        for (int i = 0; i < columnCount - 1; i++) {
            if (tableauCoefficients[i] < smallestCoefficient) {
                smallestCoefficient = tableauCoefficients[i];
            }
        }

        return smallestCoefficient;
    }

    [[nodiscard]] auto getPivotColumn() const -> int {
        // objective function should be in first row
        // find index with the smallest number

        int smallestIndex = -1;
        T smallestCoefficient = static_cast<T>(std::numeric_limits<T>::max());

        // Trim last column because it's the solutions
        for (int i = 0; i < columnCount - 1; i++) {
            if (tableauCoefficients[i] < smallestCoefficient) {
                smallestIndex = i;
                smallestCoefficient = tableauCoefficients[i];
            }
        }

        return smallestIndex;
    }

    [[nodiscard]] auto getPivotRow(const int columnIndex) const -> int {
        // divide each column coefficient by coefficient in the solution column

        auto smallestRowIndex = -1;
        auto smallestResultColumnValue = static_cast<T>(std::numeric_limits<T>::max());

        for (int i = 1; i < rowCount; i++) {
            auto valueIndex = columnCount * i + columnIndex; // ., 1, ., ., 4, ., ., 7, .
            auto solutionIndex = columnCount * i + columnCount - 1; // get row - last item

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

    auto performPivot(const int pivotColumnIndex, const int pivotRowIndex, const int currentRowIndex) -> void {
        // get the pivot element value
        // divide by itself
        // divide all elements in the row by that value

        auto pivotElementIndex = rowCount * pivotRowIndex + pivotColumnIndex;
        auto pivotElement = tableauCoefficients[pivotElementIndex];
        // TODO - do I need to check for zero?

        // coreTableau[pivotElementIndex] /= pivotElement;

        for (int i = 0; i < columnCount; i++) {
            auto thisElementIndex = rowCount * currentRowIndex + i;
            tableauCoefficients[thisElementIndex] /= pivotElement;
        }
    }

    auto divideAllInRow(T value, const int rowIndex) -> void {
        if (value == 0) {
            return;
        }

        const auto startIndex = rowIndex * columnCount;
        const auto endIndex = rowIndex * columnCount + columnCount;

        for (int i = startIndex; i < endIndex; i++) {
            tableauCoefficients[i] /= value;
        }
    }

    auto divideAllInRowByPivotRow(const int thisRowIndex, const int pivotRowIndex) -> void {
        auto currentRowIndex = thisRowIndex * columnCount;
        auto currentPivotRowIndex = pivotRowIndex * columnCount;

        for (int i = currentRowIndex; i < columnCount; i++) {
            auto currentPivotRowElement = tableauCoefficients[currentPivotRowIndex];

            if (currentPivotRowElement != 0) {
                tableauCoefficients[currentRowIndex] /= currentPivotRowElement;
            }

            currentRowIndex++;
            currentPivotRowIndex++;
        }
    }

    auto zeroOtherElementsInColumn(const int columnIndex, const int rowIndex) -> void {
        auto pivotElementIndex = columnCount * rowIndex + columnIndex;
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

    [[nodiscard]] auto getBasicColumnSolution(const int columnIndex) const -> T {
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
};


#endif //LINEAR_PROGRAMMING_TABLEAU_H
