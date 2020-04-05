//======================================================================
// 2020 (c) Evgeny Latkin
// License: Apache 2.0 (http://www.apache.org/licenses/)
//======================================================================

#include <tfcp/twofold.h>

#include <gtest/gtest.h>

#include <string>
#include <type_traits>

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

        EXPECT_TRUE((std::is_same<tfloat, twofold<float>>::value));

        EXPECT_TRUE((std::is_same<tdouble, twofold<double>>::value));

        EXPECT_TRUE((std::is_same<pfloat, coupled<float>>::value));

        EXPECT_TRUE((std::is_same<pdouble, coupled<double>>::value));
    }

    static void test_release()
    {
    #ifndef NDEBUG
        GTEST_SKIP();
    #endif

        EXPECT_TRUE((std::is_same<tfloat, float>::value));

        EXPECT_TRUE((std::is_same<tdouble, double>::value));

        EXPECT_TRUE((std::is_same<pfloat, coupled<float>>::value));

        EXPECT_TRUE((std::is_same<pdouble, coupled<double>>::value));
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
