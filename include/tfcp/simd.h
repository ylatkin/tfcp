// 2019 (c) Evgeny Latkin
// Free for non-commercial and academic

#ifndef TFCP_SIMD_H
#define TFCP_SIMD_H

//--------------------------------------------------------------

#if __AVX2__

    #ifndef TFCP_NO_AVX
        #define TFCP_AVX

        #ifndef TFCP_NO_FMA
            #define TFCP_FMA
        #endif
    #endif

#elif __AVX__

    #ifndef TFCP_NO_AVX
        #define TFCP_AVX
    #endif

    #ifdef TFCP_FMA
        #error TFCP_FMA: invalid here!
    #endif

#elif __SSE2__

    #ifndef TFCP_NO_SSE
        #define TFCP_SSE
    #endif

    #ifdef TFCP_AVX
        #error TFCP_AVX: invalid here!
    #endif

    #ifdef TFCP_FMA
        #error TFCP_FMA: invalid here!
    #endif

#else

    #ifdef TFCP_SSE
        #error TFCP_SSE: invalid here!
    #endif

    #ifdef TFCP_AVX
        #error TFCP_AVX: invalid here!
    #endif

    #ifdef TFCP_FMA
        #error TFCP_FMA: invalid here!
    #endif

#endif

//--------------------------------------------------------------

#if defined(__GNUC__)

    #define TFCP_GNUC

#elif defined(_MSC_VER)

    #define TFCP_MSC

#else

    #error Unsupported compiler!

#endif

//--------------------------------------------------------------

#if defined(TFCP_AVX)

#include <tfcp/simd_avx.h>

#elif defined(TFCP_SSE)

#include <tfcp/simd_sse.h>

#else // no vector extension

#include <tfcp/simd_none.h>

#endif

//--------------------------------------------------------------

#endif // TFCP_SIMD_H
