/* SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Copyright:
 *   2020      Evan Nemerson <evan@nemerson.com>
 *   2020      Christopher Moore <moore@free.fr>
 */

#if !defined(SIMDE_X86_AVX512_PERMUTEXVAR_H)
#define SIMDE_X86_AVX512_PERMUTEXVAR_H

#include "and.h"
#include "andnot.h"
#include "blend.h"
#include "mov.h"
#include "or.h"
#include "set1.h"
#include "slli.h"
#include "srli.h"
#include "test.h"
#include "types.h"

HEDLEY_DIAGNOSTIC_PUSH
SIMDE_DISABLE_UNWANTED_DIAGNOSTICS
SIMDE_BEGIN_DECLS_

SIMDE_FUNCTION_ATTRIBUTES
simde__m512i simde_mm512_permutexvar_epi16(simde__m512i idx, simde__m512i a)
{
#if defined(SIMDE_X86_AVX512BW_NATIVE)
    return _mm512_permutexvar_epi16(idx, a);
#else
    simde__m512i_private idx_ = simde__m512i_to_private(idx), a_ = simde__m512i_to_private(a), r_;

#if defined(SIMDE_ARM_NEON_A64V8_NATIVE)
    uint8x16x4_t table = {{a_.m128i_private[0].neon_u8, a_.m128i_private[1].neon_u8,
                           a_.m128i_private[2].neon_u8, a_.m128i_private[3].neon_u8}};
    uint16x8_t mask16 = vdupq_n_u16(0x001F);
    uint16x8_t byte_index16 = vdupq_n_u16(0x0100);

    SIMDE_VECTORIZE
    for (size_t i = 0; i < (sizeof(r_.m128i_private) / sizeof(r_.m128i_private[0])); i++) {
        uint16x8_t index16 = vandq_u16(idx_.m128i_private[i].neon_u16, mask16);
        index16 = vmulq_n_u16(index16, 0x0202);
        index16 = vaddq_u16(index16, byte_index16);
        r_.m128i_private[i].neon_u8 = vqtbl4q_u8(table, vreinterpretq_u8_u16(index16));
    }
#else
    SIMDE_VECTORIZE
    for (size_t i = 0; i < (sizeof(r_.i16) / sizeof(r_.i16[0])); i++) {
        r_.i16[i] = a_.i16[idx_.i16[i] & 0x1F];
    }
#endif

    return simde__m512i_from_private(r_);
#endif
}
#if defined(SIMDE_X86_AVX512BW_ENABLE_NATIVE_ALIASES)
#undef _mm512_permutexvar_epi16
#define _mm512_permutexvar_epi16(idx, a) simde_mm512_permutexvar_epi16(idx, a)
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde__m512i simde_mm512_mask_permutexvar_epi16(simde__m512i src, simde__mmask32 k,
                                                simde__m512i idx, simde__m512i a)
{
#if defined(SIMDE_X86_AVX512BW_NATIVE)
    return _mm512_mask_permutexvar_epi16(src, k, idx, a);
#else
    return simde_mm512_mask_mov_epi16(src, k, simde_mm512_permutexvar_epi16(idx, a));
#endif
}
#if defined(SIMDE_X86_AVX512BW_ENABLE_NATIVE_ALIASES)
#undef _mm512_mask_permutexvar_epi16
#define _mm512_mask_permutexvar_epi16(src, k, idx, a) \
    simde_mm512_mask_permutexvar_epi16(src, k, idx, a)
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde__m512i simde_mm512_maskz_permutexvar_epi16(simde__mmask32 k, simde__m512i idx, simde__m512i a)
{
#if defined(SIMDE_X86_AVX512BW_NATIVE)
    return _mm512_maskz_permutexvar_epi16(k, idx, a);
#else
    return simde_mm512_maskz_mov_epi16(k, simde_mm512_permutexvar_epi16(idx, a));
#endif
}
#if defined(SIMDE_X86_AVX512BW_ENABLE_NATIVE_ALIASES)
#undef _mm512_maskz_permutexvar_epi16
#define _mm512_maskz_permutexvar_epi16(k, idx, a) simde_mm512_maskz_permutexvar_epi16(k, idx, a)
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde__m512i simde_mm512_permutexvar_epi32(simde__m512i idx, simde__m512i a)
{
#if defined(SIMDE_X86_AVX512F_NATIVE)
    return _mm512_permutexvar_epi32(idx, a);
#else
    simde__m512i_private idx_ = simde__m512i_to_private(idx), a_ = simde__m512i_to_private(a), r_;

#if defined(SIMDE_ARM_NEON_A64V8_NATIVE)
    uint8x16x4_t table = {{a_.m128i_private[0].neon_u8, a_.m128i_private[1].neon_u8,
                           a_.m128i_private[2].neon_u8, a_.m128i_private[3].neon_u8}};
    uint32x4_t mask32 = vdupq_n_u32(0x0000000F);
    uint32x4_t byte_index32 = vdupq_n_u32(0x03020100);

    SIMDE_VECTORIZE
    for (size_t i = 0; i < (sizeof(r_.m128i_private) / sizeof(r_.m128i_private[0])); i++) {
        uint32x4_t index32 = vandq_u32(idx_.m128i_private[i].neon_u32, mask32);
        index32 = vmulq_n_u32(index32, 0x04040404);
        index32 = vaddq_u32(index32, byte_index32);
        r_.m128i_private[i].neon_u8 = vqtbl4q_u8(table, vreinterpretq_u8_u32(index32));
    }
#else
#if !defined(__INTEL_COMPILER)
    SIMDE_VECTORIZE
#endif
    for (size_t i = 0; i < (sizeof(r_.i32) / sizeof(r_.i32[0])); i++) {
        r_.i32[i] = a_.i32[idx_.i32[i] & 0x0F];
    }
#endif

    return simde__m512i_from_private(r_);
#endif
}
#if defined(SIMDE_X86_AVX512F_ENABLE_NATIVE_ALIASES)
#undef _mm512_permutexvar_epi32
#define _mm512_permutexvar_epi32(idx, a) simde_mm512_permutexvar_epi32(idx, a)
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde__m512i simde_mm512_mask_permutexvar_epi32(simde__m512i src, simde__mmask16 k,
                                                simde__m512i idx, simde__m512i a)
{
#if defined(SIMDE_X86_AVX512F_NATIVE)
    return _mm512_mask_permutexvar_epi32(src, k, idx, a);
#else
    return simde_mm512_mask_mov_epi32(src, k, simde_mm512_permutexvar_epi32(idx, a));
#endif
}
#if defined(SIMDE_X86_AVX512F_ENABLE_NATIVE_ALIASES)
#undef _mm512_mask_permutexvar_epi32
#define _mm512_mask_permutexvar_epi32(src, k, idx, a) \
    simde_mm512_mask_permutexvar_epi32(src, k, idx, a)
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde__m512i simde_mm512_maskz_permutexvar_epi32(simde__mmask16 k, simde__m512i idx, simde__m512i a)
{
#if defined(SIMDE_X86_AVX512F_NATIVE)
    return _mm512_maskz_permutexvar_epi32(k, idx, a);
#else
    return simde_mm512_maskz_mov_epi32(k, simde_mm512_permutexvar_epi32(idx, a));
#endif
}
#if defined(SIMDE_X86_AVX512F_ENABLE_NATIVE_ALIASES)
#undef _mm512_maskz_permutexvar_epi32
#define _mm512_maskz_permutexvar_epi32(k, idx, a) simde_mm512_maskz_permutexvar_epi32(k, idx, a)
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde__m512i simde_mm512_permutexvar_epi64(simde__m512i idx, simde__m512i a)
{
#if defined(SIMDE_X86_AVX512F_NATIVE)
    return _mm512_permutexvar_epi64(idx, a);
#else
    simde__m512i_private idx_ = simde__m512i_to_private(idx), a_ = simde__m512i_to_private(a), r_;

#if !defined(__INTEL_COMPILER)
    SIMDE_VECTORIZE
#endif
    for (size_t i = 0; i < (sizeof(r_.i64) / sizeof(r_.i64[0])); i++) {
        r_.i64[i] = a_.i64[idx_.i64[i] & 7];
    }

    return simde__m512i_from_private(r_);
#endif
}
#if defined(SIMDE_X86_AVX512F_ENABLE_NATIVE_ALIASES)
#undef _mm512_permutexvar_epi64
#define _mm512_permutexvar_epi64(idx, a) simde_mm512_permutexvar_epi64(idx, a)
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde__m512i simde_mm512_mask_permutexvar_epi64(simde__m512i src, simde__mmask8 k, simde__m512i idx,
                                                simde__m512i a)
{
#if defined(SIMDE_X86_AVX512F_NATIVE)
    return _mm512_mask_permutexvar_epi64(src, k, idx, a);
#else
    return simde_mm512_mask_mov_epi64(src, k, simde_mm512_permutexvar_epi64(idx, a));
#endif
}
#if defined(SIMDE_X86_AVX512F_ENABLE_NATIVE_ALIASES)
#undef _mm512_mask_permutexvar_epi64
#define _mm512_mask_permutexvar_epi64(src, k, idx, a) \
    simde_mm512_mask_permutexvar_epi64(src, k, idx, a)
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde__m512i simde_mm512_maskz_permutexvar_epi64(simde__mmask8 k, simde__m512i idx, simde__m512i a)
{
#if defined(SIMDE_X86_AVX512F_NATIVE)
    return _mm512_maskz_permutexvar_epi64(k, idx, a);
#else
    return simde_mm512_maskz_mov_epi64(k, simde_mm512_permutexvar_epi64(idx, a));
#endif
}
#if defined(SIMDE_X86_AVX512F_ENABLE_NATIVE_ALIASES)
#undef _mm512_maskz_permutexvar_epi64
#define _mm512_maskz_permutexvar_epi64(k, idx, a) simde_mm512_maskz_permutexvar_epi64(k, idx, a)
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde__m512i simde_mm512_permutexvar_epi8(simde__m512i idx, simde__m512i a)
{
#if defined(SIMDE_X86_AVX512VBMI_NATIVE)
    return _mm512_permutexvar_epi8(idx, a);
#elif defined(SIMDE_X86_AVX512BW_NATIVE) && defined(SIMDE_X86_AVX512VL_NATIVE)
    simde__m512i hilo, hi, lo, hi2, lo2, idx2;
    simde__m512i ones = simde_mm512_set1_epi8(1);
    simde__m512i low_bytes = simde_mm512_set1_epi16(0x00FF);

    idx2 = simde_mm512_srli_epi16(idx, 1);
    hilo = simde_mm512_permutexvar_epi16(idx2, a);
    simde__mmask64 mask = simde_mm512_test_epi8_mask(idx, ones);
    lo = simde_mm512_and_si512(hilo, low_bytes);
    hi = simde_mm512_srli_epi16(hilo, 8);

    idx2 = simde_mm512_srli_epi16(idx, 9);
    hilo = simde_mm512_permutexvar_epi16(idx2, a);
    lo2 = simde_mm512_slli_epi16(hilo, 8);
    hi2 = simde_mm512_andnot_si512(low_bytes, hilo);

    lo = simde_mm512_or_si512(lo, lo2);
    hi = simde_mm512_or_si512(hi, hi2);

    return simde_mm512_mask_blend_epi8(mask, lo, hi);
#else
    simde__m512i_private idx_ = simde__m512i_to_private(idx), a_ = simde__m512i_to_private(a), r_;

#if defined(SIMDE_ARM_NEON_A64V8_NATIVE)
    uint8x16x4_t table = {{a_.m128i_private[0].neon_u8, a_.m128i_private[1].neon_u8,
                           a_.m128i_private[2].neon_u8, a_.m128i_private[3].neon_u8}};
    uint8x16_t mask = vdupq_n_u8(0x3F);

    SIMDE_VECTORIZE
    for (size_t i = 0; i < (sizeof(r_.m128i_private) / sizeof(r_.m128i_private[0])); i++) {
        r_.m128i_private[i].neon_u8 =
            vqtbl4q_u8(table, vandq_u8(idx_.m128i_private[i].neon_u8, mask));
    }
#else
    SIMDE_VECTORIZE
    for (size_t i = 0; i < (sizeof(r_.i8) / sizeof(r_.i8[0])); i++) {
        r_.i8[i] = a_.i8[idx_.i8[i] & 0x3F];
    }
#endif

    return simde__m512i_from_private(r_);
#endif
}
#if defined(SIMDE_X86_AVX512VBMI_ENABLE_NATIVE_ALIASES)
#undef _mm512_permutexvar_epi8
#define _mm512_permutexvar_epi8(idx, a) simde_mm512_permutexvar_epi8(idx, a)
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde__m512i simde_mm512_mask_permutexvar_epi8(simde__m512i src, simde__mmask64 k, simde__m512i idx,
                                               simde__m512i a)
{
#if defined(SIMDE_X86_AVX512VBMI_NATIVE)
    return _mm512_mask_permutexvar_epi8(src, k, idx, a);
#else
    return simde_mm512_mask_mov_epi8(src, k, simde_mm512_permutexvar_epi8(idx, a));
#endif
}
#if defined(SIMDE_X86_AVX512VBMI_ENABLE_NATIVE_ALIASES)
#undef _mm512_mask_permutexvar_epi8
#define _mm512_mask_permutexvar_epi8(src, k, idx, a) \
    simde_mm512_mask_permutexvar_epi8(src, k, idx, a)
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde__m512i simde_mm512_maskz_permutexvar_epi8(simde__mmask64 k, simde__m512i idx, simde__m512i a)
{
#if defined(SIMDE_X86_AVX512VBMI_NATIVE)
    return _mm512_maskz_permutexvar_epi8(k, idx, a);
#else
    return simde_mm512_maskz_mov_epi8(k, simde_mm512_permutexvar_epi8(idx, a));
#endif
}
#if defined(SIMDE_X86_AVX512VBMI_ENABLE_NATIVE_ALIASES)
#undef _mm512_maskz_permutexvar_epi8
#define _mm512_maskz_permutexvar_epi8(k, idx, a) simde_mm512_maskz_permutexvar_epi8(k, idx, a)
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde__m512d simde_mm512_permutexvar_pd(simde__m512i idx, simde__m512d a)
{
#if defined(SIMDE_X86_AVX512F_NATIVE)
    return _mm512_permutexvar_pd(idx, a);
#else
    simde__m512i_private idx_ = simde__m512i_to_private(idx);
    simde__m512d_private a_ = simde__m512d_to_private(a), r_;

#if !defined(__INTEL_COMPILER)
    SIMDE_VECTORIZE
#endif
    for (size_t i = 0; i < (sizeof(r_.f64) / sizeof(r_.f64[0])); i++) {
        r_.f64[i] = a_.f64[idx_.i64[i] & 7];
    }

    return simde__m512d_from_private(r_);
#endif
}
#if defined(SIMDE_X86_AVX512F_ENABLE_NATIVE_ALIASES)
#undef _mm512_permutexvar_pd
#define _mm512_permutexvar_pd(idx, a) simde_mm512_permutexvar_pd(idx, a)
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde__m512d simde_mm512_mask_permutexvar_pd(simde__m512d src, simde__mmask8 k, simde__m512i idx,
                                             simde__m512d a)
{
#if defined(SIMDE_X86_AVX512F_NATIVE)
    return _mm512_mask_permutexvar_pd(src, k, idx, a);
#else
    return simde_mm512_mask_mov_pd(src, k, simde_mm512_permutexvar_pd(idx, a));
#endif
}
#if defined(SIMDE_X86_AVX512F_ENABLE_NATIVE_ALIASES)
#undef _mm512_mask_permutexvar_pd
#define _mm512_mask_permutexvar_pd(src, k, idx, a) simde_mm512_mask_permutexvar_pd(src, k, idx, a)
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde__m512d simde_mm512_maskz_permutexvar_pd(simde__mmask8 k, simde__m512i idx, simde__m512d a)
{
#if defined(SIMDE_X86_AVX512F_NATIVE)
    return _mm512_maskz_permutexvar_pd(k, idx, a);
#else
    return simde_mm512_maskz_mov_pd(k, simde_mm512_permutexvar_pd(idx, a));
#endif
}
#if defined(SIMDE_X86_AVX512F_ENABLE_NATIVE_ALIASES)
#undef _mm512_maskz_permutexvar_pd
#define _mm512_maskz_permutexvar_pd(k, idx, a) simde_mm512_maskz_permutexvar_pd(k, idx, a)
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde__m512 simde_mm512_permutexvar_ps(simde__m512i idx, simde__m512 a)
{
#if defined(SIMDE_X86_AVX512F_NATIVE)
    return _mm512_permutexvar_ps(idx, a);
#else
    simde__m512i_private idx_ = simde__m512i_to_private(idx);
    simde__m512_private a_ = simde__m512_to_private(a), r_;

#if !defined(__INTEL_COMPILER)
    SIMDE_VECTORIZE
#endif
    for (size_t i = 0; i < (sizeof(r_.f32) / sizeof(r_.f32[0])); i++) {
        r_.f32[i] = a_.f32[idx_.i32[i] & 0x0F];
    }

    return simde__m512_from_private(r_);
#endif
}
#if defined(SIMDE_X86_AVX512F_ENABLE_NATIVE_ALIASES)
#undef _mm512_permutexvar_ps
#define _mm512_permutexvar_ps(idx, a) simde_mm512_permutexvar_ps(idx, a)
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde__m512 simde_mm512_mask_permutexvar_ps(simde__m512 src, simde__mmask16 k, simde__m512i idx,
                                            simde__m512 a)
{
#if defined(SIMDE_X86_AVX512F_NATIVE)
    return _mm512_mask_permutexvar_ps(src, k, idx, a);
#else
    return simde_mm512_mask_mov_ps(src, k, simde_mm512_permutexvar_ps(idx, a));
#endif
}
#if defined(SIMDE_X86_AVX512F_ENABLE_NATIVE_ALIASES)
#undef _mm512_mask_permutexvar_ps
#define _mm512_mask_permutexvar_ps(src, k, idx, a) simde_mm512_mask_permutexvar_ps(src, k, idx, a)
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde__m512 simde_mm512_maskz_permutexvar_ps(simde__mmask16 k, simde__m512i idx, simde__m512 a)
{
#if defined(SIMDE_X86_AVX512F_NATIVE)
    return _mm512_maskz_permutexvar_ps(k, idx, a);
#else
    return simde_mm512_maskz_mov_ps(k, simde_mm512_permutexvar_ps(idx, a));
#endif
}
#if defined(SIMDE_X86_AVX512F_ENABLE_NATIVE_ALIASES)
#undef _mm512_maskz_permutexvar_ps
#define _mm512_maskz_permutexvar_ps(k, idx, a) simde_mm512_maskz_permutexvar_ps(k, idx, a)
#endif

SIMDE_END_DECLS_
HEDLEY_DIAGNOSTIC_POP

#endif /* !defined(SIMDE_X86_AVX512_PERMUTEXVAR_H) */
