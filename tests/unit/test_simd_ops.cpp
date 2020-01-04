//======================================================================
// 2019 (c) Evgeny Latkin
// License: Apache 2.0 (http://www.apache.org/licenses/)
//======================================================================

#include <tfcp/test_utils.h>
#include <tfcp/simd.h>

#include <gtest/gtest.h>

#include <string>
#include <tuple>

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
protected:
    template<typename T>
    static void test_add(const std::string& typeName) {
        T x = { 2 }, y = { 3 };
        T result = x + y;
        EXPECT_EQ(get(result, 0), 5);
    }

    template<typename T>
    static void test_sub(const std::string& typeName) {
        T x = { 2 }, y = { 3 };
        T result = x - y;
        EXPECT_EQ(get(result, 0), -1);
    }

    template<typename T>
    static void test_mul(const std::string& typeName) {
        T x = { 2 }, y = { 3 };
        T result = x * y;
        EXPECT_EQ(get(result, 0), 6);
    }

    template<typename T>
    static void test_div(const std::string& typeName) {
        T x = { 3 }, y = { 2 };
        T result = x / y;
        EXPECT_EQ(get(result, 0), 1.5);
    }
};

TEST_P(TestUnitOps, smoke) {
    auto param = GetParam();
    auto type = std::get<0>(param);
    auto op   = std::get<1>(param);

    if (type == "float") {
        if (op == "add") {
            test_add<float>(type);
        }
        else if (op == "sub") {
            test_sub<float>(type);
        }
        else if (op == "mul") {
            test_mul<float>(type);
        }
        else if (op == "div") {
            test_div<float>(type);
        }
        else {
            FAIL() << "unknown op: " << op;
        }
    }
    else if (type == "double") {
        if (op == "add") {
            test_add<double>(type);
        }
        else if (op == "sub") {
            test_sub<double>(type);
        }
        else if (op == "mul") {
            test_mul<double>(type);
        }
        else if (op == "div") {
            test_div<double>(type);
        }
        else {
            FAIL() << "unknown op: " << op;
        }
    }
    else if (type == "floatx") {
        if (op == "add") {
            test_add<floatx>(type);
        }
        else if (op == "sub") {
            test_sub<floatx>(type);
        }
        else if (op == "mul") {
            test_mul<floatx>(type);
        }
        else if (op == "div") {
            test_div<floatx>(type);
        }
        else {
            FAIL() << "unknown op: " << op;
        }
    }
    else if (type == "doublex") {
        if (op == "add") {
            test_add<doublex>(type);
        }
        else if (op == "sub") {
            test_sub<doublex>(type);
        }
        else if (op == "mul") {
            test_mul<doublex>(type);
        }
        else if (op == "div") {
            test_div<doublex>(type);
        }
        else {
            FAIL() << "unknown op: " << op;
        }
    }
    else {
        FAIL() << "unknown type: " << type;
    }
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
