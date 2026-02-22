//
// Created by neb on 22/02/2026.
//

#ifndef LINEAR_PROGRAMMING_VARIABLE_H
#define LINEAR_PROGRAMMING_VARIABLE_H

static int nextFreeId = 0;

template <typename T>

class Variable {
public:
    explicit Variable(T var) : var(var) {
        id = nextFreeId++;
    };

    auto operator<(const Variable& other) const -> bool {
        return id < other.id;
    }

private:
    int id;
    T var;
};


#endif //LINEAR_PROGRAMMING_VARIABLE_H
