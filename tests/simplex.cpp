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

    auto objective = Expression(Variable<double>({ .coefficient = 1.0, .symbol = x }))
        + Variable<double>({ .coefficient = 2.0, .symbol = y })
        - Variable<double>({ .coefficient = 2.3, .symbol = z })
        < Variable<double>({ .coefficient = 2.0, .kind = Objective });

    // auto simplex = Simplex<double>(objective);
    // auto result = simplex.solve();

    // TODO
    EXPECT_EQ(1, 2);
}

TEST(Simplex, TahaCases3_2_1) {
    const std::string z = "z";
    const std::string x1 = "x1";
    const std::string x2 = "x2";

    const auto objective = Expression(Variable<double>({ .coefficient = 2.0, .symbol = x1 }))
        + Variable<double>({ .coefficient = 3.0, .symbol = x2 })
        == Variable<double>({ .symbol = z, .kind = Objective });

    const std::vector constraints = {
        Expression(Variable<double>({ .coefficient = 2.0, .symbol = x1 }))
            + Variable<double>({ .symbol = x2 })
            <= Variable<double>({ .coefficient = 4.0, .kind = Solution }),
        Expression(Variable<double>({ .symbol = x1 }))
            + Variable<double>({ .coefficient = 2.0, .symbol = x2 })
            <= Variable<double>({ .coefficient = 5.0, .kind = Solution }),
    };

    const auto simplex = Simplex<double>(Maximise, objective, constraints);
    auto result = simplex.solve();

    // TODO
    EXPECT_EQ(1, 2);
}

TEST(Simplex, LibreTextsExample) {
    const auto z = objectiveVar("z");
    const auto x1 = var("x1");
    const auto x2 = var("x2");

    const auto objective = 40.0 * x1 + 30.0 * x2 == z;

    const std::vector constraints = {
        x1 + x2 <= 12.0,
        2.0 * x1 + x2 <= 16.0
    };

    const auto simplex = Simplex(Maximise, objective, constraints);
    const auto result = simplex.solve();

    EXPECT_THAT(result, testing::UnorderedElementsAre(
        400.0 * z,
        4.0 * x1,
        8.0 * x2
    ));
}
