// 2019 (c) Evgeny Latkin
// Free for non-commercial and academic

#ifndef TFCP_SIMD_SSE_H
#define TFCP_SIMD_SSE_H

//----------------------------------------------------------------------
//
// Define "vector" types: leverage Intel SSE2
//
//----------------------------------------------------------------------

#include <tfcp/defs.h>

TFCP_INLINE float tfcp_addf(float x, float y) { return x + y; }
TFCP_INLINE float tfcp_subf(float x, float y) { return x - y; }
TFCP_INLINE float tfcp_mulf(float x, float y) { return x * y; }
TFCP_INLINE float tfcp_divf(float x, float y) { return x / y; }

TFCP_INLINE double tfcp_add(double x, double y) { return x + y; }
TFCP_INLINE double tfcp_sub(double x, double y) { return x - y; }
TFCP_INLINE double tfcp_mul(double x, double y) { return x * y; }
TFCP_INLINE double tfcp_div(double x, double y) { return x / y; }

#ifdef __cplusplus

    #include <cmath>

    inline float  tfcp_sqrtf(float  x) { return std::sqrt(x); }
    inline double tfcp_sqrt (double x) { return std::sqrt(x); }

#else

    #include <math.h>

    static float  tfcp_sqrtf(float  x) { return sqrtf(x); }
    static double tfcp_sqrt (double x) { return sqrt (x); }

#endif

//----------------------------------------------------------------------

#include <emmintrin.h>

#define  floatx_nlanes 4
#define doublex_nlanes 2

#ifdef TFCP_GNUC

    typedef  float    floatx   __attribute__((__vector_size__(16)));
    typedef  double  doublex   __attribute__((__vector_size__(16)));

    TFCP_INLINE floatx tfcp_addfx(floatx x, floatx y) { return x + y; }
    TFCP_INLINE floatx tfcp_subfx(floatx x, floatx y) { return x - y; }
    TFCP_INLINE floatx tfcp_mulfx(floatx x, floatx y) { return x * y; }
    TFCP_INLINE floatx tfcp_divfx(floatx x, floatx y) { return x / y; }

    TFCP_INLINE doublex tfcp_addx(doublex x, doublex y) { return x + y; }
    TFCP_INLINE doublex tfcp_subx(doublex x, doublex y) { return x - y; }
    TFCP_INLINE doublex tfcp_mulx(doublex x, doublex y) { return x * y; }
    TFCP_INLINE doublex tfcp_divx(doublex x, doublex y) { return x / y; }

//  TFCP_INLINE floatx  tfcp_sqrtf(floatx  x) { __m128  t = _mm_sqrt_ps(*(__m128 *)& x); return *(floatx *)& t; }
//  TFCP_INLINE doublex tfcp_sqrt (doublex x) { __m128d t = _mm_sqrt_pd(*(__m128d*)& x); return *(doublex*)& t; }

    TFCP_INLINE floatx  tfcp_sqrtf(floatx  x) { return _mm_sqrt_ps(x); }
    TFCP_INLINE doublex tfcp_sqrt (doublex x) { return _mm_sqrt_pd(x); }

#else

    typedef  __m128    floatx;
    typedef  __m128d  doublex;

    TFCP_INLINE floatx tfcp_addfx(floatx x, floatx y) { return _mm_add_ps(x, y); }
    TFCP_INLINE floatx tfcp_subfx(floatx x, floatx y) { return _mm_sub_ps(x, y); }
    TFCP_INLINE floatx tfcp_mulfx(floatx x, floatx y) { return _mm_mul_ps(x, y); }
    TFCP_INLINE floatx tfcp_divfx(floatx x, floatx y) { return _mm_div_ps(x, y); }

    TFCP_INLINE doublex tfcp_addx(doublex x, doublex y) { return _mm_add_pd(x, y); }
    TFCP_INLINE doublex tfcp_subx(doublex x, doublex y) { return _mm_sub_pd(x, y); }
    TFCP_INLINE doublex tfcp_mulx(doublex x, doublex y) { return _mm_mul_pd(x, y); }
    TFCP_INLINE doublex tfcp_divx(doublex x, doublex y) { return _mm_div_pd(x, y); }

    TFCP_INLINE floatx  tfcp_sqrtf(floatx  x) { return _mm_sqrt_ps(x); }
    TFCP_INLINE doublex tfcp_sqrt (doublex x) { return _mm_sqrt_pd(x); }

#endif

//----------------------------------------------------------------------
#endif // TFCP_SIMD_SSE_H
