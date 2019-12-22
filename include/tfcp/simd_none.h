// 2019 (c) Evgeny Latkin
// Free for non-commercial and academic

#ifndef TFCP_SIMD_NONE_H
#define TFCP_SIMD_NONE_H
//----------------------------------------------------------------------
//
// Define "vector" types: actually scalar in this case (simd == none)
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

typedef float  floatx;
typedef double doublex;

#define  floatx_nlanes 1
#define doublex_nlanes 1

TFCP_INLINE float tfcp_addf(float x, float y) { return x + y; }
TFCP_INLINE float tfcp_subf(float x, float y) { return x - y; }
TFCP_INLINE float tfcp_mulf(float x, float y) { return x * y; }
TFCP_INLINE float tfcp_divf(float x, float y) { return x / y; }

TFCP_INLINE double tfcp_add(double x, double y) { return x + y; }
TFCP_INLINE double tfcp_sub(double x, double y) { return x - y; }
TFCP_INLINE double tfcp_mul(double x, double y) { return x * y; }
TFCP_INLINE double tfcp_div(double x, double y) { return x / y; }

TFCP_INLINE floatx tfcp_addfx(floatx x, floatx y) { return x + y; }
TFCP_INLINE floatx tfcp_subfx(floatx x, floatx y) { return x - y; }
TFCP_INLINE floatx tfcp_mulfx(floatx x, floatx y) { return x * y; }
TFCP_INLINE floatx tfcp_divfx(floatx x, floatx y) { return x / y; }

TFCP_INLINE doublex tfcp_addx(doublex x, doublex y) { return x + y; }
TFCP_INLINE doublex tfcp_subx(doublex x, doublex y) { return x - y; }
TFCP_INLINE doublex tfcp_mulx(doublex x, doublex y) { return x * y; }
TFCP_INLINE doublex tfcp_divx(doublex x, doublex y) { return x / y; }

#ifdef __cplusplus

    #include <cmath>

    inline float  tfcp_sqrtf(float  x) { return std::sqrt(x); }
    inline double tfcp_sqrt (double x) { return std::sqrt(x); }

#else

    #include <math.h>

    static float  tfcp_sqrtf(float  x) { return sqrtf(x); }
    static double tfcp_sqrt (double x) { return sqrt (x); }

#endif

TFCP_INLINE  floatx tfcp_sqrtfx(floatx  x) { return tfcp_sqrtf(x); }
TFCP_INLINE doublex tfcp_sqrtx (doublex x) { return tfcp_sqrt (x); }

//----------------------------------------------------------------------
#endif // TFCP_SIMD_NONE_H
