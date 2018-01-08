// Copyright (c) 2016-2018, Pacific Biosciences of California, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//  * Neither the name of Pacific Biosciences nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY PACIFIC
// BIOSCIENCES AND ITS CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL PACIFIC BIOSCIENCES OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
// USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

// Author: Derek Barnett

#ifndef PBCOPPER_UTILITY_SYSTEMINFO_INL_H
#define PBCOPPER_UTILITY_SYSTEMINFO_INL_H

#include <cstdint>

#include <boost/detail/endian.hpp>

#include <pbcopper/utility/SystemInfo.h>

namespace PacBio {
namespace Utility {

//
// if something funky with Boost detection, do this 'manual' compile-time check
// as fallback
//
#if !defined(BOOST_LITTLE_ENDIAN) && !defined(BOOST_BIG_ENDIAN)

namespace internal {

class EndianHelper
{
private:
    static constexpr uint32_t num_  = 0x01020304;
    static constexpr uint8_t check_ = static_cast<const uint8_t&>(num_);
public:
    static constexpr bool little = (check_ == 0x04);
    static constexpr bool big    = (check_ == 0x01);
    static_assert(little || big, "cannot determine endianness");
};

} // namespace internal

#endif // no Boost endian

inline constexpr SystemInfo::Endian SystemInfo::ByteOrder(void)
{
#if defined(BOOST_LITTLE_ENDIAN)
    return SystemInfo::LittleEndian;
#elif defined(BOOST_BIG_ENDIAN)
    return SystemInfo::BigEndian;
#else
    return internal::EndianHelper::little ? SystemInfo::LittleEndian
                                          : SystemInfo::BigEndian;
#endif
}

inline constexpr bool SystemInfo::IsBigEndian(void)
{ return SystemInfo::ByteOrder() == SystemInfo::BigEndian; }

inline constexpr bool SystemInfo::IsLittleEndian(void)
{ return SystemInfo::ByteOrder() == SystemInfo::LittleEndian; }

} // namespace Utility
} // namespace PacBio

#endif // PBCOPPER_UTILITY_SYSTEMINFO_H
