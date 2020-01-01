#include <tfcp/simd.h>

#include <gtest/gtest.h>

#include <iostream>

namespace {

#if defined(TFCP_SIMD_FMA) || defined(TFCP_STD_FMA)
template<typename T> void test_fma(const char name[]) {
    std::cout << "test_fma: " << name << std::endl;
    T x = 1, y = 2, z = 3;
    T r = tfcp::fmsub(x, y, z);
    EXPECT_EQ(r, -1);
}

TEST(TfcpSimd, Fma) {
    test_fma<float>("float");
    test_fma<double>("double");
}
#endif

} // namespace
