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

class TestUnitBasicDiv : public TestWithParam<Param> {
private:

    template<typename TX>
    static void check(int& errors, TX x0, TX x1, TX y0, TX y1, TX r0, TX r1,
                      const char type[], const char op[], int n)
    {
        using T = typename tfcp::traitx<TX>::base;
        static constexpr int lenx = sizeof(TX) / sizeof(T);
        T tolerance = std::is_same<T, float>::value ? 0.00001f : 1e-13;

        x0 = renormalize(x0, x1, x1);
        y0 = renormalize(y0, y1, y1);
        r0 = renormalize(r0, r1, r1);   // r = x / y, presumably

        TX a0, a1;
        a0 = pmul(y0, y1, r0, r1, a1);  // a = y * r, nearly x presumably

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

    // u, v = update(x, y), then z = tdiv(u, v)
    template<typename T> static T update_tdiv(T x0, T x1, T y0, T y1, T& u0, T& u1, T& v0, T& v1, T& z1)
    {
        u0 = x0;
        u1 = x1;
        v0 = y0;
        v1 = y1;
        return tdiv(u0, u1, v0, v1, z1);
    }

    // u, v = update(x, y), then z = tdiv1(u, v)
    template<typename T> static T update_tdiv1(T x0, T x1, T y0, T y1, T& u0, T& u1, T& v0, T& v1, T& z1)
    {
        u0 = x0;
        u1 = x1;
        v0 = y0;
        v1 = setzerox<T>();
        return tdiv1(u0, u1, v0, z1);
    }

    // u, v = update(x, y), then z = tdiv2(u, v)
    template<typename T> static T update_tdiv2(T x0, T x1, T y0, T y1, T& u0, T& u1, T& v0, T& v1, T& z1)
    {
        u0 = x0;
        u1 = setzerox<T>();
        v0 = y0;
        v1 = y1;
        return tdiv2(u0, v0, v1, z1);
    }

    // u, v = update(x, y), then z = tdiv0(u, v)
    template<typename T> static T update_tdiv0(T x0, T x1, T y0, T y1, T& u0, T& u1, T& v0, T& v1, T& z1)
    {
        u0 = x0;
        u1 = setzerox<T>();
        v0 = y0;
        v1 = setzerox<T>();
        return tdiv0(u0, v0, z1);
    }

    // u, v = update(x, y), then z = tdivp(u, v)
    template<typename T> static T update_tdivp(T x0, T x1, T y0, T y1, T& u0, T& u1, T& v0, T& v1, T& z1)
    {
        u0 = renormalize(x0, x1, u1);
        v0 = renormalize(y0, y1, v1);
        return tdivp(u0, u1, v0, v1, z1);
    }

    // u, v = update(x, y), then z = pdiv(u, v)
    template<typename T> static T update_pdiv(T x0, T x1, T y0, T y1, T& u0, T& u1, T& v0, T& v1, T& z1)
    {
        u0 = renormalize(x0, x1, u1);
        v0 = renormalize(y0, y1, v1);
        return pdiv(u0, u1, v0, v1, z1);
    }

    // u, v = update(x, y), then z = pdiv1(u, v)
    template<typename T> static T update_pdiv1(T x0, T x1, T y0, T y1, T& u0, T& u1, T& v0, T& v1, T& z1)
    {
        u0 = renormalize(x0, x1, u1);
        v0 = y0;
        v1 = setzerox<T>();
        return pdiv1(u0, u1, v0, z1);
    }

    // u, v = update(x, y), then z = pdiv2(u, v)
    template<typename T> static T update_pdiv2(T x0, T x1, T y0, T y1, T& u0, T& u1, T& v0, T& v1, T& z1)
    {
        u0 = x0;
        u1 = setzerox<T>();
        v0 = renormalize(y0, y1, v1);
        return pdiv2(u0, u1, v0, z1);
    }

    // u, v = update(x, y), then z = pdiv0(u, v)
    template<typename T> static T update_pdiv0(T x0, T x1, T y0, T y1, T& u0, T& u1, T& v0, T& v1, T& z1)
    {
        u0 = x0;
        u1 = setzerox<T>();
        v0 = y0;
        v1 = setzerox<T>();
        return pdiv0(u0, v0, z1);
    }

protected:

    template<typename TX>
    static void test_sqrt(const char type[], const char op[],
        TX (*f)(TX x0, TX x1, TX y0, TX y1, TX& u0, TX& u1, TX& v0, TX& v1, TX& z1))
    {
        using T = typename tfcp::traitx<TX>::base;
        static constexpr int lenx = sizeof(TX) / sizeof(T);

        std::mt19937 gen;
        std::exponential_distribution<T> dis(1);

        int errors = 0;

        // repeat this test 1000 times
        for (int n = 0; n < 1000; n++)
        {
            TX x0, x1, y0, y1;  // twofold x = x0 + x1, etc.
            for (int i = 0; i < lenx; i++)
            {
                T x0i = dis(gen);
                T x1i = dis(gen) * x0i;
                T y0i = dis(gen);
                T y1i = dis(gen) * y0i;
                getx(x0, i) = x0i;
                getx(x1, i) = x1i;
                getx(y0, i) = y0i;
                getx(y1, i) = y1i;
            }

            // u, v = update(x, y) for specific function
            // - if f = tdiv , then u = x , v = y (unchanged)
            // - if f = tdiv1, then u = x , v = y0
            // - if f = tdiv2, then u = x0, v = y
            // - if f = tdiv0, then u = x0, v = y0
            // - if f = tdivp, then u = renorm(x), v = renorm(y)
            // - if f = pdiv , then u = renorm(x), v = renorm(y)
            // - if f = pdiv1, then u = renorm(x), v = y0
            // - if f = pdiv2, then u = x0, v = renorm(y)
            // - if f = pdiv , then u = x0, v = y0
            TX u0, u1, v0, v1, r0, r1;
            r0 = f(x0, x1, y0, y1, u0, u1, v0, v1, r1);
            check(errors, u0, u1, v0, v1, r0, r1, type, op, n);
        }

        ASSERT_EQ(errors, 0);
    }
};

TEST_P(TestUnitBasicDiv, smoke) {
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
            OP_CASE(TX, tdiv);              \
            OP_CASE(TX, tdiv1)              \
            OP_CASE(TX, tdiv2)              \
            OP_CASE(TX, tdiv0)              \
            OP_CASE(TX, tdivp)              \
            OP_CASE(TX, pdiv);              \
            OP_CASE(TX, pdiv1);             \
            OP_CASE(TX, pdiv2);             \
            OP_CASE(TX, pdiv0);             \
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

INSTANTIATE_TEST_SUITE_P(types, TestUnitBasicDiv,
                         Combine(Values("float",
                                        "double",
                                        "floatx",
                                        "doublex"),
                                 Values("tdiv",
                                        "tdiv1",
                                        "tdiv2",
                                        "tdiv0",
                                        "tdivp",
                                        "pdiv",
                                        "pdiv1",
                                        "pdiv2",
                                        "pdiv0")));
