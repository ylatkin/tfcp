//======================================================================
// 2019 (c) Evgeny Latkin
// License: Apache 2.0 (http://www.apache.org/licenses/)
//======================================================================

#include <tfcp/test_utils.h>
#include <tfcp/simd.h>

#include <gtest/gtest.h>

#include <cmath>

#include <random>
#include <string>

#if !defined(TFCP_SIMD_AVX)
#error TFCP_SIMD_AVX undefined! (please enable AVX2)
#endif

#if !defined(TFCP_SIMD_FMA)
#error TFCP_SIMD_FMA undefined! (please enable AVX2)
#endif

namespace {

using namespace tfcp;

using namespace testing;

//----------------------------------------------------------------------

using TypeName = std::string;

class TestUnitFma : public TestWithParam<TypeName>
{
protected:
    template<typename T, typename TX>
    static void test_fma(const char typeName[])
    {
        (void) typeName; // unused

        std::mt19937 gen;
        std::uniform_real_distribution<T> dis(0, 1000);

        // repeat this test 1000 times
        for (int n = 0; n < 1000; n++)
        {
            TX x, y, z, result;

            int len = sizeof(TX) / sizeof(T);
            for (int i = 0; i < len; i++)
            {
                get(x, i) = dis(gen);
                get(y, i) = dis(gen);
                get(z, i) = dis(gen);
            }

            result = tfcp::fmsub(x, y, z); // short-vector operation

            for (int i = 0; i < len; i++)
            {
                EXPECT_EQ(get(result, i), std::fma(get(x, i), get(y, i), -get(z, i)));
            }
        }
    }
};

TEST_P(TestUnitFma, smoke) {
    auto typeName = GetParam();

    #define TESTCASE(T,TX)        \
        if (typeName == #TX) {    \
            test_fma<T, TX>(#TX); \
            return;               \
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

INSTANTIATE_TEST_SUITE_P(types, TestUnitFma, Values("float",
                                                    "double",
                                                    "floatx",
                                                    "doublex"));
