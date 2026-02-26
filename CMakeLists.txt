cmake_minimum_required(VERSION 4.1)
project(linear_programming)

set(CMAKE_CXX_STANDARD 23)

add_executable(linear_programming main.cpp
        simplex/Simplex.h
        simplex/Simplex.cpp
        data_types/Constraint.cpp
        data_types/Constraint.h
        data_types/Variable.h
        data_types/Expression.h
        data_types/EnumTypes.h
        simplex/Tableau.cpp
        simplex/Tableau.h
        data_types/Primitive.h
        data_types/Value.h)
