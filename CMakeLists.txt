cmake_minimum_required(VERSION 4.1)
project(linear_programming)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        tests
        tests/hello_test.cpp
        tests/simplex.cpp
)

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

target_link_libraries(
        tests
        GTest::gtest_main
        gmock gtest
)

include(GoogleTest)
gtest_discover_tests(tests
        DISCOVERY_MODE PRE_TEST
)
