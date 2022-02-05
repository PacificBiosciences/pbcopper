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
// Clang 13 (amd64; -march=znver2 -O3 -DNDEBUG): 14 bytes L1i
//   0:   b8 08 01 30 00          mov    eax,0x300108
//   5:   c4 e2 43 f7 c0          shrx   eax,eax,edi
//   a:   83 e0 03                and    eax,0x3
//   d:   c3                      ret
constexpr int32_t ConvertAsciiTo2bit(const char base) noexcept
{
    assert((base == 'A') || (base == 'C') || (base == 'G') || (base == 'T'));

    const int32_t truncatedBase = base & 0b1'1111;

    // ASCII range [64, 85)
    constexpr uint32_t lut{//  ┝T┥                       ┝G┥     ┝C┥ ┝A┥
                           0b0'1'1'0'0'0'0'0'0'0'0'0'0'0'1'0'0'0'0'1'0'0'0};

    return (lut >> truncatedBase) & 0b11;
}

// Convert 2-bit encoding (NCBI2na) to ASCII
//
// Clang 13 (amd64; -march=znver2 -O3 -DNDEBUG): 14 bytes L1i
//   0:   c1 e7 03                shl    edi,0x3
//   3:   b8 41 43 47 54          mov    eax,0x54474341
//   8:   c4 e2 43 f7 c0          shrx   eax,eax,edi
//   d:   c3                      ret
constexpr char Convert2bitToAscii(const int32_t val) noexcept
{
    assert(val >= 0);
    assert(val < 4);

    constexpr auto lut = Container::BitmaskContainer<32, 8>::MakeFromArray('A', 'C', 'G', 'T');
    return lut[val];
}

}  // namespace Container
}  // namespace PacBio

#endif  // PBCOPPER_CONTAINER_BITCONVERSION_H
