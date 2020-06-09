#include <pbcopper/cli2/internal/OptionTranslator.h>

#include <gtest/gtest.h>

#include <pbcopper/cli2/Option.h>
#include <pbcopper/cli2/OptionValue.h>
#include <pbcopper/cli2/internal/OptionData.h>
#include <pbcopper/json/JSON.h>

using Json = PacBio::JSON::Json;
using Option = PacBio::CLI_v2::Option;
using OptionValue = PacBio::CLI_v2::OptionValue;
using OptionValueType = PacBio::CLI_v2::OptionValueType;
using OptionTranslator = PacBio::CLI_v2::internal::OptionTranslator;

// clang-format off

TEST(CLI2_OptionTranslator, throws_on_empty_option_text)
{
    const Option testOption{""};
    EXPECT_THROW(OptionTranslator::Translate(testOption), std::runtime_error);
}

TEST(CLI2_OptionTranslator, throws_on_malformatted_json)
{
    const Option testOption{"{ this is not: 'valid' JSON[]"};
    EXPECT_THROW(OptionTranslator::Translate(testOption), std::runtime_error);
}

TEST(CLI2_OptionTranslator, throws_on_empty_json_definition)
{
    const Option testOption{"{}"};
    EXPECT_THROW(OptionTranslator::Translate(testOption), std::runtime_error);
}

TEST(CLI2_OptionTranslator, throws_on_empty_name)
{
    const Option testOption
    {
     R"({
        "names" : [],
        "description" : "test description"
     })"
    };
    EXPECT_THROW(OptionTranslator::Translate(testOption), std::runtime_error);
}

TEST(CLI2_OptionTranslator, throws_on_missing_name)
{
    const Option testOption
    {
     R"({
        "description" : "test description"
     })"
    };
    EXPECT_THROW(OptionTranslator::Translate(testOption), std::runtime_error);
}

TEST(CLI2_OptionTranslator, throws_on_empty_description)
{
    const Option testOption
    {
     R"({
        "names" : ["test"],
        "description" : ""
     })"
    };
    EXPECT_THROW(OptionTranslator::Translate(testOption), std::runtime_error);
}

TEST(CLI2_OptionTranslator, throws_on_missing_description)
{
    const Option testOption
    {
     R"({
        "names" : ["test"]
     })"
    };
    EXPECT_THROW(OptionTranslator::Translate(testOption), std::runtime_error);
}

TEST(CLI2_OptionTranslator, creates_basic_integer_option_data_from_text)
{
    const Option testOption
    {
     R"({
        "names" : ["test"],
        "description" : "test_description",
        "type" : "int",
        "default" : 42
     })"
    };

    const auto optionData = OptionTranslator::Translate(testOption);

    const auto& names = optionData.names;
    ASSERT_EQ(1u, names.size());
    EXPECT_EQ("test", names[0]);

    const auto value = PacBio::CLI_v2::OptionValueToInt(*optionData.defaultValue);
    EXPECT_EQ(42, value);

    EXPECT_EQ("test_description", optionData.description);
    EXPECT_EQ(OptionValueType::INT, optionData.type);

    EXPECT_FALSE(optionData.isHidden);
    EXPECT_TRUE(optionData.choices.empty());
}

TEST(CLI2_OptionTranslator, creates_basic_unsigned_integer_option_data_from_text)
{
    const Option testOption
    {
     R"({
        "names" : ["test"],
        "description" : "test_description",
        "type" : "unsigned int",
        "default" : 42
     })"
    };

    const auto optionData = OptionTranslator::Translate(testOption);
    const auto value = PacBio::CLI_v2::OptionValueToUInt(*optionData.defaultValue);
    EXPECT_EQ(42u, value);

    EXPECT_THROW({PacBio::CLI_v2::OptionValueToInt(*optionData.defaultValue); },    std::exception);
    EXPECT_THROW({PacBio::CLI_v2::OptionValueToDouble(*optionData.defaultValue); }, std::exception);
    EXPECT_THROW({PacBio::CLI_v2::OptionValueToBool(*optionData.defaultValue); },   std::exception);
    EXPECT_THROW({PacBio::CLI_v2::OptionValueToString(*optionData.defaultValue); }, std::exception);
}

TEST(CLI2_OptionTranslator, creates_basic_float_option_data_from_text)
{
    const Option testOption
    {
     R"({
        "names" : ["test"],
        "description" : "test_description",
        "type" : "float",
        "default" : 3.14
     })"
    };

    const auto optionData = OptionTranslator::Translate(testOption);

    const auto& names = optionData.names;
    ASSERT_EQ(1u, names.size());
    EXPECT_EQ("test", names[0]);

    const auto value = PacBio::CLI_v2::OptionValueToDouble(*optionData.defaultValue);
    EXPECT_EQ(3.14, value);

    EXPECT_EQ("test_description", optionData.description);
    EXPECT_EQ(OptionValueType::FLOAT, optionData.type);

    EXPECT_FALSE(optionData.isHidden);
    EXPECT_TRUE(optionData.choices.empty());
}

TEST(CLI2_OptionTranslator, creates_basic_boolean_option_data_from_text)
{
    const Option testOption
    {
     R"({
        "names" : ["test"],
        "description" : "test_description",
        "type" : "bool"
     })"
    };

    const auto optionData = OptionTranslator::Translate(testOption);

    const auto& names = optionData.names;
    ASSERT_EQ(1u, names.size());
    EXPECT_EQ("test", names[0]);

    const auto value = PacBio::CLI_v2::OptionValueToBool(*optionData.defaultValue);
    EXPECT_FALSE(value);

    EXPECT_EQ("test_description", optionData.description);
    EXPECT_EQ(OptionValueType::BOOL, optionData.type);

    EXPECT_FALSE(optionData.isHidden);
    EXPECT_TRUE(optionData.choices.empty());
}

TEST(CLI2_OptionTranslator, creates_basic_string_option_data_from_text)
{
    const Option testOption
    {
     R"({
        "names" : ["test"],
        "description" : "test_description",
        "type" : "string",
        "default" : "foo"
     })"
    };

    const auto optionData = OptionTranslator::Translate(testOption);

    const auto& names = optionData.names;
    ASSERT_EQ(1u, names.size());
    EXPECT_EQ("test", names[0]);

    const auto value = PacBio::CLI_v2::OptionValueToString(*optionData.defaultValue);
    EXPECT_EQ("foo", value);

    EXPECT_EQ("test_description", optionData.description);
    EXPECT_EQ(OptionValueType::STRING, optionData.type);

    EXPECT_FALSE(optionData.isHidden);
    EXPECT_TRUE(optionData.choices.empty());
}

TEST(CLI2_OptionTranslator, creates_basic_file_option_data_from_text)
{
    const Option testOption
    {
     R"({
        "names" : ["test"],
        "description" : "test_description",
        "type" : "file",
        "default" : "foo"
     })"
    };

    const auto optionData = OptionTranslator::Translate(testOption);

    const auto& names = optionData.names;
    ASSERT_EQ(1u, names.size());
    EXPECT_EQ("test", names[0]);

    const auto value = PacBio::CLI_v2::OptionValueToString(*optionData.defaultValue);
    EXPECT_EQ("foo", value);

    EXPECT_EQ("test_description", optionData.description);
    EXPECT_EQ(OptionValueType::FILE, optionData.type);

    EXPECT_FALSE(optionData.isHidden);
    EXPECT_TRUE(optionData.choices.empty());
}

TEST(CLI2_OptionTranslator, creates_basic_dir_option_data_from_text)
{
    const Option testOption
    {
     R"({
        "names" : ["test"],
        "description" : "test_description",
        "type" : "dir",
        "default" : "foo"
     })"
    };

    const auto optionData = OptionTranslator::Translate(testOption);

    const auto& names = optionData.names;
    ASSERT_EQ(1u, names.size());
    EXPECT_EQ("test", names[0]);

    const auto value = PacBio::CLI_v2::OptionValueToString(*optionData.defaultValue);
    EXPECT_EQ("foo", value);

    EXPECT_EQ("test_description", optionData.description);
    EXPECT_EQ(OptionValueType::DIR, optionData.type);

    EXPECT_FALSE(optionData.isHidden);
    EXPECT_TRUE(optionData.choices.empty());
}

TEST(CLI2_OptionTranslator, creates_option_with_choices)
{
    const Option testOption
    {
     R"({
        "names" : ["test"],
        "description" : "test_description",
        "type" : "string",
        "choices" : ["bar", "foo", "baz"],
        "default" : "foo"
     })"
    };

    const auto optionData = OptionTranslator::Translate(testOption);

    const auto& names = optionData.names;
    ASSERT_EQ(1u, names.size());
    EXPECT_EQ("test", names[0]);

    const auto value = PacBio::CLI_v2::OptionValueToString(*optionData.defaultValue);
    EXPECT_EQ("foo", value);

    EXPECT_EQ("test_description", optionData.description);
    EXPECT_EQ(OptionValueType::STRING, optionData.type);

    const auto& choices = optionData.choices;
    ASSERT_EQ(3u, choices.size());
    EXPECT_EQ("bar", PacBio::CLI_v2::OptionValueToString(choices[0]));
    EXPECT_EQ("foo", PacBio::CLI_v2::OptionValueToString(choices[1]));
    EXPECT_EQ("baz", PacBio::CLI_v2::OptionValueToString(choices[2]));

    EXPECT_FALSE(optionData.isHidden);
}

TEST(CLI2_OptionTranslator, creates_hidden_option)
{
    const Option testOption
    {
     R"({
        "names" : ["test"],
        "description" : "test_description",
        "type" : "int",
        "hidden" : true,
        "default" : 10
     })"
    };

    const auto optionData = OptionTranslator::Translate(testOption);

    EXPECT_TRUE(optionData.isHidden);

    const auto names = optionData.names;
    ASSERT_EQ(1u, names.size());
    EXPECT_EQ("test", names[0]);

    const auto value = PacBio::CLI_v2::OptionValueToInt(*optionData.defaultValue);
    EXPECT_EQ(10, value);

    EXPECT_EQ("test_description", optionData.description);
    EXPECT_EQ(OptionValueType::INT, optionData.type);

    EXPECT_TRUE(optionData.choices.empty());
}

TEST(CLI2_OptionTranslator, throws_if_default_string_not_in_choices_list)
{
    const Option testOption
    {
     R"({
        "names" : ["test"],
        "description" : "test_description",
        "type" : "string",
        "choices" : ["bar", "foo", "baz"],
        "default" : "nope"
     })"
    };

    EXPECT_THROW({OptionTranslator::Translate(testOption);}, std::runtime_error);
}

TEST(CLI2_OptionTranslator, can_pass_default_integer_value_at_option_ctor)
{
    const int DefaultValue = 42;

    const Option testOption
    {
     R"({
        "names" : ["test"],
        "description" : "test_description",
        "type" : "integer"
     })",
     DefaultValue
    };

    const auto testOptionData = OptionTranslator::Translate(testOption);
    EXPECT_TRUE(testOptionData.defaultValue);

    const auto value = PacBio::CLI_v2::OptionValueToInt(*testOptionData.defaultValue);
    EXPECT_EQ(DefaultValue, value);
}

TEST(CLI2_OptionTranslator, can_pass_default_unsigned_integer_value_at_option_ctor)
{
    const unsigned int DefaultValue = 42;

    const Option testOption
    {
     R"({
        "names" : ["test"],
        "description" : "test_description",
        "type" : "unsigned integer"
     })",
     DefaultValue
    };

    const auto testOptionData = OptionTranslator::Translate(testOption);
    EXPECT_TRUE(testOptionData.defaultValue);

    const auto value = PacBio::CLI_v2::OptionValueToUInt(*testOptionData.defaultValue);
    EXPECT_EQ(DefaultValue, value);
}

TEST(CLI2_OptionTranslator, can_pass_default_float_value_at_option_ctor)
{
    const double DefaultValue = 3.14;

    const Option testOption
    {
     R"({
        "names" : ["test"],
        "description" : "test_description",
        "type" : "float"
     })",
     DefaultValue
    };

    const auto testOptionData = OptionTranslator::Translate(testOption);
    EXPECT_TRUE(testOptionData.defaultValue);

    const auto value = PacBio::CLI_v2::OptionValueToDouble(*testOptionData.defaultValue);
    EXPECT_EQ(DefaultValue, value);
}

TEST(CLI2_OptionTranslator, can_pass_default_string_value_at_option_ctor)
{
    const std::string DefaultValue = "foo";

    const Option testOption
    {
     R"({
        "names" : ["test"],
        "description" : "test_description",
        "type" : "string"
     })",
     DefaultValue
    };

    const auto testOptionData = OptionTranslator::Translate(testOption);
    EXPECT_TRUE(testOptionData.defaultValue);

    const auto value = PacBio::CLI_v2::OptionValueToString(*testOptionData.defaultValue);
    EXPECT_EQ(DefaultValue, value);
}

TEST(CLI2_OptionTranslator, throws_if_default_value_is_not_in_choices)
{
    const std::string DefaultValue = "nope";

    const Option testOption
    {
     R"({
        "names" : ["test"],
        "description" : "test_description",
        "type" : "string",
        "choices" : ["foo", "bar", "baz"]
     })",
     DefaultValue
    };

    EXPECT_THROW({OptionTranslator::Translate(testOption);}, std::runtime_error);
}

TEST(CLI2_OptionTranslator, can_add_hidden_names)
{
    const std::string DefaultValue = "nope";

    const Option testOption
    {
     R"({
        "names" : ["test"],
        "names.hidden" : ["hideMe", "Who_would_name_an_Option_like__this"],
        "description" : "test_description",
        "type" : "string"
     })",
     DefaultValue
    };

    const auto testOptionData = OptionTranslator::Translate(testOption);
    ASSERT_EQ(2, testOptionData.hiddenNames.size());
    EXPECT_EQ("hideMe", testOptionData.hiddenNames[0]);
    EXPECT_EQ("Who_would_name_an_Option_like__this", testOptionData.hiddenNames[1]);
}

TEST(CLI2_OptionTranslator, can_create_description_from_array)
{
    const std::string expectedText{
        "Specifies the level of info which will be output produced on stderr. "
        "0 turns all output off, 1 outputs only warnings, while levels 2 and "
        "above outputs a status message every 1000000 (2), 100000 (3), 1000 (4), "
        "100 (5), 10 (6) and 1 (7) reads."
    };

    const Option testOption
    {
     R"({
        "names" : ["test"],
        "description" : [
            "Specifies the level of info which will be output produced on stderr. ",
            "0 turns all output off, 1 outputs only warnings, while levels 2 and ",
            "above outputs a status message every 1000000 (2), 100000 (3), 1000 (4), ",
            "100 (5), 10 (6) and 1 (7) reads."
        ],
        "type" : "bool"
     })"
    };
    const auto testOptionData = OptionTranslator::Translate(testOption);
    EXPECT_EQ(expectedText, testOptionData.description);
}

// clang-format on
