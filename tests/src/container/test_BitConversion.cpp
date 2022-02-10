#include <pbcopper/container/BitConversion.h>

#include <gtest/gtest.h>

using namespace PacBio::Container;

TEST(Container_BitConversion, ConvertAsciiTo2bit)
{
    EXPECT_EQ(ConvertAsciiTo2bit('A'), 0);
    EXPECT_EQ(ConvertAsciiTo2bit('C'), 1);
    EXPECT_EQ(ConvertAsciiTo2bit('G'), 2);
    EXPECT_EQ(ConvertAsciiTo2bit('T'), 3);
}

TEST(Container_BitConversion, Convert2bitToAscii)
{
    EXPECT_EQ(Convert2bitToAscii(0), 'A');
    EXPECT_EQ(Convert2bitToAscii(1), 'C');
    EXPECT_EQ(Convert2bitToAscii(2), 'G');
    EXPECT_EQ(Convert2bitToAscii(3), 'T');
}

TEST(Container_BitConversion, RoundRobin2bit)
{
    EXPECT_EQ(ConvertAsciiTo2bit(Convert2bitToAscii(0)), 0);
    EXPECT_EQ(ConvertAsciiTo2bit(Convert2bitToAscii(1)), 1);
    EXPECT_EQ(ConvertAsciiTo2bit(Convert2bitToAscii(2)), 2);
    EXPECT_EQ(ConvertAsciiTo2bit(Convert2bitToAscii(3)), 3);
}

TEST(Container_BitConversion, RoundRobinAscii)
{
    EXPECT_EQ(Convert2bitToAscii(ConvertAsciiTo2bit('A')), 'A');
    EXPECT_EQ(Convert2bitToAscii(ConvertAsciiTo2bit('C')), 'C');
    EXPECT_EQ(Convert2bitToAscii(ConvertAsciiTo2bit('G')), 'G');
    EXPECT_EQ(Convert2bitToAscii(ConvertAsciiTo2bit('T')), 'T');
}
