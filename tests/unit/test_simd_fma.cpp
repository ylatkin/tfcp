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

using namespace tfcp;

using namespace testing;

using namespace std;

//----------------------------------------------------------------------
//
// GTest cannot pretty-print parameters of the char* or std::string type
// (for me this looks like the bug at the GTest)
//
// So we define the parameter type which enables GTest pretty-printing
//
//----------------------------------------------------------------------

class TypeName {
private:
    string m_name;
public:
    string get() { return m_name; }
    TypeName(const char name[]) { m_name = name; }
public:
    friend
    ostream& operator<< (ostream& out, const TypeName& typeName) {
        return out << typeName.m_name;
    }
};

//----------------------------------------------------------------------

class TestUnitFma : public TestWithParam<TypeName> {
public:
    template<typename T>
    static void test_fma(const char name[]) {
        T x = { 1 }, y = { 2 }, z = { 3 };
        T r = fmsub(x, y, z);
        EXPECT_EQ(get(r, 0), -1);
    }
};

TEST_P(TestUnitFma, smoke) {
    auto param = GetParam();
    std::string typeName = param.get();

    if (typeName == "float") {
        test_fma<float>(typeName.c_str());
    }
    else if (typeName == "double") {
        test_fma<double>(typeName.c_str());
    }
    else if (typeName == "floatx") {
        test_fma<floatx>(typeName.c_str());
    }
    else if (typeName == "doublex") {
        test_fma<doublex>(typeName.c_str());
    }
    else {
        FAIL();
    }
}

//----------------------------------------------------------------------

} // namespace

INSTANTIATE_TEST_SUITE_P(types, TestUnitFma,
                         Values(TypeName("float"),
                                TypeName("double"),
                                TypeName("floatx"),
                                TypeName("doublex")),
                         PrintToStringParamName());
