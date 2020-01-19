//======================================================================
// 2014-2015, 2019-2020 (c) Evgeny Latkin
// License: Apache 2.0 (http://www.apache.org/licenses/)
//======================================================================

#include <tfcp/twofold.h>
#include <tfcp/basic.h>

namespace tfcp {
//======================================================================

    //------------------------------------------------------------------
    //
    //  Arithmetic functions: tadd, tsub, tmul, tdiv, tsqrt, renormalize
    //
    //------------------------------------------------------------------

#define TFCP_RENORM(T)                                \
    shaped<T> renormalize(const shaped<T>& x) {       \
        T value, error;                               \
        value = renormalize(x.value, x.error, error); \
        return shaped<T>(value, error);               \
    }                                                 \
    shaped<T> fast_renorm(const shaped<T>& x) {       \
        T value, error;                               \
        value = fast_renorm(x.value, x.error, error); \
        return shaped<T>(value, error);               \
    }
    TFCP_RENORM(double);
    TFCP_RENORM(float);
#undef TFCP_RENORM

//======================================================================
}  // namespace tfcp
