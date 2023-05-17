#ifndef PBCOPPER_CONTAINER_BITCONVERSION_H
#define PBCOPPER_CONTAINER_BITCONVERSION_H

#include <pbcopper/PbcopperConfig.h>

#include "BitmaskContainer.h"

#include <cassert>
#include <cstdint>

namespace PacBio {
namespace Container {

// Convert ASCII to the 2-bit encoding known as NCBI2na
//
//   The NCBI2na format is a 2-bit format that does not
//   allow for encoding ambiguous bases:
//
//     Bit2 Bit1 | Dec | State
//     -----------------------
//      0    0   |  0  |   A
//      0    1   |  1  |   C
//      1    0   |  2  |   G
//      1    1   |  3  |   T
//
//   One advantage of NCBI2na is that reverse complementing
//   can be achieved by bit-wise negation.
//
// Reference:
//   https://www.ncbi.nlm.nih.gov/IEB/ToolBox/SDKDOCS/BIOSEQ.HTML
//
// * checked == false:
//   The input is not checked for validity. Passing a non-DNA
//   character into the function does **NOT** cause UB, but yields
//   an unpredictable value from the set {0, 1, 2, 3}.
//
//   Clang 13 (amd64; -march=znver2 -O3 -DNDEBUG): 14 bytes L1i
//     0:   b8 08 01 30 00          mov    eax,0x300108
//     5:   c4 e2 43 f7 c0          shrx   eax,eax,edi
//     a:   83 e0 03                and    eax,0x3
//     d:   c3                      ret
//
// * checked == true:
//   The input is checked for validity. Passing a non-DNA character
//   yields a return value >=4.
//
//   Clang 13 (amd64; -march=znver2 -O3 -DNDEBUG): 52 bytes L1i
//     0:   b8 75 ff ef ff          mov    eax,0xffefff75
//     5:   ba 44 00 44 44          mov    edx,0x44440044
//     a:   c4 e2 43 f7 c8          shrx   ecx,eax,edi
//     f:   b8 08 01 30 00          mov    eax,0x300108
//    14:   c4 e2 43 f7 c0          shrx   eax,eax,edi
//    19:   40 c0 ef 03             shr    dil,0x3
//    1d:   c1 e1 02                shl    ecx,0x2
//    20:   40 80 e7 1c             and    dil,0x1c
//    24:   83 e0 03                and    eax,0x3
//    27:   c4 e2 43 f7 d2          shrx   edx,edx,edi
//    2c:   09 ca                   or     edx,ecx
//    2e:   83 e2 04                and    edx,0x4
//    31:   09 d0                   or     eax,edx
//    33:   c3                      ret
template <bool complement = false, bool checked = false>
PB_CUDA_HOST PB_CUDA_DEVICE constexpr std::int32_t ConvertAsciiTo2bit(
    const unsigned char base) noexcept
{
    const std::int32_t truncatedBase = base & 0b1'1111;

    std::uint32_t invalidBits = 0;
    if constexpr (checked) {
        // check the least-significant 5 bits
        constexpr auto LSB_INVALID_LOOKUP_TABLE = Container::BitmaskContainer<32, 1>::MakeFromArray(
            1, 0 /*A*/, 1, 0 /*C*/, 1, 1, 1, 0 /*G*/, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 /*T*/,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
        const std::uint32_t lsbCheckResult = 4 * LSB_INVALID_LOOKUP_TABLE[truncatedBase];

        // check the most-significant 3 bits
        constexpr auto MSB_INVALID_LOOKUP_TABLE =
            Container::BitmaskContainer<32, 4>::MakeFromArray(4, 4, 0, 0, 4, 4, 4, 4);
        const std::uint32_t msbCheckResult = MSB_INVALID_LOOKUP_TABLE[base >> 5];

        invalidBits = lsbCheckResult | msbCheckResult;
    } else {
        assert((base == 'A') || (base == 'C') || (base == 'G') || (base == 'T') || (base == 'a') ||
               (base == 'c') || (base == 'g') || (base == 't'));
    }

    constexpr std::uint32_t LOOKUP_TABLE{complement
                                             //  _^]\[ZYXWVUTSRQPONMLKJIHGFEDCBA@ [64, 96)
                                             //   ~}|{zyxwvutsrqponmlkjihgfedcba` [96, 128)
                                             //            ┌┤           ┌┤  ┌┤┌┤
                                             ? 0b00000000000000000000000010010110
                                             : 0b00000000001100000000000100001000};

    return invalidBits | ((LOOKUP_TABLE >> truncatedBase) & 0b11);
}

// Convert 2-bit encoding (NCBI2na) to ASCII
//
// Clang 13 (amd64; -march=znver2 -O3 -DNDEBUG): 14 bytes L1i
//   0:   c1 e7 03                shl    edi,0x3
//   3:   b8 41 43 47 54          mov    eax,0x54474341
//   8:   c4 e2 43 f7 c0          shrx   eax,eax,edi
//   d:   c3                      ret
PB_CUDA_HOST PB_CUDA_DEVICE constexpr char Convert2bitToAscii(const std::int32_t val) noexcept
{
    constexpr auto LOOKUP_TABLE =
        Container::BitmaskContainer<32, 8>::MakeFromArray('A', 'C', 'G', 'T');
    return LOOKUP_TABLE[val];
}

}  // namespace Container
}  // namespace PacBio

#endif  // PBCOPPER_CONTAINER_BITCONVERSION_H
