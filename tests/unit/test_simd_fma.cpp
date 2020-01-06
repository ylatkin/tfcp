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

class TestUnitSimdFma : public TestWithParam<TypeName>
{
protected:
    template<typename T, typename TX>
    static void test_fma(const char type[])
    {
        std::mt19937 gen;
        std::uniform_real_distribution<T> dis(-100, 100);

        int errors = 0;

        // repeat this test 1000 times
        for (int n = 0; n < 1000; n++)
        {
            TX x, y, z, result;

            int len = sizeof(TX) / sizeof(T);
            for (int i = 0; i < len; i++)
            {
                getx(x, i) = dis(gen);
                getx(y, i) = dis(gen);
                getx(z, i) = dis(gen);
            }

            result = tfcp::fmsub(x, y, z); // maybe short-vector operation

            for (int i = 0; i < len; i++)
            {
                T xi = getx(x, i);
                T yi = getx(y, i);
                T zi = getx(z, i);
                T ri = getx(result, i);       // actual result
                T ei = std::fma(xi, yi, -zi); // expected
                if (ri != ei)
                {
                    printf("ERROR: type=%s iter=%d i=%d result=%g(%a) expected=%g(%a)\n",
                           type, n + 1, i, ri, ri, ei, ei);
                    errors++;
                    if (errors > 25) {
                        FAIL() << "too many failures";
                    }
                }
            }
        }

        ASSERT_EQ(errors, 0);
    }
};

TEST_P(TestUnitSimdFma, smoke) {
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

INSTANTIATE_TEST_SUITE_P(types, TestUnitSimdFma, Values("float",
                                                        "double",
                                                        "floatx",
                                                        "doublex"));
