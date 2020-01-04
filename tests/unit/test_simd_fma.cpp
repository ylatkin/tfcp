//======================================================================
// 2019 (c) Evgeny Latkin
// License: Apache 2.0 (http://www.apache.org/licenses/)
//======================================================================

#include <tfcp/test_utils.h>
#include <tfcp/simd.h>

#include <gtest/gtest.h>

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

class TestUnitFma : public TestWithParam<TypeName> {
protected:
    template<typename T>
    static void test_fma(const std::string& typeName) {
        T x = { 1 }, y = { 2 }, z = { 3 };
        T result = tfcp::fmsub(x, y, z);
        EXPECT_EQ(get(result, 0), -1);
    }
};

TEST_P(TestUnitFma, smoke) {
    auto typeName = GetParam();

    if (typeName == "float") {
        test_fma<float>(typeName);
    }
    else if (typeName == "double") {
        test_fma<double>(typeName);
    }
    else if (typeName == "floatx") {
        test_fma<floatx>(typeName);
    }
    else if (typeName == "doublex") {
        test_fma<doublex>(typeName);
    }
    else {
        FAIL();
    }
}

//----------------------------------------------------------------------

} // namespace

INSTANTIATE_TEST_SUITE_P(types, TestUnitFma, Values("float",
                                                    "double",
                                                    "floatx",
                                                    "doublex"));
