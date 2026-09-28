//
// Created by neb on 23/02/2026.
//

#ifndef LINEAR_PROGRAMMING_TABLEAU_H
#define LINEAR_PROGRAMMING_TABLEAU_H
#include <memory>
#include <vector>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <format>
#include <algorithm>

#include "../data_types/Expression.h"
#include "../data_types/VarList.h"


template<typename T>
class Tableau {
public:
    Tableau(
        const ObjectiveType objectiveType,
        Constraint<T> objectiveFunction,
        std::vector<Constraint<T>> &constraints
    ) : primaryObjective(objectiveFunction),
        objectiveType(objectiveType) {
        reformulateObjective(primaryObjective);

        if (objectiveType == Minimise) {
            primaryObjective.negateExpression();
        }

        extractVariables(primaryObjective.getExpression());

        addSlacksToConstraints(constraints);
        subtractSurplusesFromConstraints(constraints);

        // Create the RHS var
        extractVariables(Expression<T>(Variable<T>({.symbol = "RHS", .kind = Solution})));

        initTableau(primaryObjective, constraints);
    }

    auto initTableau(Constraint<T> &objective, std::vector<Constraint<T>> &constraints) -> void {
        columnCount = static_cast<int>(vars.size());
        rowCount = static_cast<int>(constraints.size()) + 1;
        tableauCoefficients = std::vector<T>(columnCount * rowCount);

        modifyTableauObjectiveRow(objective);

        for (int i = columnCount; i < tableauCoefficients.size(); i++) {
            auto constraintIndex = std::floor(i / columnCount) - 1;

            // Last column should be the solution
            if (i % columnCount == columnCount - 1) {
                auto constraintCoefficient = constraints[constraintIndex].getVariable()->getCoefficient();
                tableauCoefficients[i] = constraintCoefficient;

                continue;
            }

            const auto& thisConstraint = constraints[constraintIndex];
            auto varSymbol = vars.at(i % columnCount);

            auto matchingVar = thisConstraint.findVariable(varSymbol);

            // Missing variables can be zeroed
            if (!matchingVar) {
                tableauCoefficients[i] = static_cast<T>(0);
                continue;
            }

            auto thisCoefficient = matchingVar.value()->getCoefficient();
            tableauCoefficients[i] = thisCoefficient;
        }

        auto symbolMap = objective.getSymbolsOfKinds({Var, Slack, Artificial, Surplus});
        for (auto i = 0; i < vars.size(); i++) {
            if (const auto var = vars.at(i); !symbolMap.contains(var)) {
                continue;
            }

            if (!isColumnBasic(i)) {
                continue;
            }

            const auto basicRowIndex = getBasicRowIndex(i);
            if (basicRowIndex < 0) {
                continue;
            }

            eliminateRow(0, i, basicRowIndex);
        }
    }

    auto modifyTableauObjectiveRow(Constraint<T> &objective) -> void {
        // Make the first objective result row
        for (int i = 0; i < vars.size(); i++) {
            auto var = objective.findVariable(vars.at(i));
            if (!var) {
                tableauCoefficients[i] = 0;
                continue;
            }

            tableauCoefficients[i] = var.value()->getCoefficient();
        }
    }

    auto initPhaseOne(std::vector<Constraint<T> > &constraints) -> void {
        const auto artificialVariables = addArtificialsToConstraints(constraints);

        // Create the phase one objective
        if (artificialVariables.size() > 0) {
            auto expr = Expression<T>(artificialVariables[0]);

            if (artificialVariables.size() > 1) {
                for (int i = 1; i < artificialVariables.size(); i++) {
                    expr = expr + artificialVariables[i];
                }
            }

            phaseOneObjective = expr == 0.0;
            if (!phaseOneObjective) {
                return;
            }

            reformulateObjective(phaseOneObjective.value());
            extractVariables(expr);
        }

        initTableau(phaseOneObjective.value(), constraints);
    }

    auto prepareForPhaseTwo(std::vector<Constraint<T>> &constraints) -> void {
        removeArtificialVariables(constraints);

        for (size_t i = 0; i < columnCount - 1; i++) {
            auto varSymbol = vars.at(i % columnCount);

            auto matchingVar = primaryObjective.findVariable(varSymbol);
            if (!matchingVar) {
                continue;
            }

            const auto coefficient = matchingVar.value()->getCoefficient();
            tableauCoefficients[i] = coefficient;
        }
    }

    auto rebalanceBasicVariables() -> void {
        for (auto i = 0; i < vars.size() - 1; i++) {
            if (!isColumnBasic(i)) {
                continue;
            }

            const auto basicRowIndex = getBasicRowIndex(i);
            if (basicRowIndex < 0) {
                continue;
            }

            if (std::abs(tableauCoefficients[i]) > std::numeric_limits<T>::epsilon()) {
                eliminateRow(0, i, basicRowIndex);
            }
        }
    }

    auto addSlacksToConstraints(std::vector<Constraint<T> > &constraints) -> void {
        for (auto i = 1; auto &constraint: constraints) {
            if (constraint.isRequiresSlackVariable()) {
                constraint.addVariable(Variable<T>({.symbol = std::format("s{}", i), .kind = Slack}));
            }

            extractVariables(constraint.getExpression());
            i++;
        }
    }

    auto subtractSurplusesFromConstraints(std::vector<Constraint<T> > &constraints) -> void {
        for (auto i = 1; auto &constraint: constraints) {
            if (!constraint.isRequiresSurplusVariable()) {
                continue;
            }

            constraint.addVariable(Variable<T>({.coefficient = -1, .symbol = std::format("y{}", i), .kind = Surplus}));
            extractVariables(constraint.getExpression());
            i++;
        }
    }

    auto addArtificialsToConstraints(std::vector<Constraint<T> > &constraints) const -> std::vector<Variable<T> > {
        std::vector<Variable<T> > addedVariables = {};

        for (auto i = 1; auto &constraint: constraints) {
            if (!constraint.isRequiresArtificialVariable()) {
                continue;
            }

            const auto symbol = std::format("a{}", i);
            const auto var = Variable<T>({.symbol = std::move(symbol), .kind = Artificial});
            constraint.addVariable(var);
            addedVariables.push_back(var);
            i++;
        }

        return addedVariables;
    }

    [[nodiscard]] auto isSolved() const -> bool {
        return !isNegativeValueInObjectiveRow();
    }

    [[nodiscard]] auto isNegativeValueInObjectiveRow() const -> bool {
        const auto epsilon = std::numeric_limits<T>::epsilon();
        // Exclude RHS
        for (int i = 0; i < columnCount - 2; i++) { // TODO - make this -2 nicer
            if (tableauCoefficients[i] < -epsilon) {
                return true;
            }
        }

        return false;
    }

    [[nodiscard]] auto isPhaseOneSolved() const -> SolutionStatus {
        // All non-artificial coefficients are zero
        // All artificial variables are non-basic

        if (!isOptimal()) return Incomplete;

        const auto rhs = tableauCoefficients[columnCount - 1];
        const auto epsilon = std::numeric_limits<T>::epsilon();
        if (tableauCoefficients[rhs] <= epsilon) {
            return Optimal;
        } else {
            // TODO - handle infeasible + degenerate cases
            return Infeasible;
        }

        return Incomplete;
    }

    [[nodiscard]] auto isOptimal() const -> bool {
        auto isOptimal = true;
        const auto epsilon = std::numeric_limits<T>::epsilon();
        // Exclude RHS
        for (int i = 0; i < columnCount - 1; i++) {
            if (tableauCoefficients[i] > epsilon) {
                isOptimal = false;
                break;
            }
        }

        return isOptimal;
    }

    [[nodiscard]] auto isPhaseOneValid(const std::vector<int> &artificialColumns) const -> bool {
        // All artificial variables must be non-basic
        for (const size_t columnIndex : artificialColumns) {
            if (!isColumnBasic(columnIndex)) {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] auto getFinalObjective() const -> std::vector<Variable<T> > {
        std::vector<Variable<T> > finalVariables = {};

        for (auto i = 0; i < vars.size() - 1; i++) {
            const auto thisVarIndex = i % (columnCount - vars.size() - 1);
            auto varSymbol = vars.at(thisVarIndex);

            const auto isObjective = vars.kindAt(thisVarIndex) == Objective;
            if (isObjective) {
                // Get objective RHS
                auto solution = tableauCoefficients[columnCount - 1];
                solution = objectiveType == Minimise
                    ? solution * -1
                    : solution;

                finalVariables.push_back(Variable<T>({
                    .coefficient = solution,
                    .symbol = varSymbol
                }));

                continue;
            }

            if (!isObjective && !primaryObjective.findVariable(varSymbol)) {
                continue;
            }

            if (!isColumnBasic(i, true) && isObjective) {
                continue;
            }

            for (int rowIndex = 0; rowIndex < rowCount; rowIndex++) {
                auto elementIndex = rowIndex * columnCount + i;
                auto coefficient = tableauCoefficients[elementIndex];

                if (coefficient == 1) {
                    auto solutionIndex = rowIndex * columnCount + columnCount - 1;
                    auto solution = tableauCoefficients[solutionIndex];

                    finalVariables.push_back(Variable<T>({
                        .coefficient = solution,
                        .symbol = varSymbol
                    }));

                    break;
                }
            }
        }

        return finalVariables;
    }

    auto pivot(const size_t pivotColumnIndex) -> void {
        if (pivotColumnIndex == -1) {
            return;
        }

        const auto pivotRowIndex = getPivotRow(pivotColumnIndex);
        if (pivotRowIndex == -1) {
            return;
        }

        handlePivotRow(pivotRowIndex, pivotColumnIndex, pivotRowIndex);
        for (size_t i = 0; i < rowCount; i++) {
            if (i == pivotRowIndex) {
                continue;
            }

            eliminateRow(i, pivotColumnIndex, pivotRowIndex);
        }
    }

    [[nodiscard]] auto getMinimisedPivotColumn() -> size_t {
        size_t largestIndex = -1;
        T largestCoefficient = static_cast<T>(std::numeric_limits<T>::min());

        for (size_t i = 0; i < columnCount - 1; i++) {
            if (tableauCoefficients[i] > largestCoefficient) {
                largestIndex = i;
                largestCoefficient = tableauCoefficients[i];
            }
        }

        return largestIndex;
    }

    auto print() const -> void {
        if (tableauCoefficients.empty() || vars.empty()) return;

        const int cols = static_cast<int>(vars.size());
        const int rows = static_cast<int>(tableauCoefficients.size()) / cols;

        // Determine column widths: max of header length or formatted value length
        constexpr int precision = 4;
        std::vector<int> colWidths(cols);

        for (int j = 0; j < cols; ++j) {
            colWidths[j] = static_cast<int>(vars.at(j).size());
            for (int i = 0; i < rows; ++i) {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(precision) << tableauCoefficients[i * cols + j];
                colWidths[j] = std::max(colWidths[j], static_cast<int>(oss.str().size()));
            }
            colWidths[j] += 2; // padding
        }

        // Header
        for (int j = 0; j < cols; ++j)
            std::cout << std::setw(colWidths[j]) << std::right << vars.at(j);
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

    // Any instance of == or >= in a constraint means that the two-phase method is required
    [[nodiscard]] auto isRequiresTwoPhase(const std::vector<Constraint<T>> &constraints) const -> bool {
        for (auto &constraint: constraints) {
            const auto equality = constraint.getEquality();

            if (equality == eq || equality == gte) {
                return true;
            }
        }

        return false;
    }

    [[nodiscard]] auto getPivotColumn() const -> size_t {
        size_t smallestIndex = -1;
        T smallestCoefficient = static_cast<T>(std::numeric_limits<T>::max());

        for (size_t i = 0; i < columnCount - 2; i++) {
            if (tableauCoefficients[i] < smallestCoefficient) {
                smallestIndex = i;
                smallestCoefficient = tableauCoefficients[i];
            }
        }

        return smallestIndex;
    }

private:
    VarList vars {};
    std::vector<T> tableauCoefficients;

    Constraint<T> primaryObjective;
    std::optional<Constraint<T>> phaseOneObjective;

    size_t columnCount;
    size_t rowCount;
    ObjectiveType objectiveType;

    auto handlePivotRow(const size_t thisRowIndex, const size_t pivotColumnIndex, const size_t pivotRowIndex) -> void {
        T pivotElement = tableauCoefficients[pivotRowIndex * columnCount + pivotColumnIndex];

        for (auto i = 0; i < columnCount; i++) {
            auto columnIndex = thisRowIndex * columnCount + i;
            tableauCoefficients[columnIndex] /= pivotElement;
        }
    }

    auto eliminateRow(const size_t thisRowIndex, const size_t pivotColumnIndex, const size_t pivotRowIndex) -> void {
        const auto coefficient = tableauCoefficients[thisRowIndex * columnCount + pivotColumnIndex];

        if (coefficient == 0) return;

        for (auto i = 0; i < columnCount; i++) {
            auto thisElementIndex = thisRowIndex * columnCount + i;
            auto targetElementIndex = pivotRowIndex * columnCount + i;
            auto thisCoefficient = tableauCoefficients[thisElementIndex];

            tableauCoefficients[thisElementIndex] = thisCoefficient - coefficient * tableauCoefficients[targetElementIndex];
        }
    }

    static auto reformulateObjective(Constraint<T> &objectiveFunction) -> void {
        objectiveFunction.zeroEquation();
    }

    auto extractVariables(const Expression<T> &expr) -> void {
        // TODO - handle multiple instances in the same expression

        for (auto &var: expr.getInnerVariables()) {
            vars.extractVariable(var);
        }
    }

    [[nodiscard]] auto isColumnBasic(const size_t columnIndex, const bool includeObjective = false) const -> bool {
        int oneCount = 0;
        const size_t startingRowIndex = includeObjective
            ? 0
            : 1;
        const auto epsilon = std::numeric_limits<T>::epsilon();

        for (size_t i = startingRowIndex; i < rowCount; i++) {
            const auto coefficient = tableauCoefficients[columnCount * i + columnIndex];

            // Is one
            if (std::abs(coefficient - 1) <= epsilon) {
                oneCount++;

                if (oneCount > 1) {
                    return false;
                }
            } else {
                // Not zero
                if (std::abs(coefficient) > epsilon) {
                    return false;
                }
            }
        }

        return oneCount == 1;
    }

    // Given a basic column, return the row index
    [[nodiscard]] auto getBasicRowIndex(const size_t columnIndex) const -> size_t {
        // Skip the first objective row
        for (size_t i = 1; i < rowCount; i++) {
            auto thisColumnIndex = i * columnCount + columnIndex;
            auto thisCoefficient = tableauCoefficients[thisColumnIndex];

            if (thisCoefficient == 1) {
                return i;
            }

            if (thisCoefficient != 0) {
                return -1;
            }
        }

        return -1;
    }

    [[nodiscard]] auto getPivotRow(const size_t columnIndex) const -> size_t {
        size_t smallestRowIndex = -1;
        auto smallestResultColumnValue = static_cast<T>(std::numeric_limits<T>::max());

        for (size_t i = 1; i < rowCount; i++) {
            auto valueIndex = columnCount * i + columnIndex;
            auto solutionIndex = columnCount * i + columnCount - 1;

            auto value = tableauCoefficients[valueIndex];
            if (value <= 0) {
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

    auto divideRowByValue(T value, const size_t rowIndex) -> void {
        if (value == 0) {
            return;
        }

        const auto startIndex = rowIndex * columnCount;
        const auto endIndex = rowIndex * columnCount + columnCount;

        for (size_t i = startIndex; i < endIndex; i++) {
            tableauCoefficients[i] /= value;
        }
    }

    auto divideRowByPivotRow(const size_t rowIndex, const size_t pivotRowIndex) -> void {
        auto currentRowIndex = rowIndex * columnCount;
        auto currentPivotRowIndex = pivotRowIndex * columnCount;

        for (size_t i = currentRowIndex; i < columnCount; i++) {
            auto currentPivotRowElement = tableauCoefficients[currentPivotRowIndex];

            if (currentPivotRowElement != 0) {
                tableauCoefficients[currentRowIndex] /= currentPivotRowElement;
            }

            currentRowIndex++;
            currentPivotRowIndex++;
        }
    }

    auto zeroOtherElementsInColumn(const size_t columnIndex, const size_t rowIndex) -> void {
        auto pivotElementIndex = columnCount * rowIndex + columnIndex;
        auto pivotElement = tableauCoefficients[pivotElementIndex];

        for (size_t i = 0; i < rowCount; i++) {
            if (i == rowIndex) {
                continue;
            }

            auto thisElementIndex = rowCount * i + columnIndex;
            auto thisElement = tableauCoefficients[thisElementIndex];

            auto zeroCoefficient = thisElement * -1 * pivotElement;

            for (size_t j = 0; j < columnIndex; j++) {
                auto thisIndex = rowCount * i + j;
                tableauCoefficients[thisIndex] + zeroCoefficient;
            }
        }
    }

    [[nodiscard]] auto getBasicColumnSolution(const size_t columnIndex) const -> T {
        for (size_t i = 0; i < rowCount; i++) {
            auto fieldIndex = i * rowCount + columnIndex;

            if (tableauCoefficients[fieldIndex] == 1) {
                auto solutionIndex = rowCount * i;
                return tableauCoefficients[solutionIndex];
            }
        }

        return -1;
    }

    auto removeArtificialVariables(std::vector<Constraint<T>> &constraints) -> void {
        auto artificialIndices = vars.getIndicesOfKind(Artificial);

        removeTableauColumnIndices(artificialIndices);
        vars.removeAtIndices(artificialIndices);
    }

    auto removeTableauColumnIndices(const std::vector<size_t> &indices) -> void {
        std::vector<bool> keepColumns(columnCount, true);
        for (const auto columnIndex: indices) {
            if (columnIndex >= 0 && columnIndex < columnCount) {
                keepColumns[columnIndex] = false;
            }
        }

        int newColumnCount = 0;
        for (const bool keep : keepColumns) {
            if (keep) { newColumnCount++; }
        }

        if (newColumnCount == columnCount) {
            return;
        }

        size_t writeIndex = 0;
        for (size_t r = 0; r < rowCount; r++) {
            for (size_t c = 0; c < columnCount; c++) {
                size_t readIndex = static_cast<size_t>(r) * columnCount + c;

                if (keepColumns[c]) {
                    if (writeIndex != readIndex) {
                        tableauCoefficients[writeIndex] = tableauCoefficients[readIndex];
                    }

                    writeIndex++;
                }
            }
        }

        tableauCoefficients.resize(writeIndex);
        columnCount = newColumnCount;
    }
};


#endif //LINEAR_PROGRAMMING_TABLEAU_H
