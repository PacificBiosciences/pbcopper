
#include <pbcopper/utility/SystemInfo.h>
#include <gtest/gtest.h>
using namespace PacBio;
using namespace PacBio::Utility;

namespace PacBio {
namespace Utility {
namespace tests {

// one sort of 'manual' endianness check - not used by the library
static inline bool isLittleEndian(void)
{
    static const int i = 1;
    static const char* const c = reinterpret_cast<const char* const>(&i);
    return (*c == 1);
}

} // namespace tests
} // namespace Utility
} // namespace PacBio

TEST(Utility_SystemInfo, proper_byte_order_detected)
{
    if (tests::isLittleEndian()) {
        EXPECT_EQ(SystemInfo::LittleEndian, SystemInfo::ByteOrder());
        EXPECT_TRUE(SystemInfo::IsLittleEndian());
        EXPECT_FALSE(SystemInfo::IsBigEndian());
    } else {
        EXPECT_EQ(SystemInfo::BigEndian, SystemInfo::ByteOrder());
        EXPECT_FALSE(SystemInfo::IsLittleEndian());
        EXPECT_TRUE(SystemInfo::IsBigEndian());
    }
}
