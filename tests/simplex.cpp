//
// Created by neb on 04/03/2026.
//

#include <gtest/gtest.h>

#include "../cmake-build-debug/_deps/googletest-src/googlemock/include/gmock/gmock-matchers.h"
#include "../simplex/Simplex.h"

TEST(Simplex, TahaCases) {
    const auto x = "x";
    const auto y = "y";
    const auto z = "z";

    auto objective = Expression(Variable<double>({ .coefficient = 1.0, .symbol = x }))
        + Variable<double>({ .coefficient = 2.0, .symbol = y })
        - Variable<double>({ .coefficient = 2.3, .symbol = z })
        < Variable<double>({ .coefficient = 2.0, .kind = Solution });

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
        == Variable<double>({ .symbol = z, .kind = Solution });

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
    const std::string z = "z";
    const std::string x1 = "x1";
    const std::string x2 = "x2";

    const auto objective = Expression(Variable<double>({ .coefficient = 40.0, .symbol = x1 }))
        + Variable<double>({ .coefficient = 30.0, .symbol = x2 })
        == Variable<double>({  .symbol = z , .kind = Solution });

    const std::vector constraints = {
        Expression(Variable<double>({ .symbol = x1 }))
            + Variable<double>({ .symbol = x2 })
            <= Variable<double>({ .coefficient = 12.0, .kind = Solution }),
        Expression(Variable<double>({ .coefficient = 2.0, .symbol = x1 }))
            + Variable<double>({ .symbol = x2 })
            <= Variable<double>({ .coefficient = 16.0, .kind = Solution }),
    };

    const auto simplex = Simplex(Maximise, objective, constraints);
    const auto result = simplex.solve();

    EXPECT_THAT(result, testing::UnorderedElementsAre(
        Variable<double>({ .coefficient = 400.0, .symbol = z }),
        Variable<double>({ .coefficient = 4.0, .symbol = x1 }),
        Variable<double>({ .coefficient = 8.0, .symbol = x2 })
    ));
}
