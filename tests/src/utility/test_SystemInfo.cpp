#include <pbcopper/utility/SystemInfo.h>

#include <gtest/gtest.h>

namespace SystemInfoTests {

// one sort of 'manual' endianness check - not used by the library
static inline bool isLittleEndian()
{
    static const int i = 1;
    static const char* c = reinterpret_cast<const char*>(&i);
    return (*c == 1);
}

}  // namespace SystemInfoTests

TEST(Utility_SystemInfo, proper_byte_order_detected)
{
    if (SystemInfoTests::isLittleEndian()) {
        EXPECT_EQ(PacBio::Utility::SystemInfo::LittleEndian,
                  PacBio::Utility::SystemInfo::ByteOrder());
        EXPECT_TRUE(PacBio::Utility::SystemInfo::IsLittleEndian());
        EXPECT_FALSE(PacBio::Utility::SystemInfo::IsBigEndian());
    } else {
        EXPECT_EQ(PacBio::Utility::SystemInfo::BigEndian, PacBio::Utility::SystemInfo::ByteOrder());
        EXPECT_FALSE(PacBio::Utility::SystemInfo::IsLittleEndian());
        EXPECT_TRUE(PacBio::Utility::SystemInfo::IsBigEndian());
    }
}
