//======================================================================
// 2019 (c) Evgeny Latkin
// License: Apache 2.0 (http://www.apache.org/licenses/)
//======================================================================

#include <tfcp/test_utils.h>
#include <tfcp/simd.h>

#include <gtest/gtest.h>

#include <random>
#include <string>

namespace {

using namespace tfcp;

using namespace testing;

//----------------------------------------------------------------------

using TypeName = std::string;

class TestUnitSqrt : public TestWithParam<TypeName> {
protected:
    template<typename T, typename TX>
    static void test_sqrt(const char typeName[]) {
        (void)typeName; // unused

        std::mt19937 gen;
        std::uniform_real_distribution<T> dis(0, 1000);

        // repeat this test 1000 times
        for (int n = 0; n < 1000; n++)
        {
            TX x, result;

            int len = sizeof(TX) / sizeof(T);
            for (int i = 0; i < len; i++)
            {
                get(x, i) = dis(gen);
            }

            result = tfcp::sqrt(x); // short-vector operation

            for (int i = 0; i < len; i++)
            {
                EXPECT_EQ(get(result, i), std::sqrt(get(x, i)));
            }
        }
    }
};

TEST_P(TestUnitSqrt, smoke) {
    auto typeName = GetParam();

    #define TESTCASE(T,TX)         \
        if (typeName == #TX) {     \
            test_sqrt<T, TX>(#TX); \
            return;                \
        }

    TESTCASE(float, float);
    TESTCASE(float, floatx);
    TESTCASE(double, double);
    TESTCASE(double, doublex);

    #undef TESTCASE

    FAIL() << "unknown type: " << typeName;
}

//----------------------------------------------------------------------

} // namespace

INSTANTIATE_TEST_SUITE_P(types, TestUnitSqrt, Values("float",
                                                     "double",
                                                     "floatx",
                                                     "doublex"));
