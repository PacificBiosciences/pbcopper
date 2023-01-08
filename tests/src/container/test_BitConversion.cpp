#include <pbcopper/container/BitConversion.h>

#include <gtest/gtest.h>

using namespace PacBio::Container;

TEST(Container_BitConversion, ConvertAsciiTo2bit)
{
    const auto f = ConvertAsciiTo2bit<false>;

    EXPECT_EQ(f('A'), 0);
    EXPECT_EQ(f('a'), 0);
    EXPECT_EQ(f('C'), 1);
    EXPECT_EQ(f('c'), 1);
    EXPECT_EQ(f('G'), 2);
    EXPECT_EQ(f('g'), 2);
    EXPECT_EQ(f('T'), 3);
    EXPECT_EQ(f('t'), 3);
}

TEST(Container_BitConversion, ConvertAsciiTo2bitComplement)
{
    const auto f = ConvertAsciiTo2bit<true>;

    EXPECT_EQ(f('A'), 3);
    EXPECT_EQ(f('a'), 3);
    EXPECT_EQ(f('C'), 2);
    EXPECT_EQ(f('c'), 2);
    EXPECT_EQ(f('G'), 1);
    EXPECT_EQ(f('g'), 1);
    EXPECT_EQ(f('T'), 0);
    EXPECT_EQ(f('t'), 0);
}

TEST(Container_BitConversion, ConvertAsciiTo2bitChecked)
{
    const auto f = ConvertAsciiTo2bit<false, true>;

    for (int32_t i = 0; i < 256; ++i) {
        switch (i) {
            case 'A':
            case 'a':
                EXPECT_EQ(f(i), 0);
                break;
            case 'C':
            case 'c':
                EXPECT_EQ(f(i), 1);
                break;
            case 'G':
            case 'g':
                EXPECT_EQ(f(i), 2);
                break;
            case 'T':
            case 't':
                EXPECT_EQ(f(i), 3);
                break;
            default:
                EXPECT_GE(f(i), 4);
                break;
        }
    }
}

TEST(Container_BitConversion, ConvertAsciiTo2bitComplementChecked)
{
    const auto f = ConvertAsciiTo2bit<true, true>;

    for (int32_t i = 0; i < 256; ++i) {
        switch (i) {
            case 'A':
            case 'a':
                EXPECT_EQ(f(i), 3);
                break;
            case 'C':
            case 'c':
                EXPECT_EQ(f(i), 2);
                break;
            case 'G':
            case 'g':
                EXPECT_EQ(f(i), 1);
                break;
            case 'T':
            case 't':
                EXPECT_EQ(f(i), 0);
                break;
            default:
                EXPECT_GE(f(i), 4);
                break;
        }
    }
}

TEST(Container_BitConversion, Convert2bitToAscii)
{
    const auto f = Convert2bitToAscii;

    EXPECT_EQ(f(0), 'A');
    EXPECT_EQ(f(1), 'C');
    EXPECT_EQ(f(2), 'G');
    EXPECT_EQ(f(3), 'T');
}

TEST(Container_BitConversion, RoundRobin2bit)
{
    const auto innerF = Convert2bitToAscii;
    const auto outerF = ConvertAsciiTo2bit<false>;

    EXPECT_EQ(outerF(innerF(0)), 0);
    EXPECT_EQ(outerF(innerF(1)), 1);
    EXPECT_EQ(outerF(innerF(2)), 2);
    EXPECT_EQ(outerF(innerF(3)), 3);
}

TEST(Container_BitConversion, RoundRobin2bitComplement)
{
    const auto innerF = Convert2bitToAscii;
    const auto outerF = ConvertAsciiTo2bit<true>;

    EXPECT_EQ(outerF(innerF(0)), 3);
    EXPECT_EQ(outerF(innerF(1)), 2);
    EXPECT_EQ(outerF(innerF(2)), 1);
    EXPECT_EQ(outerF(innerF(3)), 0);
}

TEST(Container_BitConversion, RoundRobinAscii)
{
    const auto innerF = ConvertAsciiTo2bit<false>;
    const auto outerF = Convert2bitToAscii;

    EXPECT_EQ(outerF(innerF('A')), 'A');
    EXPECT_EQ(outerF(innerF('C')), 'C');
    EXPECT_EQ(outerF(innerF('G')), 'G');
    EXPECT_EQ(outerF(innerF('T')), 'T');
}

TEST(Container_BitConversion, RoundRobinAsciiComplement)
{
    const auto innerF = ConvertAsciiTo2bit<true>;
    const auto outerF = Convert2bitToAscii;

    EXPECT_EQ(outerF(innerF('A')), 'T');
    EXPECT_EQ(outerF(innerF('C')), 'G');
    EXPECT_EQ(outerF(innerF('G')), 'C');
    EXPECT_EQ(outerF(innerF('T')), 'A');
}
