//======================================================================
// 2014-2015, 2019-2020 (c) Evgeny Latkin
// License: Apache 2.0 (http://www.apache.org/licenses/)
//======================================================================

#ifndef TFCP_EXACT_H
#define TFCP_EXACT_H
//======================================================================
//
// Implementation of exact transforms:
//   a + b -> s + t
//   a * b -> p + e
//
// Algorithms:
// - Knuth, Dekker: exact rounding error of a + b
// - Dekker and Veltkamp: exact a + b without FMA
//
// C++ templates, main scalar types: float, double
//
// Additionally, short-vector types: floatx, doublex
//
// Typical interface:
//   T add(T x, T y, T& r1); -- returns r0 = x+y and r1 = rounding error
//
//======================================================================

#include <tfcp/simd.h>

namespace tfcp {

    //------------------------------------------------------------------
    //
    //  Add, subtract, renormalize:
    //
    //------------------------------------------------------------------

    // Fast add, if |x| is greater than or equals |y|:
    template<typename T> inline T fast_padd0(T x, T y, T& r1)
    {
        T r0 = x + y;
        T yt = r0 - x;
        r1 = y - yt;
        return r0;
    }

    // Fast subtract, if |x| greater or equals |y|:
    template<typename T> inline T fast_psub0(T x, T y, T& r1)
    {
        T r0 = x - y;
        T yt = x - r0;
        r1 = yt - y;
        return r0;
    }

    // Add, any x and y:
    template<typename T> inline T padd0(T x, T y, T& r1)
    {
        T r0 = x + y;
        T yt = r0 - x;
        T xt = r0 - yt;
        T ey = y - yt;
        T ex = x - xt;
        r1 = ey + ex;
        return r0;
    }

    // Subtract, any x and y:
    template<typename T> inline T psub0(T x, T y, T& r1)
    {
        T r0 = x - y;
        T yt = r0 - x;
        T xt = r0 - yt;
        T ey = y + yt;
        T ex = x - xt;
        r1 = ex - ey;
        return r0;
    }

    // Renormalization: just wrapper
    template<typename T> T renormalize(T x0, T x1, T& r1) { return      padd0(x0, x1, r1); }
    template<typename T> T fast_renorm(T x0, T x1, T& r1) { return fast_padd0(x0, x1, r1); }

    //------------------------------------------------------------------
    //
    // Exact a * b -> p + e
    //
    //------------------------------------------------------------------

    //
    // Dekker-Veltkamp algorithm without FMA
    //

    // Magic constant for psplit0 (below)
    //   2**27 + 1: if double or doublex
    //   2**12 + 1: if float  or floatx
    template<typename T> T psplit0_c();
    template<> inline float   psplit0_c() { return setallx<float>(4097.f); }
    template<> inline floatx  psplit0_c() { return setallx<floatx>(4097.f); }
    template<> inline double  psplit0_c() { return setallx<double>(134217729.); }
    template<> inline doublex psplit0_c() { return setallx<doublex>(134217729.); }

    // h+l=split(x), h is higher, l is lower parts
    template<typename T> inline T psplit0(T x, T& l)
    {
        T c = psplit0_c<T>();
        T a = c * x;
        T b = a - x;
        T h = a - b;
          l = x - h;
        return h;
    }

    // Exact r0 + r1 = x * y
    // Do not use fma()
    template<typename T> inline T nofma_pmul0(T x, T y, T& r1)
    {
        T x0, x1, y0, y1, r0, e0, e1, e2;
        r0 = x * y;
        x0 = psplit0(x, x1); // x = x0 + x1 exactly
        y0 = psplit0(y, y1);
        e0 = r0 - x0 * y0;   // note, that x0*y0 is exact
        e1 = e0 - x0 * y1;
        e2 = e1 - x1 * y0;
        r1 = x1 * y1 - e2;
        return r0;
    }

    //
    // Leverage of FMA
    //

    // Exact r0 + r1 = x * y
    template<typename T> inline T pmul0(T x, T y, T& r1)
    {
        T r0 = x * y;
        r1 = fmsub(x, y, r0);
        return r0;
    }

} // namespace tfcp

//======================================================================
#endif // TFCP_EXACT_H
