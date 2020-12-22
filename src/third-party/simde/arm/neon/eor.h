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

#if !defined(SIMDE_ARM_NEON_EOR_H)
#define SIMDE_ARM_NEON_EOR_H

#include "types.h"

HEDLEY_DIAGNOSTIC_PUSH
SIMDE_DISABLE_UNWANTED_DIAGNOSTICS
SIMDE_BEGIN_DECLS_

SIMDE_FUNCTION_ATTRIBUTES
simde_int8x8_t simde_veor_s8(simde_int8x8_t a, simde_int8x8_t b)
{
#if defined(SIMDE_ARM_NEON_A32V7_NATIVE)
    return veor_s8(a, b);
#elif defined(SIMDE_X86_MMX_NATIVE)
    return _mm_xor_si64(a, b);
#else
    simde_int8x8_private r_, a_ = simde_int8x8_to_private(a), b_ = simde_int8x8_to_private(b);

#if defined(SIMDE_VECTOR_SUBSCRIPT_OPS)
    r_.values = a_.values ^ b_.values;
#else
    SIMDE_VECTORIZE
    for (size_t i = 0; i < (sizeof(r_.values) / sizeof(r_.values[0])); i++) {
        r_.values[i] = a_.values[i] ^ b_.values[i];
    }
#endif

    return simde_int8x8_from_private(r_);
#endif
}
#if defined(SIMDE_ARM_NEON_A32V7_ENABLE_NATIVE_ALIASES)
#undef veor_s8
#define veor_s8(a, b) simde_veor_s8((a), (b))
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde_int16x4_t simde_veor_s16(simde_int16x4_t a, simde_int16x4_t b)
{
#if defined(SIMDE_ARM_NEON_A32V7_NATIVE)
    return veor_s16(a, b);
#elif defined(SIMDE_X86_MMX_NATIVE)
    return _mm_xor_si64(a, b);
#else
    simde_int16x4_private r_, a_ = simde_int16x4_to_private(a), b_ = simde_int16x4_to_private(b);

#if defined(SIMDE_VECTOR_SUBSCRIPT_OPS)
    r_.values = a_.values ^ b_.values;
#else
    SIMDE_VECTORIZE
    for (size_t i = 0; i < (sizeof(r_.values) / sizeof(r_.values[0])); i++) {
        r_.values[i] = a_.values[i] ^ b_.values[i];
    }
#endif

    return simde_int16x4_from_private(r_);
#endif
}
#if defined(SIMDE_ARM_NEON_A32V7_ENABLE_NATIVE_ALIASES)
#undef veor_s16
#define veor_s16(a, b) simde_veor_s16((a), (b))
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde_int32x2_t simde_veor_s32(simde_int32x2_t a, simde_int32x2_t b)
{
#if defined(SIMDE_ARM_NEON_A32V7_NATIVE)
    return veor_s32(a, b);
#elif defined(SIMDE_X86_MMX_NATIVE)
    return _mm_xor_si64(a, b);
#else
    simde_int32x2_private r_, a_ = simde_int32x2_to_private(a), b_ = simde_int32x2_to_private(b);

#if defined(SIMDE_VECTOR_SUBSCRIPT_OPS)
    r_.values = a_.values ^ b_.values;
#else
    SIMDE_VECTORIZE
    for (size_t i = 0; i < (sizeof(r_.values) / sizeof(r_.values[0])); i++) {
        r_.values[i] = a_.values[i] ^ b_.values[i];
    }
#endif

    return simde_int32x2_from_private(r_);
#endif
}
#if defined(SIMDE_ARM_NEON_A32V7_ENABLE_NATIVE_ALIASES)
#undef veor_s32
#define veor_s32(a, b) simde_veor_s32((a), (b))
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde_int64x1_t simde_veor_s64(simde_int64x1_t a, simde_int64x1_t b)
{
#if defined(SIMDE_ARM_NEON_A32V7_NATIVE)
    return veor_s64(a, b);
#elif defined(SIMDE_X86_MMX_NATIVE)
    return _mm_xor_si64(a, b);
#else
    simde_int64x1_private r_, a_ = simde_int64x1_to_private(a), b_ = simde_int64x1_to_private(b);

#if defined(SIMDE_VECTOR_SUBSCRIPT_OPS)
    r_.values = a_.values ^ b_.values;
#else
    SIMDE_VECTORIZE
    for (size_t i = 0; i < (sizeof(r_.values) / sizeof(r_.values[0])); i++) {
        r_.values[i] = a_.values[i] ^ b_.values[i];
    }
#endif

    return simde_int64x1_from_private(r_);
#endif
}
#if defined(SIMDE_ARM_NEON_A32V7_ENABLE_NATIVE_ALIASES)
#undef veor_s64
#define veor_s64(a, b) simde_veor_s64((a), (b))
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde_uint8x8_t simde_veor_u8(simde_uint8x8_t a, simde_uint8x8_t b)
{
#if defined(SIMDE_ARM_NEON_A32V7_NATIVE)
    return veor_u8(a, b);
#elif defined(SIMDE_X86_MMX_NATIVE)
    return _mm_xor_si64(a, b);
#else
    simde_uint8x8_private r_, a_ = simde_uint8x8_to_private(a), b_ = simde_uint8x8_to_private(b);

#if defined(SIMDE_VECTOR_SUBSCRIPT_OPS)
    r_.values = a_.values ^ b_.values;
#else
    SIMDE_VECTORIZE
    for (size_t i = 0; i < (sizeof(r_.values) / sizeof(r_.values[0])); i++) {
        r_.values[i] = a_.values[i] ^ b_.values[i];
    }
#endif

    return simde_uint8x8_from_private(r_);
#endif
}
#if defined(SIMDE_ARM_NEON_A32V7_ENABLE_NATIVE_ALIASES)
#undef veor_u8
#define veor_u8(a, b) simde_veor_u8((a), (b))
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde_uint16x4_t simde_veor_u16(simde_uint16x4_t a, simde_uint16x4_t b)
{
#if defined(SIMDE_ARM_NEON_A32V7_NATIVE)
    return veor_u16(a, b);
#elif defined(SIMDE_X86_MMX_NATIVE)
    return _mm_xor_si64(a, b);
#else
    simde_uint16x4_private r_, a_ = simde_uint16x4_to_private(a), b_ = simde_uint16x4_to_private(b);

#if defined(SIMDE_VECTOR_SUBSCRIPT_OPS)
    r_.values = a_.values ^ b_.values;
#else
    SIMDE_VECTORIZE
    for (size_t i = 0; i < (sizeof(r_.values) / sizeof(r_.values[0])); i++) {
        r_.values[i] = a_.values[i] ^ b_.values[i];
    }
#endif

    return simde_uint16x4_from_private(r_);
#endif
}
#if defined(SIMDE_ARM_NEON_A32V7_ENABLE_NATIVE_ALIASES)
#undef veor_u16
#define veor_u16(a, b) simde_veor_u16((a), (b))
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde_uint32x2_t simde_veor_u32(simde_uint32x2_t a, simde_uint32x2_t b)
{
#if defined(SIMDE_ARM_NEON_A32V7_NATIVE)
    return veor_u32(a, b);
#elif defined(SIMDE_X86_MMX_NATIVE)
    return _mm_xor_si64(a, b);
#else
    simde_uint32x2_private r_, a_ = simde_uint32x2_to_private(a), b_ = simde_uint32x2_to_private(b);

#if defined(SIMDE_VECTOR_SUBSCRIPT_OPS)
    r_.values = a_.values ^ b_.values;
#else
    SIMDE_VECTORIZE
    for (size_t i = 0; i < (sizeof(r_.values) / sizeof(r_.values[0])); i++) {
        r_.values[i] = a_.values[i] ^ b_.values[i];
    }
#endif

    return simde_uint32x2_from_private(r_);
#endif
}
#if defined(SIMDE_ARM_NEON_A32V7_ENABLE_NATIVE_ALIASES)
#undef veor_u32
#define veor_u32(a, b) simde_veor_u32((a), (b))
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde_uint64x1_t simde_veor_u64(simde_uint64x1_t a, simde_uint64x1_t b)
{
#if defined(SIMDE_ARM_NEON_A32V7_NATIVE)
    return veor_u64(a, b);
#elif defined(SIMDE_X86_MMX_NATIVE)
    return _mm_xor_si64(a, b);
#else
    simde_uint64x1_private r_, a_ = simde_uint64x1_to_private(a), b_ = simde_uint64x1_to_private(b);

#if defined(SIMDE_VECTOR_SUBSCRIPT_OPS)
    r_.values = a_.values ^ b_.values;
#else
    SIMDE_VECTORIZE
    for (size_t i = 0; i < (sizeof(r_.values) / sizeof(r_.values[0])); i++) {
        r_.values[i] = a_.values[i] ^ b_.values[i];
    }
#endif

    return simde_uint64x1_from_private(r_);
#endif
}
#if defined(SIMDE_ARM_NEON_A32V7_ENABLE_NATIVE_ALIASES)
#undef veor_u64
#define veor_u64(a, b) simde_veor_u64((a), (b))
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde_int8x16_t simde_veorq_s8(simde_int8x16_t a, simde_int8x16_t b)
{
#if defined(SIMDE_ARM_NEON_A32V7_NATIVE)
    return veorq_s8(a, b);
#elif defined(SIMDE_X86_SSE2_NATIVE)
    return _mm_xor_si128(a, b);
#elif defined(SIMDE_POWER_ALTIVEC_P6_NATIVE)
    return vec_xor(a, b);
#elif defined(SIMDE_WASM_SIMD128_NATIVE)
    return wasm_v128_xor(a, b);
#else
    simde_int8x16_private r_, a_ = simde_int8x16_to_private(a), b_ = simde_int8x16_to_private(b);

#if defined(SIMDE_VECTOR_SUBSCRIPT_OPS)
    r_.values = a_.values ^ b_.values;
#else
    SIMDE_VECTORIZE
    for (size_t i = 0; i < (sizeof(r_.values) / sizeof(r_.values[0])); i++) {
        r_.values[i] = a_.values[i] ^ b_.values[i];
    }
#endif

    return simde_int8x16_from_private(r_);
#endif
}
#if defined(SIMDE_ARM_NEON_A32V7_ENABLE_NATIVE_ALIASES)
#undef veorq_s8
#define veorq_s8(a, b) simde_veorq_s8((a), (b))
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde_int16x8_t simde_veorq_s16(simde_int16x8_t a, simde_int16x8_t b)
{
#if defined(SIMDE_ARM_NEON_A32V7_NATIVE)
    return veorq_s16(a, b);
#elif defined(SIMDE_X86_SSE2_NATIVE)
    return _mm_xor_si128(a, b);
#elif defined(SIMDE_POWER_ALTIVEC_P6_NATIVE)
    return vec_xor(a, b);
#elif defined(SIMDE_WASM_SIMD128_NATIVE)
    return wasm_v128_xor(a, b);
#else
    simde_int16x8_private r_, a_ = simde_int16x8_to_private(a), b_ = simde_int16x8_to_private(b);

#if defined(SIMDE_VECTOR_SUBSCRIPT_OPS)
    r_.values = a_.values ^ b_.values;
#else
    SIMDE_VECTORIZE
    for (size_t i = 0; i < (sizeof(r_.values) / sizeof(r_.values[0])); i++) {
        r_.values[i] = a_.values[i] ^ b_.values[i];
    }
#endif

    return simde_int16x8_from_private(r_);
#endif
}
#if defined(SIMDE_ARM_NEON_A32V7_ENABLE_NATIVE_ALIASES)
#undef veorq_s16
#define veorq_s16(a, b) simde_veorq_s16((a), (b))
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde_int32x4_t simde_veorq_s32(simde_int32x4_t a, simde_int32x4_t b)
{
#if defined(SIMDE_ARM_NEON_A32V7_NATIVE)
    return veorq_s32(a, b);
#elif defined(SIMDE_X86_SSE2_NATIVE)
    return _mm_xor_si128(a, b);
#elif defined(SIMDE_POWER_ALTIVEC_P6_NATIVE)
    return vec_xor(a, b);
#elif defined(SIMDE_WASM_SIMD128_NATIVE)
    return wasm_v128_xor(a, b);
#else
    simde_int32x4_private r_, a_ = simde_int32x4_to_private(a), b_ = simde_int32x4_to_private(b);

#if defined(SIMDE_VECTOR_SUBSCRIPT_OPS)
    r_.values = a_.values ^ b_.values;
#else
    SIMDE_VECTORIZE
    for (size_t i = 0; i < (sizeof(r_.values) / sizeof(r_.values[0])); i++) {
        r_.values[i] = a_.values[i] ^ b_.values[i];
    }
#endif

    return simde_int32x4_from_private(r_);
#endif
}
#if defined(SIMDE_ARM_NEON_A32V7_ENABLE_NATIVE_ALIASES)
#undef veorq_s32
#define veorq_s32(a, b) simde_veorq_s32((a), (b))
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde_int64x2_t simde_veorq_s64(simde_int64x2_t a, simde_int64x2_t b)
{
#if defined(SIMDE_ARM_NEON_A32V7_NATIVE)
    return veorq_s64(a, b);
#elif defined(SIMDE_X86_SSE2_NATIVE)
    return _mm_xor_si128(a, b);
#elif defined(SIMDE_POWER_ALTIVEC_P7_NATIVE)
    return vec_xor(a, b);
#elif defined(SIMDE_WASM_SIMD128_NATIVE)
    return wasm_v128_xor(a, b);
#else
    simde_int64x2_private r_, a_ = simde_int64x2_to_private(a), b_ = simde_int64x2_to_private(b);

#if defined(SIMDE_VECTOR_SUBSCRIPT_OPS)
    r_.values = a_.values ^ b_.values;
#else
    SIMDE_VECTORIZE
    for (size_t i = 0; i < (sizeof(r_.values) / sizeof(r_.values[0])); i++) {
        r_.values[i] = a_.values[i] ^ b_.values[i];
    }
#endif

    return simde_int64x2_from_private(r_);
#endif
}
#if defined(SIMDE_ARM_NEON_A32V7_ENABLE_NATIVE_ALIASES)
#undef veorq_s64
#define veorq_s64(a, b) simde_veorq_s64((a), (b))
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde_uint8x16_t simde_veorq_u8(simde_uint8x16_t a, simde_uint8x16_t b)
{
#if defined(SIMDE_ARM_NEON_A32V7_NATIVE)
    return veorq_u8(a, b);
#elif defined(SIMDE_X86_SSE2_NATIVE)
    return _mm_xor_si128(a, b);
#elif defined(SIMDE_POWER_ALTIVEC_P6_NATIVE)
    return vec_xor(a, b);
#elif defined(SIMDE_WASM_SIMD128_NATIVE)
    return wasm_v128_xor(a, b);
#else
    simde_uint8x16_private r_, a_ = simde_uint8x16_to_private(a), b_ = simde_uint8x16_to_private(b);

#if defined(SIMDE_VECTOR_SUBSCRIPT_OPS)
    r_.values = a_.values ^ b_.values;
#else
    SIMDE_VECTORIZE
    for (size_t i = 0; i < (sizeof(r_.values) / sizeof(r_.values[0])); i++) {
        r_.values[i] = a_.values[i] ^ b_.values[i];
    }
#endif

    return simde_uint8x16_from_private(r_);
#endif
}
#if defined(SIMDE_ARM_NEON_A32V7_ENABLE_NATIVE_ALIASES)
#undef veorq_u8
#define veorq_u8(a, b) simde_veorq_u8((a), (b))
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde_uint16x8_t simde_veorq_u16(simde_uint16x8_t a, simde_uint16x8_t b)
{
#if defined(SIMDE_ARM_NEON_A32V7_NATIVE)
    return veorq_u16(a, b);
#elif defined(SIMDE_X86_SSE2_NATIVE)
    return _mm_xor_si128(a, b);
#elif defined(SIMDE_POWER_ALTIVEC_P6_NATIVE)
    return vec_xor(a, b);
#elif defined(SIMDE_WASM_SIMD128_NATIVE)
    return wasm_v128_xor(a, b);
#else
    simde_uint16x8_private r_, a_ = simde_uint16x8_to_private(a), b_ = simde_uint16x8_to_private(b);

#if defined(SIMDE_VECTOR_SUBSCRIPT_OPS)
    r_.values = a_.values ^ b_.values;
#else
    SIMDE_VECTORIZE
    for (size_t i = 0; i < (sizeof(r_.values) / sizeof(r_.values[0])); i++) {
        r_.values[i] = a_.values[i] ^ b_.values[i];
    }
#endif

    return simde_uint16x8_from_private(r_);
#endif
}
#if defined(SIMDE_ARM_NEON_A32V7_ENABLE_NATIVE_ALIASES)
#undef veorq_u16
#define veorq_u16(a, b) simde_veorq_u16((a), (b))
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde_uint32x4_t simde_veorq_u32(simde_uint32x4_t a, simde_uint32x4_t b)
{
#if defined(SIMDE_ARM_NEON_A32V7_NATIVE)
    return veorq_u32(a, b);
#elif defined(SIMDE_X86_SSE2_NATIVE)
    return _mm_xor_si128(a, b);
#elif defined(SIMDE_POWER_ALTIVEC_P6_NATIVE)
    return vec_xor(a, b);
#elif defined(SIMDE_WASM_SIMD128_NATIVE)
    return wasm_v128_xor(a, b);
#else
    simde_uint32x4_private r_, a_ = simde_uint32x4_to_private(a), b_ = simde_uint32x4_to_private(b);

#if defined(SIMDE_VECTOR_SUBSCRIPT_OPS)
    r_.values = a_.values ^ b_.values;
#else
    SIMDE_VECTORIZE
    for (size_t i = 0; i < (sizeof(r_.values) / sizeof(r_.values[0])); i++) {
        r_.values[i] = a_.values[i] ^ b_.values[i];
    }
#endif

    return simde_uint32x4_from_private(r_);
#endif
}
#if defined(SIMDE_ARM_NEON_A32V7_ENABLE_NATIVE_ALIASES)
#undef veorq_u32
#define veorq_u32(a, b) simde_veorq_u32((a), (b))
#endif

SIMDE_FUNCTION_ATTRIBUTES
simde_uint64x2_t simde_veorq_u64(simde_uint64x2_t a, simde_uint64x2_t b)
{
#if defined(SIMDE_ARM_NEON_A32V7_NATIVE)
    return veorq_u64(a, b);
#elif defined(SIMDE_X86_SSE2_NATIVE)
    return _mm_xor_si128(a, b);
#elif defined(SIMDE_POWER_ALTIVEC_P7_NATIVE)
    return vec_xor(a, b);
#elif defined(SIMDE_WASM_SIMD128_NATIVE)
    return wasm_v128_xor(a, b);
#else
    simde_uint64x2_private r_, a_ = simde_uint64x2_to_private(a), b_ = simde_uint64x2_to_private(b);

#if defined(SIMDE_VECTOR_SUBSCRIPT_OPS)
    r_.values = a_.values ^ b_.values;
#else
    SIMDE_VECTORIZE
    for (size_t i = 0; i < (sizeof(r_.values) / sizeof(r_.values[0])); i++) {
        r_.values[i] = a_.values[i] ^ b_.values[i];
    }
#endif

    return simde_uint64x2_from_private(r_);
#endif
}
#if defined(SIMDE_ARM_NEON_A32V7_ENABLE_NATIVE_ALIASES)
#undef veorq_u64
#define veorq_u64(a, b) simde_veorq_u64((a), (b))
#endif

SIMDE_END_DECLS_
HEDLEY_DIAGNOSTIC_POP

#endif /* !defined(SIMDE_ARM_NEON_EOR_H) */
