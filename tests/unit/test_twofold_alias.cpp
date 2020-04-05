//======================================================================
// 2020 (c) Evgeny Latkin
// License: Apache 2.0 (http://www.apache.org/licenses/)
//======================================================================

#include <tfcp/twofold.h>

#include <gtest/gtest.h>

#include <random>
#include <string>
#include <tuple>

#include <cmath>
#include <cstdio>

namespace {

using namespace tfcp;

using namespace testing;

//----------------------------------------------------------------------
//
// Test the alias types:
// - tfloat, tdouble
// - pfloat, pdouble
//
// The types pfloat, pdouble are just the alias names for the types
// coupled<float>, coupled<double>
//
// Types tfloat, tdouble are a bit more complicated:
// - They are aliases for twofold<float>, twofold<double> if Debug mode
// - They are aliases for just float, double if comiler mode is Release
//
//----------------------------------------------------------------------

using ModeName = std::string;

class TestTwofoldAlias : public TestWithParam<ModeName> {
protected:
    static void test_debug()
    {
    #ifdef NDEBUG
        GTEST_SKIP();
    #endif

        EXPECT_EQ(sizeof(tfloat), 2 * sizeof(float));

        EXPECT_EQ(sizeof(tdouble), 2 * sizeof(double));

        EXPECT_EQ(sizeof(pfloat), 2 * sizeof(float));

        EXPECT_EQ(sizeof(pdouble), 2 * sizeof(double));

    #if 0
        EXPECT_EQ(sizeof(tfloatx), 2 * sizeof(floatx));

        EXPECT_EQ(sizeof(tdoublex), 2 * sizeof(doublex));

        EXPECT_EQ(sizeof(pfloatx), 2 * sizeof(floatx));

        EXPECT_EQ(sizeof(pdoublex), 2 * sizeof(doublex));
    #endif
    }

    static void test_release()
    {
    #ifndef NDEBUG
        GTEST_SKIP();
    #endif

        EXPECT_EQ(sizeof(tfloat), sizeof(float));

        EXPECT_EQ(sizeof(tdouble), sizeof(double));

        EXPECT_EQ(sizeof(pfloat), 2 * sizeof(float));

        EXPECT_EQ(sizeof(pdouble), 2 * sizeof(double));

    #if 0
        EXPECT_EQ(sizeof(tfloatx), sizeof(floatx));

        EXPECT_EQ(sizeof(tdoublex), sizeof(doublex));

        EXPECT_EQ(sizeof(pfloatx), 2 * sizeof(floatx));

        EXPECT_EQ(sizeof(pdoublex), 2 * sizeof(doublex));
    #endif
    }
};

TEST_P(TestTwofoldAlias, smoke) {
    ModeName mode = GetParam();

#define MODE_CASE(MODE)  \
    if (mode == #MODE) { \
        test_##MODE();   \
        return;          \
    }

    MODE_CASE(debug);
    MODE_CASE(release);

    FAIL() << "unknown mode: " << mode;

#undef MODE_CASE
}

//----------------------------------------------------------------------

} // namespace

INSTANTIATE_TEST_SUITE_P(modes, TestTwofoldAlias, Values("debug", "release"));
