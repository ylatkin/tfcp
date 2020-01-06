//======================================================================
// 2019 (c) Evgeny Latkin
// License: Apache 2.0 (http://www.apache.org/licenses/)
//======================================================================

#ifndef TEST_UTILS_H
#define TEST_UTILS_H
//======================================================================

#include <tfcp/simd.h>

namespace tfcp {

    template<typename TX> struct traitx {};
    template<> struct traitx<float>   { using base = float; };
    template<> struct traitx<floatx>  { using base = float; };
    template<> struct traitx<double>  { using base = double; };
    template<> struct traitx<doublex> { using base = double; };

} // namespace tfcp

//======================================================================
#endif // TEST_UTILS_H
