//
// Created by neb on 23/02/2026.
//

#ifndef LINEAR_PROGRAMMING_ENUMTYPES_H
#define LINEAR_PROGRAMMING_ENUMTYPES_H

enum ObjectiveType {
    Minimise,
    Maximise,
};

enum EqualityType {
    lt,
    lte,
    gt,
    gte,
    eq,
    neq,
};

enum VarKind {
    Var,
    Objective,
    Solution,
    Slack,
    Artificial,
    Surplus,
};

enum SolutionStatus {
    Incomplete,
    Optimal,
    Degenerate,
    Infeasible,
};

#endif //LINEAR_PROGRAMMING_ENUMTYPES_H
