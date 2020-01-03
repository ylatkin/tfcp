//======================================================================
// 2019 (c) Evgeny Latkin
// License: Apache 2.0 (http://www.apache.org/licenses/)
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
// depends on the short-vector extension supported by CPU and compiler.
//
// Determine actual length like following, e.g.:
//   size_t constexpr floatx_length = sizeof(floatx) / sizeof(float);
//
//----------------------------------------------------------------------
//
// Then, define fmsub(x, y, z) = x*y - z for scalar and vector floats
//
// Note that std::fma appears useless for TFCP as our target compilers
// appear to implement it with software even if hardware supports FMA;
// either replace fma(x, y, -z) with x*y - z if no hardware support --
// so that fma(x, y, -x*y) cannot compute the rounding error of x*y
//
// Thus we use hardware specific FMA instead of the std::fma(x, y, z)
//
//----------------------------------------------------------------------
//
// Current version requires AVX + FMA intrinsics for Intel/AMD processor
//
// Use compiler options to activate AVX + FMA vector extension, e.g.:
//   cl /arch:AVX2 ...
//   clang++ -mfma ...
//
// Supported compilers: g++/clang++ for Linux, cl for Windows
//
//======================================================================

#if defined(__GNUC__)
    // OK: g++ or clang for Linux (or maybe Intel's icc)

    #ifndef TFCP_SIMD_GCC
    #define TFCP_SIMD_GCC
    #endif

    #if !defined(__FMA__)
        #error Please enable FMA! (like: g++ -mfma ...)
    #endif

#elif defined(_MSC_VER)
    // OK: cl for Windows (or maybe Intel's icl)

    #ifndef TFCP_SIMD_MSC
    #define TFCP_SIMD_MSC
    #endif

    #if !defined(__AVX2__)
        #error Please enable FMA! (like: cl /arch:AVX2 ...)
    #endif

#else
    #error Unsupported compiler!
#endif

#ifndef TFCP_SIMD_AVX
#define TFCP_SIMD_AVX
#endif

#ifndef TFCP_SIMD_FMA
#define TFCP_SIMD_FMA
#endif

//----------------------------------------------------------------------
//
// Include headers: compiler and hardware specific
//
//----------------------------------------------------------------------

#if defined(TFCP_SIMD_AVX)

    #include <immintrin.h>

#else
    #error AVX is required!
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

    #elif defined(TFCP_SIMD_MSC)

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

    #else
        #error Unsupported compiler!
    #endif

#else
    #error AVX is required!
#endif

} // namespace tfcp

//----------------------------------------------------------------------
//
// Square root: hardware specific
//
//----------------------------------------------------------------------

namespace tfcp {

#if defined(TFCP_SIMD_AVX)

    inline floatx  sqrt(floatx  x) { return _mm256_sqrt_ps(x); }
    inline doublex sqrt(doublex x) { return _mm256_sqrt_pd(x); }

    inline float sqrt(float x) {
        return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(x)));
    }

    inline double sqrt(double x) {
        return _mm_cvtsd_f64(_mm_sqrt_sd(_mm_set_sd(x), _mm_setzero_pd()));
    }

#else
    #error AVX is required!
#endif

} // namespace tfcp

//----------------------------------------------------------------------
//
// Define fmsub(x, y, z) as correctly rounded x*y - z
//
//----------------------------------------------------------------------

namespace tfcp {

#if defined(TFCP_SIMD_AVX) && defined(TFCP_SIMD_FMA)

    inline floatx  fmsub(floatx  x, floatx  y, floatx  z) { return _mm256_fmsub_ps(x, y, z); }
    inline doublex fmsub(doublex x, doublex y, doublex z) { return _mm256_fmsub_pd(x, y, z); }

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

#else
    #error AVX + FMA is required!
#endif

} // namespace tfcp

//======================================================================
#endif // TFCP_SIMD_H
