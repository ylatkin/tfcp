//======================================================================
// 2019-2020 (c) Evgeny Latkin
// License: Apache 2.0 (http://www.apache.org/licenses/)
//======================================================================

#ifndef TEST_UTILS_H
#define TEST_UTILS_H
//======================================================================

#include <tfcp/simd.h>

#include <cassert>

namespace tfcp {

    //------------------------------------------------------------------
    //
    // Type traits for short-vectors: floax, doublex
    //
    //------------------------------------------------------------------

    template<typename TX> struct traitx {};
    template<> struct traitx<float>   { using base = float; };
    template<> struct traitx<floatx>  { using base = float; };
    template<> struct traitx<double>  { using base = double; };
    template<> struct traitx<doublex> { using base = double; };

    //------------------------------------------------------------------
    //
    // Get short-vertor i'th position (by reference)
    //
    //------------------------------------------------------------------

    inline float& getx(floatx& x, int i) {
        static constexpr int length = sizeof(floatx) / sizeof(float);
        (void)length;  // maybe unused, if Release build
        assert(0 <= i && i < length);
        return reinterpret_cast<float*>(&x)[i];
    }

    inline double& getx(doublex& x, int i) {
        static constexpr int length = sizeof(doublex) / sizeof(double);
        (void)length;  // maybe unused, if Release build
        assert(0 <= i && i < length);
        return reinterpret_cast<double*>(&x)[i];
    }

    inline float& getx(float& x, int i) {
        assert(i == 0);
        return x;
    }

    inline double& getx(double& x, int i) {
        assert(i == 0);
        return x;
    }

} // namespace tfcp

//======================================================================
#endif // TEST_UTILS_H
