//
// Created by neb on 22/02/2026.
//

#ifndef LINEAR_PROGRAMMING_EXPRESSION_H
#define LINEAR_PROGRAMMING_EXPRESSION_H
#include <map>
#include <optional>
#include <vector>

#include "Variable.h"
#include "EnumTypes.h"

template <typename T>
class Constraint;

template <typename T>
class Expression {
public:
    explicit Expression(const Variable<T> var) {
        addVar(var);
    }

    explicit Expression(const std::vector<Variable<T>> otherExpressions) {
        for (const auto &var : otherExpressions) {
            addVar(var);
        }
    }

    auto operator+(const Expression expr) -> Expression {        
        merge(expr);

        return *this;
    }

    auto operator+(const Variable<T> var) -> Expression {
        addVar(var);

        return *this;
    }

    auto operator-(const Expression &expr) -> Expression {
        merge(expr);

        return *this;
    }

    auto operator-(const Variable<T> var) -> Expression {
        addVar(var);

        return *this;
    }

    auto operator<(Variable<T> var) -> Constraint<T> {
        return Constraint<T>(*this, var, lt);
    }

    auto operator<=(Variable<T> var) -> Constraint<T> {
        return Constraint<T>(*this, var, lte);
    }

    auto operator>(Variable<T> var) -> Constraint<T> {
        return Constraint<T>(*this, var, gt);
    }

    auto operator>=(Variable<T> var) -> Constraint<T> {
        return Constraint<T>(*this, var, gte);
    }

    auto operator==(Variable<T> var) -> Constraint<T> {
        return Constraint<T>(*this, var, eq);
    }

    [[nodiscard]] auto getSize() const -> size_t {
        return variables.size();
    }

    [[nodiscard]] auto findVariableBySymbol(const std::string &symbol) const -> std::optional<const Variable<T>*> {
        if (!symbolMap.contains(symbol)) {
            return std::nullopt;
        }

        return &variables[symbolMap.at(symbol)];
    }

    auto removeVariableBySymbol(const std::string &symbol) -> void {
        const auto symbolMapIndex = symbolMap.at(symbol);
        const auto variableIndex = idMap.at(symbolMapIndex);

        symbolMap.erase(symbol);
        variables.erase(variables.begin() + variableIndex);
        idMap.erase(symbolMapIndex);
    }

    [[nodiscard]] auto findVariableById(const int id) const -> std::optional<Variable<T>> {
        if (!idMap.contains(id)) {
            return std::nullopt;
        }

        return variables[idMap.at(id)];
    }

    [[nodiscard]] auto getInnerVariables() const -> std::vector<Variable<T>> {
        return variables;
    }

    [[nodiscard]] auto getInnerMap() const -> std::unordered_map<int, int> {
        return idMap;
    }

    auto flipAllSigns() -> void {
        for (auto& expr : variables) {
            expr *= -1;
        }
    }

private:
    std::vector<Variable<T>> variables = {};
    std::unordered_map<std::string, int> symbolMap = {};
    std::unordered_map<int, int> idMap = {};
    
    auto addVar(const Variable<T> var) -> void {
        if (symbolMap.contains(var.getSymbol())) {
            return;
        }

        if (idMap.contains(var.getId())) {
            return;
        }

        variables.push_back(var);

        const auto varIndex = variables.size() - 1;
        idMap.emplace(var.getId(), varIndex);
        symbolMap.emplace(var.getSymbol(), varIndex);
    }
    
    auto merge(const Expression expr) -> void {
        std::map<std::string, Variable<T>> symbols;
        for (const auto &var : variables) {
            symbols.emplace(var.getSymbol(), var);
        }

        for (auto otherVar : expr.getInnerVariables()) {
            if (symbols.contains(otherVar.getSymbol())) {
                auto var = symbols.at(otherVar.getSymbol());
                var += otherVar;
            } else {
                addVar(otherVar);
            }
        }
    }
};

template <typename T>
auto operator*(int coefficient, Variable<T> var) -> Expression<T> {
    return Expression<T>(coefficient, var);
}

// Deduction guide
template <typename T>
Expression(Variable<T>) -> Expression<T>;


#endif //LINEAR_PROGRAMMING_EXPRESSION_H
