//======================================================================
// 2019 (c) Evgeny Latkin
// Licence: Apache 2.0 (http://www.apache.org/licenses/)
//======================================================================

#ifndef TFCP_SIMD_H
#define TFCP_SIMD_H
//======================================================================
//
// Define short-vector types: floatx, doublex
//
// Types naming like floatx refers to the OpenCL-like types, e.g. float4
//
// Suffix `x` means that you do not know vector length in advance: as it
// depends on the short-vector extension supported by the compiler.
//
// Determine actual length like following, e.g.:
//   int constexpr floatx_length = sizeof(floatx) / sizeof(float);
//
// Compiling with following macro would enable/disable some features:
//   g++ -DTFCP_NO_SIMD ... -- disable SIMD: only generic float/double
//   g++ -DTFCP_STD_FMA ... -- use std::fma() for generic float/double
//
// Vector extensions currently supported:
// - generic (no extension: floatx equals float, doublex is double)
// - Intel SSE2
// - Intel AVX
// - Intel AVX2
//
// Use compiler options to activate a vector extension, e.g.:
//   cl /arch:AVX2 ...
//   clang++ -mfma ...
//
//======================================================================

#ifndef TFCP_NO_SIMD

    #if defined(__GNUC__)

        #define TFCP_SIMD_GCC

        #if defined(__FMA__)
            #define TFCP_SIMD_AVX
            #define TFCP_SIMD_FMA
        #elif defined(__AVX__)
            #define TFCP_SIMD_AVX
        #elif defined(__SSE2__)
            #define TFCP_SIMD_SSE
        #else
            // OK: only generic float/double
        #endif

    #elif defined(_MSC_VER)

        #define TFCP_SIMD_MSC

        #if defined(__AVX2__)
            #define TFCP_SIMD_AVX
            #define TFCP_SIMD_FMA
        #elif defined(__AVX__)
            #define TFCP_SIMD_AVX
        #elif defined(__SSE2__)
            #define TFCP_SIMD_SSE
        #else
            // OK: only generic float/double
        #endif

    #else // another compiler

        // OK: only generic float/double

    #endif

#endif // TFCP_NO_SIMD

//----------------------------------------------------------------------
//
// Include headers: compiler and hardware specific
//
//----------------------------------------------------------------------

#if defined(TFCP_SIMD_AVX)

    #include <immintrin.h>

#elif defined(TFCP_SIMD_SSE)

    #include <emmintrin.h>

#endif

#if defined(TFCP_SIMD_AVX) || defined(TFCP_SIMD_SSE)
    #ifndef TFCP_SIMD_VEC
    #define TFCP_SIMD_VEC
    #endif
#endif

#if !defined(TFCP_SIMD_VEC) || defined(TFCP_STD_FMA)

    #include <cmath>

#endif

//----------------------------------------------------------------------
//
// Define floatx/doublex for target compiler and hardware
//
//----------------------------------------------------------------------

namespace tfcp {

#if defined(TFCP_SIMD_AVX)

    #if defined(TFCP_SIMD_GCC)

        typedef float  floatx  __attribute__((vector_size(32), aligned(32)));
        typedef double doublex __attribute__((vector_size(32), aligned(32)));

    #else // another compiler

        typedef __m256  floatx;
        typedef __m256d doublex;

        inline floatx operator + (floatx x, floatx y) { return _mm256_add_ps(x, y); }
        inline floatx operator - (floatx x, floatx y) { return _mm256_sub_ps(x, y); }
        inline floatx operator * (floatx x, floatx y) { return _mm256_mul_ps(x, y); }
        inline floatx operator / (floatx x, floatx y) { return _mm256_div_ps(x, y); }

        inline doublex operator + (doublex x, doublex y) { return _mm256_add_pd(x, y); }
        inline doublex operator - (doublex x, doublex y) { return _mm256_sub_pd(x, y); }
        inline doublex operator * (doublex x, doublex y) { return _mm256_mul_pd(x, y); }
        inline doublex operator / (doublex x, doublex y) { return _mm256_div_pd(x, y); }

        inline floatx  operator - (floatx  x) { return _mm256_sub_ps(_mm256_setzero_ps(), x); }
        inline doublex operator - (doublex x) { return _mm256_sub_pd(_mm256_setzero_pd(), x); }

        inline floatx  operator * (float  x, floatx  y) { return _mm256_mul_ps(_mm256_set1_ps(x), y); }
        inline doublex operator * (double x, doublex y) { return _mm256_mul_pd(_mm256_set1_pd(x), y); }

    #endif

#elif defined(TFCP_SIMD_SSE)

    #if defined(TFCP_SIMD_GCC)

        typedef float  floatx  __attribute__((vector_size(16), aligned(16)));
        typedef double doublex __attribute__((vector_size(16), aligned(16)));

    #else // another compiler

        typedef __m128  floatx;
        typedef __m128d doublex;

        inline floatx operator + (floatx x, floatx y) { return _mm_add_ps(x, y); }
        inline floatx operator - (floatx x, floatx y) { return _mm_sub_ps(x, y); }
        inline floatx operator * (floatx x, floatx y) { return _mm_mul_ps(x, y); }
        inline floatx operator / (floatx x, floatx y) { return _mm_div_ps(x, y); }

        inline doublex operator + (doublex x, doublex y) { return _mm_add_pd(x, y); }
        inline doublex operator - (doublex x, doublex y) { return _mm_sub_pd(x, y); }
        inline doublex operator * (doublex x, doublex y) { return _mm_mul_pd(x, y); }
        inline doublex operator / (doublex x, doublex y) { return _mm_div_pd(x, y); }

        inline floatx  operator - (floatx  x) { return _mm_sub_ps(_mm_setzero_ps(), x); }
        inline doublex operator - (doublex x) { return _mm_sub_pd(_mm_setzero_pd(), x); }

        inline floatx  operator * (float  x, floatx  y) { return _mm_mul_ps(_mm_set1_ps(x), y); }
        inline doublex operator * (double x, doublex y) { return _mm_mul_pd(_mm_set1_pd(x), y); }

    #endif

#else // no SIMD

    typedef float  floatx;
    typedef double doublex;

#endif

} // namespace tfcp

//----------------------------------------------------------------------
//
// Square root: hardware specific, either standard
//
//----------------------------------------------------------------------

namespace tfcp {

#if defined(TFCP_SIMD_AVX)

    inline floatx  sqrt(floatx  x) { return _mm256_sqrt_ps(x); }
    inline doublex sqrt(doublex x) { return _mm256_sqrt_pd(x); }

#elif defined(TFCP_SIMD_SSE)

    inline floatx  sqrt(floatx  x) { return _mm_sqrt_ps(x); }
    inline doublex sqrt(doublex x) { return _mm_sqrt_pd(x); }

#endif

#if defined(TFCP_SIMD_AVX) || defined(TFCP_SIMD_SSE)

    inline float sqrt(float x) {
        return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(x)));
    }

    inline double sqrt(double x) {
        return _mm_cvtsd_f64(_mm_sqrt_sd(_mm_set_sd(x), _mm_setzero_pd()));
    }

#else

    inline float  sqrt(float  x) { return std::sqrt(x); }
    inline double sqrt(double x) { return std::sqrt(x); }

#endif

} // namespace tfcp

//----------------------------------------------------------------------
//
// If FMA supported, we use fmsub(x, y, z) == correctly rounded x*y - z
//
// Note that std::fma appears useless for TFCP, as our target compilers
// would implement it with software if strict-math (which is too slow),
// either would replace fma(x, y, z) with x*y + z if fast-math options,
// so we cannot use fma(x, y, -x*y) for computing rounding error of x*y
//
// Thus by default, using of the standard std::fma(x, y, z) is disabled
//
//----------------------------------------------------------------------

namespace tfcp {

#if defined(TFCP_SIMD_FMA)

    inline floatx  fmsub(floatx  x, floatx  y, floatx  z) { return _mm256_fmsub_ps(x, y, z); }
    inline doublex fmsub(doublex x, doublex y, doublex z) { return _mm256_fmsub_pd(x, y, z); }

    #if !defined(TFCP_STD_FMA)

        inline float fmsub(float x, float y, float z) {
            __m128 result = _mm_fmsub_ss(_mm_set_ss(x),
                                         _mm_set_ss(y),
                                         _mm_set_ss(z));
            return _mm_cvtss_f32(result);
        }

        inline double fmsub(double x, double y, double z) {
            __m128d result = _mm_fmsub_sd(_mm_set_sd(x),
                                          _mm_set_sd(y),
                                          _mm_set_sd(z));
            return _mm_cvtsd_f64(result);
        }

    #endif

#endif

#if defined(TFCP_STD_FMA)

    inline float  fmsub(float  x, float  y, float  z) { return std::fma(x, y, -z); }
    inline double fmsub(double x, double y, double z) { return std::fma(x, y, -z); }

#endif

} // namespace tfcp

//======================================================================
#endif // TFCP_SIMD_H
