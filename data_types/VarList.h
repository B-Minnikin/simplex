//
// Created by neb on 16/05/2026.
//

#ifndef LINEAR_PROGRAMMING_VARLIST_H
#define LINEAR_PROGRAMMING_VARLIST_H
#include <string>
#include <vector>

#include "EnumTypes.h"
#include "../data_types/Variable.h"

struct VarList {
    std::vector<std::string> vars {};

    [[nodiscard]] auto size() const -> size_t { return vars.size(); }
    [[nodiscard]] auto empty() const -> bool { return vars.empty(); }

    auto insert(std::string var, const VarKind kind) -> void {
        switch (kind) {
            case Solution: {
                if (hasSolution) {
                    std::cout << "Vars already has a solution: " << var << "\n";
                    return;
                }

                vars.push_back(var);
                hasSolution = true;
                break;
            }
            case Objective: {
                if (hasObjective) {
                    std::cout << "Vars already has an objective: " << var << "\n";
                    return;
                }

                const auto offset = hasSolution
                    ? 1 : 0;
                vars.insert(vars.end() - offset, std::move(var));
                hasObjective = true;
                break;
            }
            default:
                int offset = 0;
                if (hasSolution) {
                    offset++;
                }
                if (hasObjective) {
                    offset++;
                }

                vars.insert(vars.end() - offset, std::move(var));
                break;
        }
    }

    template<typename T>
    auto extractVariable(Variable<T> &var) -> void {
        const auto varSymbol = var.getSymbol();
        auto it = std::find_if(vars.begin(), vars.end(), [varSymbol](const std::string &symbol) -> bool {
            return symbol == varSymbol;
        });

        if (it == vars.end()) {
            this->insert(varSymbol, var.getKind());
        }
    }

    auto removeAtIndices(const std::vector<int> &indices) -> void {
        for (const auto index : indices) {
            if (index >= vars.size()) {
                continue;
            }

            if (vars.size() == index - 1) {
                hasSolution = false;
            }

            if (vars.size() == index - 2) {
                hasObjective = false;
            }

            vars.erase(vars.begin() + index);
        }
    }

    [[nodiscard]] auto all() const -> const std::vector<std::string>& {
        return vars;
    }

    [[nodiscard]] auto at(const int index) const -> std::string {
        return vars[index];
    }

private:
    // Solution should be last element
    bool hasSolution = false;

    // Objective should be penultimate element
    bool hasObjective = false;
};

#endif //LINEAR_PROGRAMMING_VARLIST_H
