//======================================================================
// 2019 (c) Evgeny Latkin
// License: Apache 2.0 (http://www.apache.org/licenses/)
//======================================================================

#include <tfcp/simd.h>

#include <gtest/gtest.h>

#include <iostream>
#include <string>

#if !defined(TFCP_SIMD_AVX)
#error TFCP_SIMD_AVX undefined! (please enable AVX2)
#endif

#if !defined(TFCP_SIMD_FMA)
#error TFCP_SIMD_FMA undefined! (please enable AVX2)
#endif

namespace {

using namespace testing;

class TypeName {
public:
    using Type = std::string;
    Type get() { return value; }
    TypeName(const char init[]) { value = init; }
public:
    friend
    std::ostream& operator<< (std::ostream& out, const TypeName& typeName) {
        return out << typeName.value;
    }
private:
    Type value;
};

class TestUnitFma : public TestWithParam<TypeName> {
public:
    template<typename T>
    static void test_fma(const char name[]) {
        T x = 1, y = 2, z = 3;
        T r = tfcp::fmsub(x, y, z);
        EXPECT_EQ(r, -1);
    }
};

TEST_P(TestUnitFma, smoke) {
    auto param = GetParam();
    std::string typeName = param.get();

    if (typeName == "float") {
        test_fma<float>("float");
    }
    else if (typeName == "double") {
        test_fma<double>("double");
    }
    else {
        FAIL();
    }
}

} // namespace

INSTANTIATE_TEST_SUITE_P(types, TestUnitFma,
                         Values(TypeName("float"),
                                TypeName("double")),
                         PrintToStringParamName());
