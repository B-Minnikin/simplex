cmake_minimum_required(VERSION 4.1)
project(linear_programming)

set(CMAKE_CXX_STANDARD 23)

add_executable(linear_programming main.cpp
        simplex/Simplex.h
        simplex/Simplex.cpp
        simplex/Constraint.cpp
        simplex/Constraint.h
        simplex/Variable.cpp
        simplex/Variable.h
        simplex/Expression.cpp
        simplex/Expression.h)
