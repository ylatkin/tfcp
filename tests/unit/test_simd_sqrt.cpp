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

class TestUnitSimdSqrt : public TestWithParam<TypeName> {
protected:
    template<typename T, typename TX>
    static void test_sqrt(const char type[]) {
        std::mt19937 gen;
        std::uniform_real_distribution<T> dis(0, 1000);

        int errors = 0;

        // repeat this test 1000 times
        for (int n = 0; n < 1000; n++)
        {
            TX x, result;

            int len = sizeof(TX) / sizeof(T);
            for (int i = 0; i < len; i++)
            {
                getx(x, i) = dis(gen);
            }

            result = tfcp::sqrt(x); // short-vector operation

            for (int i = 0; i < len; i++)
            {
                T xi = getx(x, i);
                T ri = getx(result, i); // actual result
                T ei = std::sqrt(xi);   // expected
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

TEST_P(TestUnitSimdSqrt, smoke) {
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

INSTANTIATE_TEST_SUITE_P(types, TestUnitSimdSqrt, Values("float",
                                                         "double",
                                                         "floatx",
                                                         "doublex"));
