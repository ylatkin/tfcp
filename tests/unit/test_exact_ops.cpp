//======================================================================
// 2019-2020 (c) Evgeny Latkin
// License: Apache 2.0 (http://www.apache.org/licenses/)
//======================================================================

#include <tfcp/test_utils.h>
#include <tfcp/exact.h>
#include <tfcp/simd.h>

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
// Reference functions (scalar types): ref_padd0, ref_psub0, ref_pmul0
//
//----------------------------------------------------------------------

template<typename T> T ref_padd0(T x, T y, T& r1)
{
    if (std::fabs(x) >= std::fabs(y))
    {
        return fast_padd0(x, y, r1);
    }
    else
    {
        return fast_padd0(y, x, r1);
    }
}

template<typename T> T ref_psub0(T x, T y, T& r1)
{
    if (std::fabs(x) >= std::fabs(y))
    {
        return fast_psub0(x, y, r1);
    }
    else
    {
        T t0, t1;
        t0 = fast_psub0(y, x, t1);
        r1 = -t1;
        return -t0;
    }
}

template<typename T> T ref_pmul0(T x, T y, T& r1)
{
    return nofma_pmul0(x, y, r1);
}

//----------------------------------------------------------------------

using TypeName = std::string;
using   OpName = std::string;

using Params = typename std::tuple<TypeName, OpName>;

class TestUnitExactOps : public TestWithParam<Params> {
private:

    template<typename T, typename TX, typename F, typename FX>
    static void test_case(const char type[], const char op[], F f, FX fx)
    {
        std::mt19937 gen;
        std::uniform_real_distribution<T> dis(-10, 10);

        int errors = 0;

        // repeat this test 1000 times
        for (int n = 0; n < 1000; n++)
        {
            TX x, y, r0, r1;

            int len = sizeof(TX) / sizeof(T);
            for (int i = 0; i < len; i++)
            {
                getx(x, i) = dis(gen);
                getx(y, i) = dis(gen);
            }

            r0 = fx(x, y, r1); // maybe short-vector operation

            for (int i = 0; i < len; i++)
            {
                T xi = getx(x, i);
                T yi = getx(y, i);
                T r0i, r1i; // actual result
                T e0i, e1i; // expected
                r0i = getx(r0, i);
                r1i = getx(r1, i);
                e0i = f(xi, yi, e1i);
                if (r0i != e0i || r1i != e1i) {
                    printf("ERROR: type=%s op=%s iter=%d i=%d "
                           "result=%g + %g expected=%g + %g\n",
                           type, op, n+1, i, r0i, r1i, e0i, e1i);
                    errors++;
                    if (errors > 25) {
                        FAIL() << "too many failures";
                    }
                }
            }
        }

        ASSERT_EQ(errors, 0);
    }

protected:

#define TEST_CASE(OP)                                               \
    template<typename T, typename TX>                               \
    static void test_##OP(const char type[])                        \
    {                                                               \
        test_case<T, TX>(type, #OP, ref_p##OP##0<T>, p##OP##0<TX>); \
    }

    TEST_CASE(add);
    TEST_CASE(sub);
    TEST_CASE(mul);

#undef TEST_CASE
};

TEST_P(TestUnitExactOps, smoke) {
    auto param = GetParam();
    auto type = std::get<0>(param);
    auto op   = std::get<1>(param);

#define OP_CASE(T, TX, OP)    \
    if (op == #OP) {          \
        test_##OP<T,TX>(#TX); \
        return;               \
    }

#define TYPE_CASE(T, TX)                \
    if (type == #TX) {                  \
        OP_CASE(T, TX, add);            \
        OP_CASE(T, TX, sub);            \
        OP_CASE(T, TX, mul);            \
        FAIL() << "unknown op: " << op; \
    }

    TYPE_CASE(float, float);
    TYPE_CASE(float, floatx);
    TYPE_CASE(double, double);
    TYPE_CASE(double, doublex);

#undef TYPE_CASE
#undef   OP_CASE

    FAIL() << "unknown type: " << type;
}

//----------------------------------------------------------------------

} // namespace

INSTANTIATE_TEST_SUITE_P(types, TestUnitExactOps,
                         Combine(Values("float",
                                        "double",
                                        "floatx",
                                        "doublex"),
                                 Values("add",
                                        "sub",
                                        "mul")));
