//======================================================================
// 2019-2020 (c) Evgeny Latkin
// License: Apache 2.0 (http://www.apache.org/licenses/)
//======================================================================

#include <tfcp/test_utils.h>
#include <tfcp/twofold.h>
#include <tfcp/exact.h>

#include <gtest/gtest.h>

#include <random>
#include <string>

namespace {

using namespace tfcp;

using namespace testing;

//----------------------------------------------------------------------

using TypeName = std::string;
using   OpName = std::string;

using Params = typename std::tuple<TypeName, OpName>;

class TestUnitTwofoldRenorm : public TestWithParam<Params> {
protected:

    template<typename T, typename F>
    static void test_case(const char type[], const char op[], F f)
    {
        std::mt19937 gen;
        std::normal_distribution<T> dis(0, 1);

        int errors = 0;

        // repeat this test 1000 times
        for (int n = 0; n < 1000; n++)
        {
            shaped<T> x, r;

            x.value = dis(gen);
            x.error = dis(gen) * x.value / 1000;

            r = f(x);

            T r0, r1;
            r0 = padd0(x.value, x.error, r1);

            if (r.value != r0 ||
                r.error != r1)
            {
                std::cout << "ERROR: type=" << type
                          << " op=" << op
                          << " iter=" << (n+1)
                          << " result=" << r
                          << " expected=" << r0 << "(" << r1 << ")"
                          << std::endl;
                if (++errors > 25) {
                    FAIL() << "too many failures";
                }
            }
        }

        ASSERT_EQ(errors, 0);
    }
};

TEST_P(TestUnitTwofoldRenorm, smoke) {
    auto param = GetParam();
    auto type = std::get<0>(param);
    auto op   = std::get<1>(param);

#define OP_CASE(T, OP)                    \
    if (op == #OP) {                      \
        auto f = [](const shaped<T>& x) { \
            return OP(x);                 \
        };                                \
        test_case<T>(#T, #OP, f);         \
        return;                           \
    }

#define TYPE_CASE(T)                    \
    if (type == #T) {                   \
        OP_CASE(T, renormalize);        \
        OP_CASE(T, fast_renorm);        \
        FAIL() << "unknown op: " << op; \
    }

    TYPE_CASE(float);
    TYPE_CASE(double);

#undef TYPE_CASE
#undef   OP_CASE

    FAIL() << "unknown type: " << type;
}

//----------------------------------------------------------------------

} // namespace

INSTANTIATE_TEST_SUITE_P(typesAndOps, TestUnitTwofoldRenorm,
                         Combine(Values("float",
                                        "double"),
                                 Values("renormalize",
                                        "fast_renorm")));
