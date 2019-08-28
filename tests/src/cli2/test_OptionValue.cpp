
#include <stdexcept>

#include <gtest/gtest.h>

#include <pbcopper/cli2/OptionValue.h>

using OptionValue = PacBio::CLI_v2::OptionValue;
using OptionValueType = PacBio::CLI_v2::OptionValueType;

// clang-format off

TEST(CLI2_OptionValue, value_type_from_string)
{

    EXPECT_EQ(OptionValueType::INT,    PacBio::CLI_v2::ValueType("int"));
    EXPECT_EQ(OptionValueType::INT,    PacBio::CLI_v2::ValueType("integer"));
    EXPECT_EQ(OptionValueType::UINT,   PacBio::CLI_v2::ValueType("uint"));
    EXPECT_EQ(OptionValueType::UINT,   PacBio::CLI_v2::ValueType("unsigned int"));
    EXPECT_EQ(OptionValueType::UINT,   PacBio::CLI_v2::ValueType("unsigned integer"));
    EXPECT_EQ(OptionValueType::FLOAT,  PacBio::CLI_v2::ValueType("float"));
    EXPECT_EQ(OptionValueType::BOOL,   PacBio::CLI_v2::ValueType("bool"));
    EXPECT_EQ(OptionValueType::BOOL,   PacBio::CLI_v2::ValueType("boolean"));
    EXPECT_EQ(OptionValueType::STRING, PacBio::CLI_v2::ValueType("string"));
    EXPECT_EQ(OptionValueType::FILE,   PacBio::CLI_v2::ValueType("file"));
    EXPECT_EQ(OptionValueType::DIR,    PacBio::CLI_v2::ValueType("dir"));
}

TEST(CLI2_OptionValue, throws_on_invalid_type_string)
{
    EXPECT_THROW(PacBio::CLI_v2::ValueType("INVALID"), std::runtime_error);
}

TEST(CLI2_OptionValue, equality_comparable)
{
    int x = 4;
    int y = 7;
    EXPECT_EQ(OptionValue{x}, OptionValue{x});
    EXPECT_NE(OptionValue{x}, OptionValue{y});

    std::string s{"foo"};
    EXPECT_EQ(OptionValue{s}, OptionValue{s});
    EXPECT_NE(OptionValue{s}, OptionValue{x});
    EXPECT_NE(OptionValue{s}, OptionValue{y});

    // types must match (here signed vs unsigned)
    unsigned int u = 4;
    EXPECT_NE(OptionValue{x}, OptionValue{u});
}

// clang-format on
