//======================================================================
// 2019-2020 (c) Evgeny Latkin
// License: Apache 2.0 (http://www.apache.org/licenses/)
//======================================================================

#include <tfcp/test_utils.h>
#include <tfcp/basic.h>
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

// Exact: r0 + r1 = x + y
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

// Exact: r0 + r1 = x - y
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

// Exact: r0 + r1 = x * y
template<typename T> T ref_pmul0(T x, T y, T& r1)
{
    return nofma_pmul0(x, y, r1);
}

// Twofold: z0 + z1 = (x0 + x1) + (y0 + y1)
template<typename T> T ref_tadd(T x0, T x1, T y0, T y1, T& z1)
{
    T r0, r1;
    r0 = padd0(x0, y0, r1);
    z1 = r1 + (x1 + y1);
    return r0;
}

// Twofold: z0 + z1 = (x0 + x1) - (y0 + y1)
template<typename T> T ref_tsub(T x0, T x1, T y0, T y1, T& z1)
{
    T r0, r1;
    r0 = psub0(x0, y0, r1);
    z1 = r1 + (x1 - y1);
    return r0;
}

// Twofold: z0 + z1 = (x0 + x1) * (y0 + y1)
template<typename T> T ref_tmul(T x0, T x1, T y0, T y1, T& z1)
{
    T r0, r1, p01, p10, p11, e01, e10, e11;
    r0 = pmul0(x0, y0, r1);
    p01 = pmul0(x0, y1, e01);
    p10 = pmul0(x1, y0, e10);
    p11 = pmul0(x1, y1, e11);
    z1 = e11 + (e01 + e10) + p11 + (p01 + p10);
    return r0;
}

// Coupled: z0 + z1 = (x0 + x1) + (y0 + y1)
template<typename T> T ref_padd(T x0, T x1, T y0, T y1, T& z1)
{
    T r0, r1;
    r0 = tadd(x0, x1, y0, y1, r1);
    return renormalize(r0, r1, z1);
}

// Coupled: z0 + z1 = (x0 + x1) - (y0 + y1)
template<typename T> T ref_psub(T x0, T x1, T y0, T y1, T& z1)
{
    T r0, r1;
    r0 = tsub(x0, x1, y0, y1, r1);
    return renormalize(r0, r1, z1);
}

// Coupled: z0 + z1 = (x0 + x1) * (y0 + y1)
template<typename T> T ref_pmul(T x0, T x1, T y0, T y1, T& z1)
{
    T r0, r1;
    r0 = tmul(x0, x1, y0, y1, r1);
    return renormalize(r0, r1, z1);
}

// Rename with test_... suffix
template<typename T> T test_tadd(T x0, T x1, T y0, T y1, T& z1) { return tadd(x0, x1, y0, y1, z1); }
template<typename T> T test_tsub(T x0, T x1, T y0, T y1, T& z1) { return tsub(x0, x1, y0, y1, z1); }
template<typename T> T test_tmul(T x0, T x1, T y0, T y1, T& z1) { return tmul(x0, x1, y0, y1, z1); }



// Redundant parameters for special-case functions (if one or both args are dotted)
#define TFCP_SPEC(P, Q, F) \
template<typename T> T P## t ##F## 1(T x0, T x1, T y0, T y1, T& z1) { return Q## t ##F(x0, x1, y0, setzerox<T>(), z1); } \
template<typename T> T P## p ##F## 1(T x0, T x1, T y0, T y1, T& z1) { return Q## p ##F(x0, x1, y0, setzerox<T>(), z1); } \
template<typename T> T P## t ##F## 2(T x0, T x1, T y0, T y1, T& z1) { return Q## t ##F(x0, setzerox<T>(), y0, y1, z1); } \
template<typename T> T P## p ##F## 2(T x0, T x1, T y0, T y1, T& z1) { return Q## p ##F(x0, setzerox<T>(), y0, y1, z1); } \
template<typename T> T P## t ##F## 0(T x0, T x1, T y0, T y1, T& z1) { return Q## t ##F(x0, setzerox<T>(), y0, setzerox<T>(), z1); } \
template<typename T> T P## p ##F## 0(T x0, T x1, T y0, T y1, T& z1) { return Q## p ##F(x0, setzerox<T>(), y0, setzerox<T>(), z1); }

TFCP_SPEC(ref_, ref_, add)
TFCP_SPEC(ref_, ref_, sub)
TFCP_SPEC(ref_, ref_, mul)

TFCP_SPEC(test_, , add)
TFCP_SPEC(test_, , sub)
TFCP_SPEC(test_, , mul)

#undef TFCP_SPEC

//----------------------------------------------------------------------

using TypeName = std::string;
using   OpName = std::string;

using Params = typename std::tuple<TypeName, OpName>;

class TestUnitBasicOps : public TestWithParam<Params> {
protected:

    template<typename T, typename TX>
    static void test_case(const char type[], const char op[],
                          T  (*f )(T  x0, T  x1, T  y0, T  y1, T & z1),
                          TX (*fx)(TX x0, TX x1, TX y0, TX y1, TX& z1))
    {
        T tolerance = std::is_same<T, float>::value ? 0.00001f : 1e-13;

        std::mt19937 gen;
        std::exponential_distribution<T> dis(1);

        int errors = 0;

        // repeat this test 1000 times
        for (int n = 0; n < 1000; n++)
        {
            TX x0, x1, y0, y1, r0, r1;

            int lenx = sizeof(TX) / sizeof(T);
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

            r0 = fx(x0, x1, y0, y1, r1);  // maybe short-vector operation

            for (int i = 0; i < lenx; i++)
            {
                T x0i = getx(x0, i);
                T x1i = getx(x1, i);
                T y0i = getx(y0, i);
                T y1i = getx(y1, i);

                T r0i, r1i;  // actual result
                T e0i, e1i;  // expected
                r0i = getx(r0, i);
                r1i = getx(r1, i);
                e0i = f(x0i, x1i, y0i, y1i, e1i);  // reference: scalar operation

                if (std::fabs(r0i - e0i) > tolerance * std::fabs(e0i) ||
                    std::fabs(r1i - e1i) > tolerance)
                {
                    if (errors++ < 25)
                    {
                        printf("ERROR: type=%s op=%s iter=%d i=%d actual=%g + %g expected=%g + %g\n",
                            type, op, n + 1, i, r0i, r1i, e0i, e1i);
                    }
                }
            }
        }

        ASSERT_EQ(errors, 0);
    }
};

TEST_P(TestUnitBasicOps, smoke) {
    auto param = GetParam();
    auto type = std::get<0>(param);
    auto op   = std::get<1>(param);

#define OP_CASE(T, TX, OP)                                     \
    if (op == #OP) {                                           \
        test_case<T,TX>(#TX, #OP, ref_##OP<T>, test_##OP<TX>); \
        return;               \
    }

#define TYPE_CASE(T, TX)                \
    if (type == #TX) {                  \
        OP_CASE(T, TX, tadd);           \
        OP_CASE(T, TX, tsub);           \
        OP_CASE(T, TX, tmul);           \
        OP_CASE(T, TX, tadd1);          \
        OP_CASE(T, TX, tsub1);          \
        OP_CASE(T, TX, tmul1);          \
        OP_CASE(T, TX, tadd2);          \
        OP_CASE(T, TX, tsub2);          \
        OP_CASE(T, TX, tmul2);          \
        OP_CASE(T, TX, tadd0);          \
        OP_CASE(T, TX, tsub0);          \
        OP_CASE(T, TX, tmul0);          \
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

INSTANTIATE_TEST_SUITE_P(types, TestUnitBasicOps,
                         Combine(Values("float",
                                        "double",
                                        "floatx",
                                        "doublex"),
                                 Values("tadd",
                                        "tadd1",
                                        "tadd2",
                                        "tadd0",
                                        "tsub",
                                        "tsub1",
                                        "tsub2",
                                        "tsub0",
                                        "tmul",
                                        "tmul1",
                                        "tmul2",
                                        "tmul0")));
