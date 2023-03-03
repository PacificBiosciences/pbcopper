#include <pbcopper/cli2/internal/PositionalArgumentTranslator.h>

#include <gtest/gtest.h>

#include <pbcopper/cli2/PositionalArgument.h>
#include <pbcopper/cli2/internal/PositionalArgumentData.h>

using PositionalArgument = PacBio::CLI_v2::PositionalArgument;
using PositionalArgumentData = PacBio::CLI_v2::internal::PositionalArgumentData;
using PositionalArgumentTranslator = PacBio::CLI_v2::internal::PositionalArgumentTranslator;

// clang-format off

TEST(CLI2_PositionalArgumentTranslator, throws_on_empty_text)
{
    const PositionalArgument testArg{}; // (DB): leave these braces, because icpc
    EXPECT_THROW(PositionalArgumentTranslator::Translate(testArg), std::runtime_error);
}

TEST(CLI2_PositionalArgumentTranslator, throws_if_name_is_missing)
{
    const PositionalArgument testArg
    {
     R"({
        "description" : "test_description"
     })"
    };

    EXPECT_THROW(PositionalArgumentTranslator::Translate(testArg), std::runtime_error);
}

TEST(CLI2_PositionalArgumentTranslator, throws_if_name_is_empty)
{
    const PositionalArgument testArg
    {
     R"({
        "name" : "",
        "description" : "test_description"
     })"
    };

    EXPECT_THROW(PositionalArgumentTranslator::Translate(testArg), std::runtime_error);
}

TEST(CLI2_PositionalArgumentTranslator, throws_if_description_is_missing)
{
    const PositionalArgument testArg
    {
     R"({
        "name" : "test"
     })"
    };

    EXPECT_THROW(PositionalArgumentTranslator::Translate(testArg), std::runtime_error);
}

TEST(CLI2_PositionalArgumentTranslator, throws_if_description_is_empty)
{
    const PositionalArgument testArg
    {
     R"({
        "name" : "test",
        "description" : ""
     })"
    };

    EXPECT_THROW(PositionalArgumentTranslator::Translate(testArg), std::runtime_error);
}

TEST(CLI2_PositionalArgumentTranslator, creates_minimal_positional_arg)
{
    const PositionalArgument testArg
    {
     R"({
        "name" : "test_name",
        "description" : "test_description"
     })"
    };

    const auto posArgData = PositionalArgumentTranslator::Translate(testArg);
    EXPECT_EQ("test_name", posArgData.Name);
    EXPECT_EQ("test_description", posArgData.Description);
    EXPECT_TRUE(posArgData.Required);
    EXPECT_EQ(PacBio::CLI_v2::OptionValueType::STRING, posArgData.Type);
}

TEST(CLI2_PositionalArgumentTranslator, creates_file_positional_arg)
{
    const PositionalArgument testArg
    {
     R"({
        "name" : "test_name",
        "description" : "test_description",
        "type" : "file"
     })"
    };

    const auto posArgData = PositionalArgumentTranslator::Translate(testArg);
    EXPECT_EQ("test_name", posArgData.Name);
    EXPECT_EQ("test_description", posArgData.Description);
    EXPECT_TRUE(posArgData.Required);
    EXPECT_EQ(PacBio::CLI_v2::OptionValueType::FILE, posArgData.Type);
}

TEST(CLI2_PositionalArgumentTranslator, creates_dir_positional_arg)
{
    const PositionalArgument testArg
    {
     R"({
        "name" : "test_name",
        "description" : "test_description",
        "type" : "dir"
     })"
    };

    const auto posArgData = PositionalArgumentTranslator::Translate(testArg);
    EXPECT_EQ("test_name", posArgData.Name);
    EXPECT_EQ("test_description", posArgData.Description);
    EXPECT_TRUE(posArgData.Required);
    EXPECT_EQ(PacBio::CLI_v2::OptionValueType::DIR, posArgData.Type);
}

TEST(CLI2_PositionalArgumentTranslator, creates_optional_positional_arg)
{
    const PositionalArgument testArg
    {
     R"({
        "name" : "test_name",
        "description" : "test_description",
        "required" : false
     })"
    };
    const auto posArgData = PositionalArgumentTranslator::Translate(testArg);
    EXPECT_EQ("test_name", posArgData.Name);
    EXPECT_EQ("test_description", posArgData.Description);
    EXPECT_FALSE(posArgData.Required);
    EXPECT_EQ(PacBio::CLI_v2::OptionValueType::STRING, posArgData.Type);
}

TEST(CLI2_PositionalArgumentTranslator, can_create_description_from_array)
{
    const std::string expectedText{
        "Specifies the level of info which will be output produced on stderr. "
        "0 turns all output off, 1 outputs only warnings, while levels 2 and "
        "above outputs a status message every 1000000 (2), 100000 (3), 1000 (4), "
        "100 (5), 10 (6) and 1 (7) reads."
    };

    const PositionalArgument testArg
    {
     R"({
        "name" : "test_name",
        "description" : [
            "Specifies the level of info which will be output produced on stderr. ",
            "0 turns all output off, 1 outputs only warnings, while levels 2 and ",
            "above outputs a status message every 1000000 (2), 100000 (3), 1000 (4), ",
            "100 (5), 10 (6) and 1 (7) reads."
        ],
        "required" : false
     })"
    };

    const auto testOptionData = PositionalArgumentTranslator::Translate(testArg);
    EXPECT_EQ(expectedText, testOptionData.Description);
}

// clang-format on
