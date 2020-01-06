//======================================================================
// 2019-2020 (c) Evgeny Latkin
// License: Apache 2.0 (http://www.apache.org/licenses/)
//======================================================================

#include <tfcp/test_utils.h>
#include <tfcp/basic.h>
#include <tfcp/simd.h>

#include <gtest/gtest.h>

#include <random>
#include <string>
#include <type_traits>

#include <cmath>

namespace {

using namespace tfcp;

using namespace testing;

//----------------------------------------------------------------------

using TypeName = std::string;

class TestUnitBasicSqrt : public TestWithParam<TypeName> {
protected:
    template<typename TX>
    static void test_sqrt(const char type[]) {
        using T = typename tfcp::traitx<TX>::base;
        static constexpr int lenx = sizeof(TX) / sizeof(T);

        std::mt19937 gen;
        std::exponential_distribution<T> dis(1);

        int errors = 0;

        // repeat this test 1000 times
        for (int n = 0; n < 1000; n++)
        {
            TX x0, x1;  // twofold x = x0 + x1, etc.
            for (int i = 0; i < lenx; i++)
            {
                T x0i = dis(gen);
                T x1i = dis(gen) * x0i;
                getx(x0, i) = x0i;
                getx(x1, i) = x1i;
            }

            TX r0, r1;
            r0 = tsqrt(x0, x1, r1);
            check(errors, x0, x1, r0, r1, type, n, "tsqrt");

            TX zerox = setzerox<TX>();
            r0 = tsqrt0(x0, r1);
            check(errors, x0, zerox, r0, r1, type, n, "tsqrt0");

            TX y0, y1;
            y0 = renormalize(x0, x1, y1);  // y = renormalize(x)
            r0 = tsqrtp(y0, y1, r1);
            check(errors, y0, y1, r0, r1, type, n, "tsqrtp");

            r0 = psqrt(y0, y1, r1);
            check(errors, y0, y1, r0, r1, type, n, "psqrt");

            r0 = psqrt0(y0, r1);
            check(errors, y0, zerox, r0, r1, type, n, "psqrt");
        }

        ASSERT_EQ(errors, 0);
    }

private:
    template<typename TX>
    static void check(int& errors, TX x0, TX x1, TX r0, TX r1,
                      const char type[], int n, const char fun[])
    {
        using T = typename tfcp::traitx<TX>::base;
        static constexpr int lenx = sizeof(TX) / sizeof(T);
        T tolerance = std::is_same<T, float>::value ? 0.00001f : 1e-13;

        x0 = renormalize(x0, x1, x1);
        r0 = renormalize(r0, r1, r1);  // r = sqrt(x), presumably

        TX a0, a1;
        a0 = pmul(r0, r1, r0, r1, a1);         // a = r^2

        for (int i = 0; i < lenx; i++)
        {
            T x0i = getx(x0, i);
            T x1i = getx(x1, i);
            T a0i = getx(a0, i);
            T a1i = getx(a1, i);

            if (std::fabs(x0i - a0i) > tolerance * std::fabs(x0i) ||
                std::fabs(x1i - a1i) > tolerance)
            {
                if (errors++ < 25)
                {
                    printf("ERROR: type=%s iter=%d f=%s i=%d actual=%g + %g expected=%g + %g\n",
                        type, n + 1, fun, i, a0i, a1i, x0i, x1i);
                }
            }
        }
    }
};

TEST_P(TestUnitBasicSqrt, smoke) {
    auto typeName = GetParam();

    #define TESTCASE(TX)        \
        if (typeName == #TX) {  \
            test_sqrt<TX>(#TX); \
            return;             \
        }

    TESTCASE(float);
    TESTCASE(double);
    TESTCASE(floatx);
    TESTCASE(doublex);

    #undef TESTCASE

    FAIL() << "unknown type: " << typeName;
}

//----------------------------------------------------------------------

} // namespace

INSTANTIATE_TEST_SUITE_P(types, TestUnitBasicSqrt, Values("float",
                                                          "double",
                                                          "floatx",
                                                          "doublex"));
