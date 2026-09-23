//
// Created by neb on 22/02/2026.
//

#ifndef LINEAR_PROGRAMMING_CONSTRAINT_H
#define LINEAR_PROGRAMMING_CONSTRAINT_H
#include "EnumTypes.h"
#include "Expression.h"
#include "Variable.h"

template <typename T>
class Constraint {
public:
    Constraint(
        Expression<T> expr,
        Variable<T> constraintVariable,
        const EqualityType equality)
            : expression(expr), constraintVariable(constraintVariable), equality(equality) { }

    [[nodiscard]] auto isEquation() const -> bool {
        if (equality == eq) {
            return true;
        }

        bool hasCoefficientOfOne = false;
        const bool hasFewerThanTwoExpressions = expression.getSize() < 2;

        for (auto &var : expression.getInnerVariables()) {

            // If coefficient is not 1 -> is not equation
            if (var.getCoefficient() != 1) {
                hasCoefficientOfOne = true;
            }
        }

        return !(hasCoefficientOfOne && hasFewerThanTwoExpressions);
    }

    [[nodiscard]] auto isRequiresSlackVariable() const -> bool {
        return equality == lte;
    }

    [[nodiscard]] auto isRequiresSurplusVariable() const -> bool {
        return equality == gte;
    }

    [[nodiscard]] auto isRequiresArtificialVariable() const -> bool {
        return equality == gte || equality == eq;
    }

    auto configureAsObjective() -> void {
        constraintVariable.setKind(Objective);
    }

    [[nodiscard]] auto getExpression() -> Expression<T> {
        return expression;
    }

    [[nodiscard]] auto getEquality() const -> EqualityType {
        return equality;
    }

    [[nodiscard]] auto getVariable() const -> const Variable<T>* {
        return &constraintVariable;
    }

    [[nodiscard]] auto getSymbolsOfKinds(const std::set<VarKind> &kinds) const -> std::unordered_map<std::string, bool> {
        std::unordered_map<std::string, bool> symbols = {};

        for (auto &var : expression.getInnerVariables()) {
            if (kinds.contains(var.getKind())) {
                symbols.try_emplace(var.getSymbol(), true);
            }
        }

        return symbols;
    }

    [[nodiscard]] auto getObjectiveVariable() const -> std::optional<Variable<T>*> {
        return getVariableKind(Objective);
    }

    [[nodiscard]] auto getSolutionVariable() const -> std::optional<Variable<T>*> {
        return getVariableKind(Solution);
    }

    [[nodiscard]] auto findVariable(const std::string &symbol) const -> std::optional<const Variable<T>*> {
        return expression.findVariableBySymbol(symbol);
    }

    auto addVariable(Variable<T> var) -> void {
        expression = expression + Expression<T>(var);
    }

    auto removeVariable(const std::string &symbol) -> void {
        expression.removeVariableBySymbol(symbol);
    }

    auto zeroEquation() -> void {
        expression.flipAllSigns();

        expression + Variable(std::move(constraintVariable));
        equality = eq;
        constraintVariable = Variable<T>({ .coefficient = 0 });
    }

    auto negateExpression() -> void {
        expression.flipAllSigns();
        equality = getConverseOperator(equality);
    }

private:
    Expression<T> expression;
    Variable<T> constraintVariable;
    EqualityType equality;

    [[nodiscard]] auto getVariableKind(const VarKind &kind) const -> std::optional<Variable<T>*> {
        auto vars = expression.getInnerVariables();

        auto it = std::ranges::find_if(
            vars.begin(),
            vars.end(),
            [kind](Variable<T> var) {
                return var.getKind() == kind;
            });
        if (it != vars.end()) {
            return &(*it);
        }

        return std::nullopt;
    }

    [[nodiscard]] static auto getConverseOperator(const EqualityType equality) -> EqualityType {
        switch (equality) {
            case gt:
                return lt;
            case gte:
                return lte;
            case lt:
                return gt;
            case lte:
                return gte;
        }

        return equality;
    }
};


#endif //LINEAR_PROGRAMMING_CONSTRAINT_H
