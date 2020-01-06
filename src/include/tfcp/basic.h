//======================================================================
// 2019-2020 (c) Evgeny Latkin
// License: Apache 2.0 (http://www.apache.org/licenses/)
//======================================================================

#ifndef TFCP_BASIC_H
#define TFCP_BASIC_H
//======================================================================
//
// Implementation of basic twofold/coupled operations
//
// C++ templates, main scalar types: float, double
//
// Additionally, short-vector types: floatx, doublex
//
// Typical interface:
//   T tadd(T x0, T x1, T y0, T y1, T& z1);
//
// Parameters:
//   x = x0 + x1 unevaluated
//   y = y0 + y1
//
// Returns:
//   z = z0 + z1
//
//======================================================================

#include <tfcp/exact.h>

namespace tfcp {

    //======================================================================
    //
    // Twofold arithmetic
    //
    //======================================================================

    //------------------------------------------------------------------
    //
    //  Twofold: add
    //
    //------------------------------------------------------------------

    // Twofold add, both x and y are twofold:
    template<typename T> inline T tadd(T x0, T x1, T y0, T y1, T& z1)
    {
        T r0, r1;
        r0 = padd0(x0, y0, r1);  // r0 = round(x0 + y0)
                                 // r1 = error(x0 + y0)
        z1 = r1 + (x1 + y1);
        return r0;
    }

    // Twofold add, x is twofold, y is dotted:
    template<typename T> inline T tadd1(T x0, T x1, T y0, T& z1)
    {
        T r0, r1;
        r0 = padd0(x0, y0, r1);  // r0 = round(x0 + y0)
                                 // r1 = error(x0 + y0)
        z1 = r1 + x1;
        return r0;
    }

    // Twofold add, x is dotted, y is twofold:
    template<typename T> inline T tadd2(T x0, T y0, T y1, T& z1)
    {
        T r0, r1;
        r0 = padd0(x0, y0, r1);  // r0 = round(x0 + y0)
                                 // r1 = error(x0 + y0)
        z1 = r1 + y1;
        return r0;
    }

    // Twofold add, x and y are dotted:
    template<typename T> inline T tadd0(T x0, T y0, T& z1)
    {
        return padd0(x0, y0, z1);
    }

    //------------------------------------------------------------------
    //
    //  Twofold: subtract
    //
    //------------------------------------------------------------------

    // Twofold subtract, both x and y are twofold:
    template<typename T> inline T tsub(T x0, T x1, T y0, T y1, T& z1)
    {
        T r0, r1;
        r0 = psub0(x0, y0, r1);  // r0 = round(x0 - y0)
                                 // r1 = error(x0 - y0)
        z1 = r1 + (x1 - y1);
        return r0;
    }

    // Twofold subtract, x is twofold, y is dotted:
    template<typename T> inline T tsub1(T x0, T x1, T y0, T& z1)
    {
        T r0, r1;
        r0 = psub0(x0, y0, r1);  // r0 = round(x0 - y0)
                                 // r1 = error(x0 - y0)
        z1 = r1 + x1;
        return r0;
    }

    // Twofold subtract, x is dotted, y is twofold:
    template<typename T> inline T tsub2(T x0, T y0, T y1, T& z1)
    {
        T r0, r1;
        r0 = psub0(x0, y0, r1);  // r0 = round(x0 - y0)
                                 // r1 = error(x0 - y0)
        z1 = r1 - y1;
        return r0;
    }

    // Twofold subtract, x and y are dotted:
    template<typename T> inline T tsub0(T x0, T y0, T& z1)
    {
        return psub0(x0, y0, z1);
    }

    //------------------------------------------------------------------
    //
    //  Twofold: multiply
    //
    //------------------------------------------------------------------

    // Twofold: z0 + z1 = (x0 + x1) * (y0 + y1)
    template<typename T> inline T tmul(T x0, T x1, T y0, T y1, T& z1)
    {
        T r1;
        T r0 = pmul0(x0, y0, r1); // r0 = round(x0 * y0)
                                  // r1 = error(x0 * y0)
        T p01 = x0 * y1;
        T p10 = x1 * y0;
        T p11 = x1 * y1;
        z1 = r1 + p11 + (p01 + p10);
        return r0;
    }

    // Twofold: z0 + z1 = (x0 + x1) * y0
    template<typename T> inline T tmul1(T x0, T x1, T y0, T& z1)
    {
        T r1;
        T r0 = pmul0(x0, y0, r1); // r0 = round(x0 * y0)
                                  // r1 = error(x0 * y0)
        T p10 = x1 * y0;
        z1 = r1 + p10;
        return r0;
    }

    // Twofold: z0 + z1 = x0 * (y0 + y1)
    template<typename T> inline T tmul2(T x0, T y0, T y1, T& z1)
    {
        T r1;
        T r0 = pmul0(x0, y0, r1); // r0 = round(x0 * y0)
                                  // r1 = error(x0 * y0)
        T p01 = x0 * y1;
        z1 = r1 + p01;
        return r0;
    }

    // Twofold: z0 + z1 = x0 + y0
    template<typename T> inline T tmul0(T x0, T y0, T& z1)
    {
        return pmul0(x0, y0, z1);
    }

    // Twofold: z0 + z1 = (x0 + x1) * (y0 + y1)
    // Special case if x and y are renormalized, so we can omit x1*y1
    template<typename T> inline T tmulp(T x0, T x1, T y0, T y1, T& z1)
    {
        T r1;
        T r0 = pmul0(x0, y0, r1); // r0 = round(x0 * y0)
                                  // r1 = error(x0 * y0)
        T p01 = x0 * y1;
        T p10 = x1 * y0;
        z1 = r1 + (p01 + p10);
        return r0;
    }

    //------------------------------------------------------------------
    //
    //  Twofold: divide
    //
    //------------------------------------------------------------------

    // Twofold: z0 + z1 = (x0 + x1) / (y0 + y1)
    template<typename T> inline T tdiv(T x0, T x1, T y0, T y1, T& z1)
    {
        T q0, r0, r1, r, y;
        q0 = x0 / y0;             // quotient
        r0 = fmadd(-q0, y0, x0);  // remainder
        r1 = fmadd(-q0, y1, x1);
        r = r0 + r1;              // main part of twofold r0 + r1
        y = y0 + y1;
        z1 = r / y;
        return q0;
    }

    // Twofold: z0 + z1 = (x0 + x1) / y0
    template<typename T> inline T tdiv1(T x0, T x1, T y0, T& z1)
    {
        T q0, r0, r1, r;
        q0 = x0 / y0;             // quotient
        r0 = fmadd(-q0, y0, x0);  // remainder
        r1 = x1;
        r = r0 + r1;              // main part
        z1 = r / y0;
        return q0;
    }

    // Twofold: z0 + z1 = x0 / (y0 + y1)
    template<typename T> inline T tdiv2(T x0, T y0, T y1, T& z1)
    {
        T q0, r0, r1, r, y;
        q0 = x0 / y0;             // quotient
        r0 = fmadd(-q0, y0, x0);  // remainder
        r1 =       -q0 * y1;
        r = r0 + r1;              // main part of twofold r0 + r1
        y = y0 + y1;
        z1 = r / y;
        return q0;
    }

    // Twofold: z0 + z1 = x0 / y0
    template<typename T> inline T tdiv0(T x0, T y0, T& z1)
    {
        T q0, r0, r1;
        q0 = x0 / y0;             // quotient
        r0 = fmadd(-q0, y0, x0);  // remainder
        z1 = r0 / y0;
        return q0;
    }

    // Twofold: z0 + z1 = (x0 + x1) / (y0 + y1)
    // Special case if x and y are renormalized
    template<typename T> inline T tdivp(T x0, T x1, T y0, T y1, T& z1)
    {
        T q0, r0, r1, r;
        q0 = x0 / y0;             // quotient
        r0 = fmadd(-q0, y0, x0);  // remainder
        r1 = fmadd(-q0, y1, x1);
        r = r0 + r1;              // main part of twofold r0 + r1
        z1 = r / y0;
        return q0;
    }

    //------------------------------------------------------------------
    //
    //  Twofold: square root
    //
    //------------------------------------------------------------------

    // Twofold: z0 + z1 = sqrt(x0)
    template<typename T> inline T tsqrt0(T x0, T& z1)
    {
        T r0, r1;
        r0 = sqrt(x0);
        r1 = fmadd(-r0, r0, x0);  // r0 + r1 = x0 - sqrt(x0)^2
        z1 = r1 / (r0 + r0);      // Newton iteration
        return r0;
    }

    // Twofold: z0 + z1 = sqrt(x0 + x1)
    // Special case if x is renormalized
    template<typename T> inline T tsqrtp(T x0, T x1, T& z1)
    {
        T r0, r1;
        r0 = sqrt(x0);
        r1 = fmadd(-r0, r0, x0) + x1;  // r = x - sqrt(x)^2
        z1 = r1 / (r0 + r0);           // Newton iteration
        return r0;
    }

    // Twofold: z0 + z1 = sqrt(x0 + x1)
    template<typename T> inline T tsqrt(T x0, T x1, T& z1)
    {
        T r0, u0, u1, v0, v1, w0, w1;
        r0 = sqrt(x0);
        u0 = padd0(x0, x1, u1);      // u = x renormalized
        v0 = tsqrtp(u0, u1, v1);     // v = sqrt(x)
        w0 = tsub1(v0, v1, r0, w1);  // w = sqrt(x) - r0
        z1 = w0 + w1;                // z1 = w main part
        return r0;
    }

    //======================================================================
    //
    // Coupled arithmetic
    //
    //======================================================================

    //------------------------------------------------------------------
    //
    //  Coupled: add
    //
    //------------------------------------------------------------------

    // Coupled: z0 + z1 = (x0 + x1) + (y0 + y1)
    template<typename T> inline T padd(T x0, T x1, T y0, T y1, T& z1)
    {
        T r0, r1;
        r0 = tadd(x0, x1, y0, y1, r1);   // r = x + y
        return fast_renorm(r0, r1, z1);  // z = r renormalized
    }

    // Coupled: z0 + z1 = (x0 + x1) + y0
    template<typename T> inline T padd1(T x0, T x1, T y0, T& z1)
    {
        T r0, r1;
        r0 = tadd1(x0, x1, y0, r1);      // r = x + y
        return fast_renorm(r0, r1, z1);  // z = r renormalized
    }

    // Coupled: z0 + z1 = x0 + (y0 + y1)
    template<typename T> inline T padd2(T x0, T y0, T y1, T& z1)
    {
        T r0, r1;
        r0 = tadd2(x0, y0, y1, r1);      // r = x + y
        return fast_renorm(r0, r1, z1);  // z = r renormalized
    }

    //------------------------------------------------------------------
    //
    //  Coupled: subtract
    //
    //------------------------------------------------------------------

    // Coupled: z0 + z1 = (x0 + x1) - (y0 + y1)
    template<typename T> inline T psub(T x0, T x1, T y0, T y1, T& z1)
    {
        T r0, r1;
        r0 = tsub(x0, x1, y0, y1, r1);   // r = x - y
        return fast_renorm(r0, r1, z1);  // z = r renormalized
    }

    // Coupled: z0 + z1 = (x0 + x1) - y0
    template<typename T> inline T psub1(T x0, T x1, T y0, T& z1)
    {
        T r0, r1;
        r0 = tsub1(x0, x1, y0, r1);      // r = x - y
        return fast_renorm(r0, r1, z1);  // z = r renormalized
    }

    // Coupled: z0 + z1 = x0 - (y0 + y1)
    template<typename T> inline T psub2(T x0, T y0, T y1, T& z1)
    {
        T r0, r1;
        r0 = tsub2(x0, y0, y1, r1);      // r = x - y
        return fast_renorm(r0, r1, z1);  // z = r renormalized
    }

    //------------------------------------------------------------------
    //
    //  Coupled: multiply
    //
    //------------------------------------------------------------------

    // Coupled: z0 + z1 = (x0 + x1) * (y0 + y1)
    template<typename T> inline T pmul(T x0, T x1, T y0, T y1, T& z1)
    {
        T r0, r1;
        r0 = tmulp(x0, x1, y0, y1, r1);  // r = x * y
        return fast_renorm(r0, r1, z1);  // z = r renormalized
    }

    // Coupled: z0 + z1 = (x0 + x1) * y0
    template<typename T> inline T pmul1(T x0, T x1, T y0, T& z1)
    {
        T r0, r1;
        r0 = tmul1(x0, x1, y0, r1);      // r = x * y
        return fast_renorm(r0, r1, z1);  // z = r renormalized
    }

    // Coupled: z0 + z1 = x0 * (y0 + y1)
    template<typename T> inline T pmul2(T x0, T x1, T y0, T y1, T& z1)
    {
        T r0, r1;
        r0 = tmul2(x0, y0, y1, r1);      // r = x * y
        return fast_renorm(r0, r1, z1);  // z = r renormalized
    }

    //------------------------------------------------------------------
    //
    //  Coupled: divide
    //
    //------------------------------------------------------------------

    // Coupled: z0 + z1 = (x0 + x1) / (y0 + y1)
    template<typename T> inline T pdiv(T x0, T x1, T y0, T y1, T& z1)
    {
        T r0, r1;
        r0 = tdivp(x0, x1, y0, y1, r1);  // r = x / y
        return fast_renorm(r0, r1, z1);  // z = r renormalized
    }

    // Coupled: z0 + z1 = (x0 + x1) / y0
    template<typename T> inline T pdiv1(T x0, T x1, T y0, T& z1)
    {
        T r0, r1;
        r0 = tdiv1(x0, x1, y0, r1);      // r = x / y
        return fast_renorm(r0, r1, z1);  // z = r renormalized
    }

    // Coupled: z0 + z1 = x0 / (y0 + y1)
    template<typename T> inline T pdiv2(T x0, T y0, T y1, T& z1)
    {
        T r0, r1;
        r0 = tdiv2(x0, y0, y1, r1);      // r = x / y
        return fast_renorm(r0, r1, z1);  // z = r renormalized
    }

    // Coupled: z0 + z1 = x0 / y0
    template<typename T> inline T pdiv0(T x0, T y0, T& z1)
    {
        return tdiv0(x0, y0, z1); // no need to renormalize
    }

    //------------------------------------------------------------------
    //
    //  Coupled: square root
    //
    //------------------------------------------------------------------

    // Coupled: z0 + z1 = sqrt(x0 + x1)
    template<typename T> inline T psqrt(T x0, T x1, T& z1)
    {
        T r0, r1;
        r0 = tsqrtp(x0, x1, r1);         // r = sqrt(x)
        return fast_renorm(r0, r1, z1);  // z = r renormalized
    }

    // Coupled: z0 + z1 = sqrt(x0)
    template<typename T> inline T psqrt0(T x0, T& z1)
    {
        return tsqrt0(x0, z1);  // no need to renormalize
    }

} // namespace tfcp

//======================================================================
#endif // TFCP_BASIC_H
