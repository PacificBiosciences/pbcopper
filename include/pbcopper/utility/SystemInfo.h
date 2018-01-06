#ifndef PBCOPPER_UTILITY_SYSTEMINFO_H
#define PBCOPPER_UTILITY_SYSTEMINFO_H

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Utility {

class SystemInfo
{
public:
    enum Endian
    {
        BigEndian,
        LittleEndian
    };

public:
    static constexpr SystemInfo::Endian ByteOrder(void);
    static constexpr bool IsBigEndian(void);
    static constexpr bool IsLittleEndian(void);
};

} // namespace Utility
} // namespace PacBio

#include "pbcopper/utility/internal/SystemInfo-inl.h"

#endif // PBCOPPER_UTILITY_SYSTEMINFO_H
