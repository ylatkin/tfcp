// 2019 (c) Evgeny Latkin
// Free for non-commercial and academic

#ifndef TFCP_SIMD_AVX_H
#define TFCP_SIMD_AVX_H
//----------------------------------------------------------------------
//
// Define "vector" types: leverage Intel AVX + maybe FMA (from AVX2)
//
//----------------------------------------------------------------------

#include <tfcp/defs.h>

#include <immintrin.h>

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

#ifdef TFCP_FMA

    inline float tfcp_fmaddf(float x, float y, float z) {
        __m128 t = _mm_fmadd_ss(_mm_set_ss(x),
                                _mm_set_ss(y),
                                _mm_set_ss(z));
        return _mm_cvtss_f32(t);
    }

    inline float tfcp_fmsubf(float x, float y, float z) {
        __m128 t = _mm_fmsub_ss(_mm_set_ss(x),
                                _mm_set_ss(y),
                                _mm_set_ss(z));
        return _mm_cvtss_f32(t);
    }

    inline double tfcp_fmadd(double x, double y, double z) {
        __m128d t = _mm_fmadd_sd(_mm_set_sd(x),
                                 _mm_set_sd(y),
                                 _mm_set_sd(z));
        return _mm_cvtsd_f64(t);
    }

    inline double tfcp_fmsub(double x, double y, double z) {
        __m128d t = _mm_fmsub_sd(_mm_set_sd(x),
                                 _mm_set_sd(y),
                                 _mm_set_sd(z));
        return _mm_cvtsd_f64(t);
    }

#endif // TFCP_FMA


//----------------------------------------------------------------------

#define  floatx_nlanes 8
#define doublex_nlanes 4

#ifdef TFCP_GNUC

    typedef  float    floatx   __attribute__((__vector_size__(32)));
    typedef  double  doublex   __attribute__((__vector_size__(32)));

    TFCP_INLINE floatx tfcp_addfx(floatx x, floatx y) { return x + y; }
    TFCP_INLINE floatx tfcp_subfx(floatx x, floatx y) { return x - y; }
    TFCP_INLINE floatx tfcp_mulfx(floatx x, floatx y) { return x * y; }
    TFCP_INLINE floatx tfcp_divfx(floatx x, floatx y) { return x / y; }

    TFCP_INLINE doublex tfcp_addx(doublex x, doublex y) { return x + y; }
    TFCP_INLINE doublex tfcp_subx(doublex x, doublex y) { return x - y; }
    TFCP_INLINE doublex tfcp_mulx(doublex x, doublex y) { return x * y; }
    TFCP_INLINE doublex tfcp_divx(doublex x, doublex y) { return x / y; }

//  TFCP_INLINE floatx  tfcp_sqrtf(floatx  x) { __m256  t = _mm_sqrt_ps(*(__m256 *)& x); return *(floatx *)& t; }
//  TFCP_INLINE doublex tfcp_sqrt (doublex x) { __m256d t = _mm_sqrt_pd(*(__m256d*)& x); return *(doublex*)& t; }

    TFCP_INLINE floatx  tfcp_sqrtf(floatx  x) { return _mm256_sqrt_ps(x); }
    TFCP_INLINE doublex tfcp_sqrt (doublex x) { return _mm256_sqrt_pd(x); }

#else

    typedef  __m256    floatx;
    typedef  __m256d  doublex;

    TFCP_INLINE floatx tfcp_addfx(floatx x, floatx y) { return _mm256_add_ps(x, y); }
    TFCP_INLINE floatx tfcp_subfx(floatx x, floatx y) { return _mm256_sub_ps(x, y); }
    TFCP_INLINE floatx tfcp_mulfx(floatx x, floatx y) { return _mm256_mul_ps(x, y); }
    TFCP_INLINE floatx tfcp_divfx(floatx x, floatx y) { return _mm256_div_ps(x, y); }

    TFCP_INLINE doublex tfcp_addx(doublex x, doublex y) { return _mm256_add_pd(x, y); }
    TFCP_INLINE doublex tfcp_subx(doublex x, doublex y) { return _mm256_sub_pd(x, y); }
    TFCP_INLINE doublex tfcp_mulx(doublex x, doublex y) { return _mm256_mul_pd(x, y); }
    TFCP_INLINE doublex tfcp_divx(doublex x, doublex y) { return _mm256_div_pd(x, y); }

    TFCP_INLINE floatx  tfcp_sqrtf(floatx  x) { return _mm256_sqrt_ps(x); }
    TFCP_INLINE doublex tfcp_sqrt (doublex x) { return _mm256_sqrt_pd(x); }

#endif

#ifdef TFCP_FMA

    TFCP_INLINE floatx tfcp_fmaddfx(floatx x, floatx y, floatx z) { return _mm256_fmadd_ps(x, y, z); }
    TFCP_INLINE floatx tfcp_fmsubfx(floatx x, floatx y, floatx z) { return _mm256_fmsub_ps(x, y, z); }

    TFCP_INLINE doublex tfcp_fmaddx(doublex x, doublex y, doublex z) { return _mm256_fmadd_pd(x, y, z); }
    TFCP_INLINE doublex tfcp_fmsubx(doublex x, doublex y, doublex z) { return _mm256_fmsub_pd(x, y, z); }

#endif

//----------------------------------------------------------------------
#endif // TFCP_SIMD_AVX_H
