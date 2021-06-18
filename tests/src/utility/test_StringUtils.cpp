#include <pbcopper/utility/StringUtils.h>

#include <algorithm>

#include <gtest/gtest.h>

namespace StringUtilsTests {

void checkWrappedLines(const std::string& input, const std::vector<std::string>& expected,
                       const size_t maxColumns)
{
    const auto lines = PacBio::Utility::WordWrappedLines(input, maxColumns);
    EXPECT_TRUE(std::equal(lines.cbegin(), lines.cend(), expected.cbegin()));
}

}  // namespace StringUtilsTests

TEST(Utility_StringUtils, word_wrapping_empty_string_yields_empty_result)
{
    const std::string input;
    const std::vector<std::string> expected;
    StringUtilsTests::checkWrappedLines(input, expected, 80);
}

TEST(Utility_StringUtils, word_wrapping_shorter_than_max_yields_one_line)
{
    const std::string input{"Short description."};
    const std::vector<std::string> expected{"Short description."};
    StringUtilsTests::checkWrappedLines(input, expected, 80);
}

TEST(Utility_StringUtils, word_wrapping_longer_than_max_yields_wrapped_line)
{
    //
    // NOTE(DB): You MUST keep clang-format off here. It messes up the line breaks
    //           we're testing.
    //
    // clang-format off

    const std::string input{
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua."};

    {
        SCOPED_TRACE("WordWrap, maxColumns = 30");
        const std::vector<std::string> expected {
            "Lorem ipsum dolor sit amet,",
            "consectetur adipiscing elit,",
            "sed do eiusmod tempor",
            "incididunt ut labore et dolore",
            "magna aliqua."
        };
        StringUtilsTests::checkWrappedLines(input, expected, 30);
    }
    {
        SCOPED_TRACE("WordWrap, maxColumns = 50");
        const std::vector<std::string> expected {
            "Lorem ipsum dolor sit amet, consectetur adipiscing",
            "elit, sed do eiusmod tempor incididunt ut labore",
            "et dolore magna aliqua."
        };
        StringUtilsTests::checkWrappedLines(input, expected, 50);
    }
    {
        SCOPED_TRACE("WordWrap, maxColumns = 80");
        const std::vector<std::string> expected {
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor",
            "incididunt ut labore et dolore magna aliqua."
        };
        StringUtilsTests::checkWrappedLines(input, expected, 80);
    }

    // clang-format on
}

TEST(Utility_StringUtils, si_string_to_int_throws_on_empty_input)
{
    EXPECT_THROW(PacBio::Utility::SIStringToInt(""), std::exception);
}

TEST(Utility_StringUtils, si_string_to_int_throws_on_invalid_input)
{
    EXPECT_THROW(PacBio::Utility::SIStringToInt("+"), std::exception);
    EXPECT_THROW(PacBio::Utility::SIStringToInt("-"), std::exception);
    EXPECT_THROW(PacBio::Utility::SIStringToInt("G"), std::exception);
    EXPECT_THROW(PacBio::Utility::SIStringToInt("X"), std::exception);
    EXPECT_THROW(PacBio::Utility::SIStringToInt("100x"), std::exception);
    EXPECT_THROW(PacBio::Utility::SIStringToInt("-100x"), std::exception);
}

TEST(Utility_StringUtils, converts_si_string_to_positive_int)
{
    EXPECT_EQ(0, PacBio::Utility::SIStringToInt("0"));
    EXPECT_EQ(42, PacBio::Utility::SIStringToInt("42"));

    EXPECT_EQ(100000, PacBio::Utility::SIStringToInt("100K"));
    EXPECT_EQ(100000, PacBio::Utility::SIStringToInt("100k"));

    EXPECT_EQ(100000, PacBio::Utility::SIStringToInt("+100K"));
    EXPECT_EQ(100000, PacBio::Utility::SIStringToInt("+100k"));

    EXPECT_EQ(2000000, PacBio::Utility::SIStringToInt("2M"));
    EXPECT_EQ(2000000, PacBio::Utility::SIStringToInt("2m"));

    EXPECT_EQ(60000000000, PacBio::Utility::SIStringToInt("60G"));
    EXPECT_EQ(60000000000, PacBio::Utility::SIStringToInt("60g"));
}

TEST(Utility_StringUtils, converts_si_string_to_negative_int)
{
    EXPECT_EQ(0, PacBio::Utility::SIStringToInt("-0"));
    EXPECT_EQ(-42, PacBio::Utility::SIStringToInt("-42"));

    EXPECT_EQ(-100000, PacBio::Utility::SIStringToInt("-100K"));
    EXPECT_EQ(-100000, PacBio::Utility::SIStringToInt("-100k"));

    EXPECT_EQ(-2000000, PacBio::Utility::SIStringToInt("-2M"));
    EXPECT_EQ(-2000000, PacBio::Utility::SIStringToInt("-2m"));

    EXPECT_EQ(-60000000000, PacBio::Utility::SIStringToInt("-60G"));
    EXPECT_EQ(-60000000000, PacBio::Utility::SIStringToInt("-60g"));
}

TEST(Utility_StringUtils, scientific_notation_throws_on_empty_input)
{
    EXPECT_THROW(PacBio::Utility::ScientificNotationToInt(""), std::exception);
    EXPECT_THROW(PacBio::Utility::ScientificNotationToUInt(""), std::exception);
    EXPECT_THROW(PacBio::Utility::ScientificNotationToDouble(""), std::exception);
}

TEST(Utility_StringUtils, scientific_notation_throws_on_invalid_input)
{
    EXPECT_THROW(PacBio::Utility::ScientificNotationToInt("+"), std::exception);
    EXPECT_THROW(PacBio::Utility::ScientificNotationToInt("-"), std::exception);
    EXPECT_THROW(PacBio::Utility::ScientificNotationToInt("G"), std::exception);
    EXPECT_THROW(PacBio::Utility::ScientificNotationToInt("x"), std::exception);
    EXPECT_THROW(PacBio::Utility::ScientificNotationToInt("e"), std::exception);
    EXPECT_THROW(PacBio::Utility::ScientificNotationToInt(" "), std::exception);

    EXPECT_THROW(PacBio::Utility::ScientificNotationToUInt("+"), std::exception);
    EXPECT_THROW(PacBio::Utility::ScientificNotationToUInt("-"), std::exception);
    EXPECT_THROW(PacBio::Utility::ScientificNotationToUInt("G"), std::exception);
    EXPECT_THROW(PacBio::Utility::ScientificNotationToUInt("x"), std::exception);
    EXPECT_THROW(PacBio::Utility::ScientificNotationToUInt("e"), std::exception);
    EXPECT_THROW(PacBio::Utility::ScientificNotationToUInt(" "), std::exception);

    EXPECT_THROW(PacBio::Utility::ScientificNotationToDouble("+"), std::exception);
    EXPECT_THROW(PacBio::Utility::ScientificNotationToDouble("-"), std::exception);
    EXPECT_THROW(PacBio::Utility::ScientificNotationToDouble("G"), std::exception);
    EXPECT_THROW(PacBio::Utility::ScientificNotationToDouble("x"), std::exception);
    EXPECT_THROW(PacBio::Utility::ScientificNotationToDouble("e"), std::exception);
    EXPECT_THROW(PacBio::Utility::ScientificNotationToDouble(" "), std::exception);
}

TEST(Utility_StringUtils, can_convert_scientific_notation_to_number)
{
    EXPECT_EQ(12000, PacBio::Utility::ScientificNotationToInt("12000"));
    EXPECT_EQ(12000, PacBio::Utility::ScientificNotationToInt("1.2e4"));
    EXPECT_EQ(12000, PacBio::Utility::ScientificNotationToInt("1.2E4"));
    EXPECT_EQ(-12000, PacBio::Utility::ScientificNotationToInt("-12000"));
    EXPECT_EQ(-12000, PacBio::Utility::ScientificNotationToInt("-1.2e4"));
    EXPECT_EQ(-12000, PacBio::Utility::ScientificNotationToInt("-1.2E4"));

    EXPECT_EQ(12000, PacBio::Utility::ScientificNotationToInt("12000"));
    EXPECT_EQ(12000, PacBio::Utility::ScientificNotationToUInt("1.2e4"));
    EXPECT_EQ(12000, PacBio::Utility::ScientificNotationToUInt("1.2E4"));

    EXPECT_EQ(12000.0, PacBio::Utility::ScientificNotationToDouble("12000"));
    EXPECT_EQ(12000.0, PacBio::Utility::ScientificNotationToDouble("1.2e4"));
    EXPECT_EQ(12000.0, PacBio::Utility::ScientificNotationToDouble("1.2E4"));
    EXPECT_EQ(-12000.0, PacBio::Utility::ScientificNotationToDouble("-12000"));
    EXPECT_EQ(-12000.0, PacBio::Utility::ScientificNotationToDouble("-1.2e4"));
    EXPECT_EQ(-12000.0, PacBio::Utility::ScientificNotationToDouble("-1.2E4"));

    EXPECT_EQ(0.00012, PacBio::Utility::ScientificNotationToDouble("0.00012"));
    EXPECT_EQ(0.00012, PacBio::Utility::ScientificNotationToDouble("1.2e-4"));
    EXPECT_EQ(0.00012, PacBio::Utility::ScientificNotationToDouble("1.2E-4"));
    EXPECT_EQ(-0.00012, PacBio::Utility::ScientificNotationToDouble("-0.00012"));
    EXPECT_EQ(-0.00012, PacBio::Utility::ScientificNotationToDouble("-1.2e-4"));
    EXPECT_EQ(-0.00012, PacBio::Utility::ScientificNotationToDouble("-1.2E-4"));
}
