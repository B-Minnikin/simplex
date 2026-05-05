//
// Created by neb on 04/03/2026.
//

#include <gtest/gtest.h>

#include "../cmake-build-debug/_deps/googletest-src/googlemock/include/gmock/gmock-matchers.h"
#include "../simplex/Simplex.h"
#include "../data_types/Overloads.h"

TEST(Simplex, TahaCases) {
    const auto x = "x";
    const auto y = "y";
    const auto z = "z";

// Key example for testing the 2-phase method
TEST(Simplex, TahaExample3_4_1) {
    const auto z = objectiveVar("z");
    const auto x1 = var("x1");
    const auto x2 = var("x2");

    const auto objective = 4.0 * x1 + x2 == z;

    const std::vector constraints {
        3.0 * x1 + x2 == 3.0,
        4.0 * x1 + 3.0 * x2 >= 6.0,
        x1 +- 2.0 * x2 <= 4.0,
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
        437.64 * z,
        470.6 * x1,
        329.4 * x2
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
