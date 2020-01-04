//======================================================================
// 2019 (c) Evgeny Latkin
// License: Apache 2.0 (http://www.apache.org/licenses/)
//======================================================================

#include <tfcp/test_utils.h>
#include <tfcp/simd.h>

#include <gtest/gtest.h>

#include <random>
#include <string>
#include <tuple>

#include <cstdio>

#if !defined(TFCP_SIMD_AVX)
#error TFCP_SIMD_AVX undefined! (please enable AVX2)
#endif

namespace {

using namespace tfcp;

using namespace testing;

//----------------------------------------------------------------------

using TypeName = std::string;
using   OpName = std::string;

using Params = typename std::tuple<TypeName, OpName>;

class TestUnitOps : public TestWithParam<Params> {
private:

    template<typename T> static T add(T x, T y) { return x + y; }
    template<typename T> static T sub(T x, T y) { return x - y; }
    template<typename T> static T mul(T x, T y) { return x * y; }
    template<typename T> static T div(T x, T y) { return x / y; }

    template<typename T, typename TX, typename F, typename FX>
    static void test_case(const char type[], const char op[], F f, FX fx)
    {
        std::mt19937 gen;
        std::uniform_real_distribution<T> dis(0, 1000);

        int errors = 0;

        // repeat this test 1000 times
        for (int n = 0; n < 1000; n++)
        {
            TX x, y, result;

            int len = sizeof(TX) / sizeof(T);
            for (int i = 0; i < len; i++)
            {
                get(x, i) = dis(gen);
                get(y, i) = dis(gen);
            }

            result = fx(x, y); // short-vector operation

            for (int i = 0; i < len; i++)
            {
                T actual = get(result, i);
                T expected = f(get(x, i), get(y, i));
                if (actual != expected) {
                    printf("ERROR: type=%s op=%s iter=%d i=%d result=%g(%a) expected=%g(%a)\n",
                        type, op, n+1, i, actual, actual, expected, expected);
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

#define TEST_CASE(OP)                               \
    template<typename T, typename TX>               \
    static void test_##OP(const char type[])        \
    {                                               \
        test_case<T, TX>(type, #OP, OP<T>, OP<TX>); \
    }

    TEST_CASE(add);
    TEST_CASE(sub);
    TEST_CASE(mul);
    TEST_CASE(div);

#undef TEST_CASE
};

TEST_P(TestUnitOps, smoke) {
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
        OP_CASE(T, TX, div);            \
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

INSTANTIATE_TEST_SUITE_P(types, TestUnitOps,
                         Combine(Values("float",
                                        "double",
                                        "floatx",
                                        "doublex"),
                                 Values("add",
                                        "sub",
                                        "mul",
                                        "div")));
