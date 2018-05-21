

#include <gtest/gtest.h>
#include <pbcopper/utility/StringUtils.h>

// clang-format off

namespace StringUtilsTests {

void checkWrappedLines(const std::string& input, const std::vector<std::string>& expected, const size_t maxColumns)
{
    const auto lines = PacBio::Utility::WordWrappedLines(input, maxColumns);
    ASSERT_EQ(expected.size(), lines.size());
    for (size_t i = 0; i < expected.size(); ++i)
        EXPECT_EQ(expected.at(i), lines.at(i));
}

} // namespace StringUtilsTests

TEST(Utility_StringUtils, word_wrapping_empty_string_yields_empty_result)
{
    const std::string input;
    const std::vector<std::string> expected;
    StringUtilsTests::checkWrappedLines(input, expected, 80);
}

TEST(Utility_StringUtils, word_wrapping_shorter_than_max_yields_one_line)
{
    const std::string input{"Short description."};
    const std::vector<std::string> expected {
        "Short description."
    };
    StringUtilsTests::checkWrappedLines(input, expected, 80);
}

TEST(Utility_StringUtils, word_wrapping_longer_than_max_yields_wrapped_line)
{
    // NOTE(DB): You MUST keep clang-format off here. It messes up the line breaks
    //           we're testing.

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
}

// clang-format on