//
// Created by neb on 04/03/2026.
//

#include <gtest/gtest.h>

#include "../cmake-build-debug/_deps/googletest-src/googlemock/include/gmock/gmock-matchers.h"
#include "../simplex/Simplex.h"
#include "../data_types/Overloads.h"

template <typename T>
static auto VariableNear(T coefficient, std::string symbol, T eps = static_cast<T>(1e-3)) {
    return testing::AllOf(
        testing::Property(&Variable<T>::getSymbol, symbol),
        testing::Property(&Variable<T>::getCoefficient, testing::DoubleNear(coefficient, eps))
    );
}

TEST(Simplex, TahaProblems3_10) {
    const auto z = objectiveVar("z");
    const auto x1 = var("x1");
    const auto x2 = var("x2");

    const auto objective = 2.0 * x1 + 3.0 * x2== z;

    const std::vector constraints {
        x1 + 3.0 * x2 <= 12.0,
        3.0 * x1 + 2.0 * x2 <= 12.0,
    };

    auto simplex = Simplex(Maximise, objective, constraints);

    EXPECT_THAT(simplex.solve(), testing::UnorderedElementsAre(
        VariableNear(13.7143, z.getSymbol()), // 96 / 7
        VariableNear(1.7143, x1.getSymbol()), // 12 / 7
        VariableNear(3.4286, x2.getSymbol()) // 24 / 7
    ));
}

// Key example for testing the 2-phase method
TEST(Simplex, TahaExample3_4_1) {
    const auto z = objectiveVar("z");
    const auto x1 = var("x1");
    const auto x2 = var("x2");

    const auto objective = 4.0 * x1 + x2 == z;

    const std::vector constraints {
        3.0 * x1 + x2 == 3.0,
        4.0 * x1 + 3.0 * x2 >= 6.0,
        x1 + 2.0 * x2 <= 4.0,
    };

    auto simplex = Simplex(Minimise, objective, constraints);

    EXPECT_THAT(simplex.solve(), testing::UnorderedElementsAre(
        3.4 * z,
        0.4 * x1,
        1.8 * x2
    ));
}

TEST(Simplex, TahaCases2_2_2) {
    const auto z = objectiveVar("z");
    const auto x1 = var("x1");
    const auto x2 = var("x2");

    const auto objective = 0.3 * x1 + 0.9 * x2 == z;

    const std::vector constraints {
        x1 + x2 >= 800.0,
        .21 * x1 - .3 * x2 <= 0.0,
        .03 * x1  - .01 * x2 >= 0.0,
    };

    auto simplex = Simplex(Minimise, objective, constraints);

    EXPECT_THAT(simplex.solve(), testing::UnorderedElementsAre(
        VariableNear(437.647, z.getSymbol()),
        VariableNear(470.588, x1.getSymbol()),
        VariableNear(329.412, x2.getSymbol())
    ));
}

TEST(Simplex, TahaCases3_2_1) {
    const auto z = objectiveVar("z");
    const auto x1 = var("x1");
    const auto x2 = var("x2");

    const auto objective = 2.0 * x1 + 3.0 * x2 == z;

    const std::vector constraints {
        2.0 * x1 + x2 <= 4.0,
        x1 + 2.0 * x2 <= 5.0,
    };

    auto simplex = Simplex(Maximise, objective, constraints);

    EXPECT_THAT(simplex.solve(), testing::UnorderedElementsAre(
        8.0 * z,
        1.0 * x1,
        2.0 * x2
    ));
}

TEST(Simplex, LibreTextsExample4_2_1) {
    const auto z = objectiveVar("z");
    const auto x1 = var("x1");
    const auto x2 = var("x2");

    const auto objective = 40.0 * x1 + 30.0 * x2 == z;

    const std::vector constraints = {
        x1 + x2 <= 12.0,
        2.0 * x1 + x2 <= 16.0
    };

    auto simplex = Simplex(Maximise, objective, constraints);

    EXPECT_THAT(simplex.solve(), testing::UnorderedElementsAre(
        400.0 * z,
        4.0 * x1,
        8.0 * x2
    ));
}
