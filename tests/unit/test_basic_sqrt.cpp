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
using   OpName = std::string;

using Param = typename std::tuple<TypeName, OpName>;

class TestUnitBasicSqrt : public TestWithParam<Param> {
private:

    template<typename TX>
    static void check(int& errors, TX x0, TX x1, TX r0, TX r1,
                      const char type[], const char op[], int n)
    {
        using T = typename tfcp::traitx<TX>::base;
        static constexpr int lenx = sizeof(TX) / sizeof(T);
        T tolerance = std::is_same<T, float>::value ? 0.00001f : 1e-13;

        x0 = renormalize(x0, x1, x1);
        r0 = renormalize(r0, r1, r1);   // r = sqrt(x), presumably

        TX a0, a1;
        a0 = pmul(r0, r1, r0, r1, a1);  // a = r^2

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
                    printf("ERROR: type=%s op=%s iter=%d i=%d actual=%g + %g expected=%g + %g\n",
                        type, op, n + 1, i, a0i, a1i, x0i, x1i);
                }
            }
        }
    }

protected:

    // u = update(x), then call tsqrt(u)
    template<typename T> static T update_tsqrt(T x0, T x1, T& u0, T& u1, T& z1)
    {
        u0 = x0;
        u1 = x1;
        return tsqrt(u0, u1, z1);
    }

    // u = update(x), then call tsqrt0(u)
    template<typename T> static T update_tsqrt0(T x0, T x1, T& u0, T& u1, T& z1)
    {
        u0 = x0;
        u1 = setzerox<T>();
        return tsqrt0(u0, z1);
    }

    // u = update(x), then call tsqrtp(u)
    template<typename T> static T update_tsqrtp(T x0, T x1, T& u0, T& u1, T& z1)
    {
        u0 = renormalize(x0, x1, u1);
        return tsqrtp(u0, u1, z1);
    }

    // u = update(x), then call psqrt(u)
    template<typename T> static T update_psqrt(T x0, T x1, T& u0, T& u1, T& z1)
    {
        u0 = renormalize(x0, x1, u1);
        return psqrt(u0, u1, z1);
    }

    // u = update(x), then call psqrt0(u)
    template<typename T> static T update_psqrt0(T x0, T x1, T& u0, T& u1, T& z1)
    {
        u0 = x0;
        u1 = setzerox<T>();
        return psqrt0(u0, z1);
    }

protected:

    template<typename TX>
    static void test_sqrt(const char type[], const char op[],
                          TX (*f)(TX x0, TX x1, TX& u0, TX& u1, TX& z1))
    {
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

            // u = update(x) for specific function
            // - if f = tsqrt , then u = x (unchanged)
            // - if f = tsqrt0, then u = x0
            // - if f = tsqrtp, then u = renormalize(x)
            // - if f = psqrt , then u = renormalize(x)
            // - if f = psqrt0, then u = x0
            TX u0, u1, r0, r1;
            r0 = f(x0, x1, u0, u1, r1);
            check(errors, u0, u1, r0, r1, type, op, n);
        }

        ASSERT_EQ(errors, 0);
    }
};

TEST_P(TestUnitBasicSqrt, smoke) {
    auto param = GetParam();
    auto type = std::get<0>(param);
    auto   op = std::get<1>(param);

    #define OP_CASE(TX, OP)                       \
        if (op == #OP) {                          \
            test_sqrt<TX>(#TX, #OP, update_##OP); \
            return;                               \
        }

    #define TYPE_CASE(TX)                   \
        if (type == #TX) {                  \
            OP_CASE(TX, tsqrt);             \
            OP_CASE(TX, tsqrt0);            \
            OP_CASE(TX, tsqrtp);            \
            OP_CASE(TX, psqrt);             \
            OP_CASE(TX, psqrt0);            \
            FAIL() << "unknown op: " << op; \
        }

    TYPE_CASE(float);
    TYPE_CASE(double);
    TYPE_CASE(floatx);
    TYPE_CASE(doublex);

    FAIL() << "unknown type: " << type;

    #undef TYPE_CASE
    #undef   OP_CASE
}

//----------------------------------------------------------------------

} // namespace

INSTANTIATE_TEST_SUITE_P(types, TestUnitBasicSqrt,
                         Combine(Values("float",
                                        "double",
                                        "floatx",
                                        "doublex"),
                                 Values("tsqrt",
                                        "tsqrt0",
                                        "tsqrtp",
                                        "psqrt",
                                        "psqrt0")));
