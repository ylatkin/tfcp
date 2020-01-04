//======================================================================
// 2019 (c) Evgeny Latkin
// License: Apache 2.0 (http://www.apache.org/licenses/)
//======================================================================

#include <tfcp/test_utils.h>
#include <tfcp/simd.h>

#include <gtest/gtest.h>

#include <string>

namespace {

using namespace tfcp;

using namespace testing;

//----------------------------------------------------------------------

using TypeName = std::string;

class TestUnitSqrt : public TestWithParam<TypeName> {
protected:
    template<typename T>
    static void test_sqrt(const std::string& typeName) {
        T x = { 4 }, result;
        result = tfcp::sqrt(x);
        EXPECT_EQ(get(result, 0), 2);
    }
};

TEST_P(TestUnitSqrt, smoke) {
    auto typeName = GetParam();

    if (typeName == "float") {
        test_sqrt<float>(typeName);
    }
    else if (typeName == "double") {
        test_sqrt<double>(typeName);
    }
    else if (typeName == "floatx") {
        test_sqrt<floatx>(typeName);
    }
    else if (typeName == "doublex") {
        test_sqrt<doublex>(typeName);
    }
    else {
        FAIL();
    }
}

//----------------------------------------------------------------------

} // namespace

INSTANTIATE_TEST_SUITE_P(types, TestUnitSqrt, Values("float",
                                                     "double",
                                                     "floatx",
                                                     "doublex"));
