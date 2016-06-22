#ifndef PBCOPPER_UTILITY_SYSTEMINFO_INL_H
#define PBCOPPER_UTILITY_SYSTEMINFO_INL_H

#include "pbcopper/utility/SystemInfo.h"
#include <boost/detail/endian.hpp>

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
