#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <pbcopper/cli2/Interface.h>
#include <pbcopper/cli2/MultiToolInterface.h>
#include <pbcopper/cli2/internal/InterfaceHelpPrinter.h>
#include <pbcopper/cli2/internal/OptionData.h>
#include <pbcopper/cli2/internal/OptionTranslator.h>
#include <pbcopper/cli2/internal/PositionalArgumentTranslator.h>

using Interface = PacBio::CLI_v2::Interface;
using InterfaceHelpPrinter = PacBio::CLI_v2::internal::InterfaceHelpPrinter;
using MultiToolInterface = PacBio::CLI_v2::MultiToolInterface;
using Option = PacBio::CLI_v2::Option;
using OptionData = PacBio::CLI_v2::internal::OptionData;
using OptionGroupData = PacBio::CLI_v2::internal::OptionGroupData;
using OptionTranslator = PacBio::CLI_v2::internal::OptionTranslator;
using OptionValue = PacBio::CLI_v2::OptionValue;
using OptionValueType = PacBio::CLI_v2::OptionValueType;
using PositionalArgument = PacBio::CLI_v2::PositionalArgument;
using PositionalArgumentTranslator = PacBio::CLI_v2::internal::PositionalArgumentTranslator;
using Tool = PacBio::CLI_v2::Tool;

// clang-format off

namespace CLI_v2_InterfaceHelpPrinterTests {
namespace Options {

static const Option TargetDirectory
{
R"({
    "names" : ["t", "target-dir"],
    "description" : "Copy all source files into <DIR>.",
    "type" : "dir",
    "default" : "my/default/dir"
})"
};

static const Option Force
{
R"({
    "names" : ["f", "force"],
    "description" : "Overwrite things.",
    "type" : "boolean"
})"
};

static const Option Progress
{
R"({
    "names" : ["p"],
    "description" : "Show progress during copy.",
    "type" : "boolean"
})"
};

static const Option DryRun
{
R"({
    "names" : ["n", "no-op"],
    "description" : "Dry run. Report actions that would be taken but do not perform them.",
    "type" : "boolean"
})"
};

static const Option Timeout
{
R"({
    "names" : ["timeout"],
    "description" : "Abort execution after <INT> milliseconds.",
    "type" : "integer",
    "default" : 5000
})"
};

static const Option Delta
{
R"({
    "names" : ["delta"],
    "description" : "Some delta for things.",
    "type" : "float",
    "default" : 0.01
})"
};

static const Option Ploidy
{
R"({
    "names" : ["ploidy"],
    "description" : "Genome ploidy.",
    "type" : "string",
    "default" : "haploid",
    "choices" : ["haploid", "diploid"]
})"
};

static const Option Element
{
R"({
    "names" : ["e", "element"],
    "names.hidden" : ["elementChoice"],
    "description" : "Choice of element indicates mood. Science.",
    "type" : "string",
    "default" : "fire",
    "choices" : ["earth", "wind", "fire", "water"]
})"
};

static const PositionalArgument Source
{
R"({
    "name" : "source",
    "description" : "Source file to copy.",
    "type" : "file"
})"
};

static const PositionalArgument Dest
{
R"({
    "name" : "dest",
    "description" : "Destination directory. Essentially where we want to drop things, but really just making a long description.",
    "type" : "dir"
})"
};

static const PositionalArgument OptionalStats
{
R"({
    "name" : "stats",
    "description" : "Write stats to file, rather than to stdout.",
    "type" : "FILE",
    "required" : false
})"
};

}  // namespace Options
}  // namespace CLI_v2_InterfaceHelpPrinterTests

TEST(CLI2_InterfaceHelpPrinter, formats_usage)
{
    const std::string expectedText{"Usage:\n  frobber [options]"};

    Interface i{"frobber"};

    InterfaceHelpPrinter help{i};
    const auto formattedText = help.Usage();
    EXPECT_EQ(expectedText, formattedText);
}

TEST(CLI2_InterfaceHelpPrinter, formats_usage_with_pos_args)
{
    const std::string expectedText{"Usage:\n  frobber [options] <source> <dest> [stats]"};

    Interface i{"frobber"};
    i.AddPositionalArguments({
        CLI_v2_InterfaceHelpPrinterTests::Options::Source,
        CLI_v2_InterfaceHelpPrinterTests::Options::Dest,
        CLI_v2_InterfaceHelpPrinterTests::Options::OptionalStats
    });

    InterfaceHelpPrinter help{i};
    const auto formattedText = help.Usage();
    EXPECT_EQ(expectedText, formattedText);
}

TEST(CLI2_InterfaceHelpPrinter, usage_with_optional_pos_args_always_places_them_after_required_args)
{
    const std::string expectedText{"Usage:\n  frobber [options] <source> <dest> [stats]"};

    Interface i{"frobber"};
    i.AddPositionalArguments({
        CLI_v2_InterfaceHelpPrinterTests::Options::Source,
        CLI_v2_InterfaceHelpPrinterTests::Options::OptionalStats,
        CLI_v2_InterfaceHelpPrinterTests::Options::Dest
    });

    InterfaceHelpPrinter help{i};
    const auto formattedText = help.Usage();
    EXPECT_EQ(expectedText, formattedText);
}

TEST(CLI2_InterfaceHelpPrinter, formats_description)
{
    const std::string expectedText{"some description"};

    Interface i{"frobber", "some description"};

    InterfaceHelpPrinter help{i};
    const auto formattedText = help.Description();
    EXPECT_EQ(expectedText, formattedText);
}

TEST(CLI2_InterfaceHelpPrinter, formats_empty_description)
{
    const std::string expectedText{""};

    Interface i{"frobber", ""};

    InterfaceHelpPrinter help{i};
    const auto formattedText = help.Description();
    EXPECT_EQ(expectedText, formattedText);
}

TEST(CLI2_InterfaceHelpPrinter, formats_option_names)
{
    const std::vector<std::string> expected{"-h,--help", "--log-level", "--version"};

    Interface i{"frobber"};
    InterfaceHelpPrinter help{i};
    const auto formattedHelp = help.OptionNames(i.HelpOption());
    const auto formattedLogLevel = help.OptionNames(i.LogLevelOption());
    const auto formattedVersion = help.OptionNames(i.VersionOption());
    EXPECT_EQ(expected[0], formattedHelp);
    EXPECT_EQ(expected[1], formattedLogLevel);
    EXPECT_EQ(expected[2], formattedVersion);
}

TEST(CLI2_InterfaceHelpPrinter, no_formatted_option_names_for_hidden_option)
{
    OptionData option;
    option.isHidden = true;

    const std::string expectedText{""};

    Interface i{"frobber"};
    InterfaceHelpPrinter help{i};
    const auto formattedText = help.OptionNames(option);
    EXPECT_EQ(expectedText, formattedText);
}

TEST(CLI2_InterfaceHelpPrinter, can_calculate_metrics_from_builtins_only)
{
    Interface i{"frobber"};
    InterfaceHelpPrinter help{i};

    const auto& metrics = help.Metrics();

    const std::string longestTest{"-j,--num-threads"};
    EXPECT_EQ(longestTest.size(), metrics.maxNameLength);
}

TEST(CLI2_InterfaceHelpPrinter, can_calculate_metrics_with_long_option)
{
    const Option testOption{
        R"({
        "names" : ["super-long-option-name-that-is-longer-than-loglevel"],
        "description" : "foo"
     })"};

    const std::string longestText{
        "--super-long-option-name-that-is-longer-than-loglevel",
    };

    const std::string helpText{"-h,--help"};
    const std::string logLevelText{"--log-level"};
    const std::string versionText{"--version"};

    Interface i{"frobber"};
    i.AddOption(testOption);

    InterfaceHelpPrinter help{i};
    const auto& metrics = help.Metrics();
    EXPECT_EQ(longestText.size(), metrics.maxNameLength);  // include spacer

    const auto testOptionData = OptionTranslator::Translate(testOption);
    const auto& helpOption = i.HelpOption();
    const auto& logLevelOption = i.LogLevelOption();
    const auto& versionOption = i.VersionOption();
    EXPECT_EQ(longestText,  metrics.formattedOptionNames.at(testOptionData).nameString);
    EXPECT_EQ(helpText,     metrics.formattedOptionNames.at(helpOption).nameString);
    EXPECT_EQ(logLevelText, metrics.formattedOptionNames.at(logLevelOption).nameString);
    EXPECT_EQ(versionText,  metrics.formattedOptionNames.at(versionOption).nameString);
}

TEST(CLI2_InterfaceHelpPrinter, formats_int_choices)
{
    const std::string expectedText{"0, 5, 10, 15"};

    OptionValue choice1 = 0;
    OptionValue choice2 = 5;
    OptionValue choice3 = 10;
    OptionValue choice4 = 15;

    OptionData option;
    option.type = OptionValueType::INT;
    option.choices = {choice1, choice2, choice3, choice4};

    Interface i{"frobber"};
    InterfaceHelpPrinter help{i};

    const auto formattedText = help.Choices(option);
    EXPECT_EQ(expectedText, formattedText);
}

TEST(CLI2_InterfaceHelpPrinter, formats_float_choices)
{
    const std::string expectedText{"0.1, 0.5, 1.05, 1.5"};

    OptionValue choice1 = 0.1f;
    OptionValue choice2 = 0.5f;
    OptionValue choice3 = 1.05f;
    OptionValue choice4 = 1.5f;

    OptionData option;
    option.type = OptionValueType::FLOAT;
    option.choices = {choice1, choice2, choice3, choice4};

    Interface i{"frobber"};
    InterfaceHelpPrinter help{i};

    const auto formattedText = help.Choices(option);
    EXPECT_EQ(expectedText, formattedText);
}

TEST(CLI2_InterfaceHelpPrinter, formats_string_choices)
{
    const std::string expectedText{"fire, water, air, earth"};

    OptionValue choice1 = std::string{"fire"};
    OptionValue choice2 = std::string{"water"};
    OptionValue choice3 = std::string{"air"};
    OptionValue choice4 = std::string{"earth"};

    OptionData option;
    option.type = OptionValueType::STRING;
    option.choices = {choice1, choice2, choice3, choice4};

    Interface i{"frobber"};
    InterfaceHelpPrinter help{i};

    const auto formattedText = help.Choices(option);
    EXPECT_EQ(expectedText, formattedText);
}

TEST(CLI2_InterfaceHelpPrinter, should_not_show_default_value_for_boolean)
{
    OptionData option;
    Interface i{"frobber"};
    InterfaceHelpPrinter help{i};
    EXPECT_FALSE(help.ShouldShowDefaultValue(option));
}

TEST(CLI2_InterfaceHelpPrinter, should_not_show_default_value_for_empty_string)
{
    OptionData option;
    option.type = OptionValueType::STRING;
    option.defaultValue = std::string{};
    Interface i{"frobber"};
    InterfaceHelpPrinter help{i};
    EXPECT_FALSE(help.ShouldShowDefaultValue(option));
}

TEST(CLI2_InterfaceHelpPrinter, should_show_default_value_for_string)
{
    OptionData option;
    option.type = OptionValueType::STRING;
    option.defaultValue = std::string{"foo"};
    Interface i{"frobber"};
    InterfaceHelpPrinter help{i};
    EXPECT_TRUE(help.ShouldShowDefaultValue(option));
}

TEST(CLI2_InterfaceHelpPrinter, should_show_default_value_for_number)
{
    OptionData option;
    option.type = OptionValueType::INT;
    option.defaultValue = 42;
    Interface i{"frobber"};
    InterfaceHelpPrinter help{i};
    EXPECT_TRUE(help.ShouldShowDefaultValue(option));
}

TEST(CLI2_InterfaceHelpPrinter, should_not_show_default_value_for_number_with_no_default)
{
    OptionData option;
    option.type = OptionValueType::INT;
    Interface i{"frobber"};
    InterfaceHelpPrinter help{i};
    EXPECT_FALSE(help.ShouldShowDefaultValue(option));
}

TEST(CLI2_InterfaceHelpPrinter, formats_default_value)
{
    OptionData option;
    option.type = OptionValueType::INT;
    option.defaultValue = 42;

    const std::string expectedText{"42"};

    Interface i{"frobber"};
    InterfaceHelpPrinter help{i};
    const auto defaultValue = help.DefaultValue(option);
    EXPECT_EQ(expectedText, defaultValue);
}

TEST(CLI2_InterfaceHelpPrinter, formats_option_with_wordwrapped_description)
{
//                                                                               80
//                                                                               v
    const std::string expectedText{
"  -x                      Lorem ipsum dolor sit amet, consectetur adipiscing\n"
"                          elit, sed do eiusmod tempor incididunt ut labore et\n"
"                          dolore magna aliqua."};
/*
"  -j,--num-threads  INT   Number of threads to use, 0 means autodetection. [0]        <-- initial spacing to description based on this built-in
*/
    const Option optionWithLongDescription{
        R"({
        "names" : ["x"],
        "description" : "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua."
     })"};

    const auto option = OptionTranslator::Translate(optionWithLongDescription);

    Interface i{"frobber"};
    i.AddOption(optionWithLongDescription);

    InterfaceHelpPrinter help{i, 80};
    const auto formattedText = help.Option(option);
    EXPECT_EQ(expectedText, formattedText);
}

TEST(CLI2_InterfaceHelpPrinter, formats_empty_option_group)
{
    const std::string expectedText{""};

    Interface i{"frobber"};
    InterfaceHelpPrinter help{i};
    const auto formattedText = help.OptionGroup(OptionGroupData{});
    EXPECT_EQ(expectedText, formattedText);
}

TEST(CLI2_InterfaceHelpPrinter, formats_builtin_option_group)
{
    const std::string expectedText{
        "Options:\n"
        "  -h,--help               Show this help and exit.\n"
        "  --log-level       STR   Set log level. Valid choices: (TRACE, DEBUG, INFO, WARN, FATAL). [WARN]\n"
        "  --log-file        FILE  Log to a file, instead of stderr.\n"
        "  -j,--num-threads  INT   Number of threads to use, 0 means autodetection. [0]\n"
        "  --version               Show application version and exit.\n"};

    Interface i{"frobber"};
    InterfaceHelpPrinter help{i};
    const auto formattedText = help.Options();
    EXPECT_EQ(expectedText, formattedText);
}

TEST(CLI2_InterfaceHelpPrinter, prints_log_level_override)
{
    const std::string expectedText{
        "Options:\n"
        "  -h,--help               Show this help and exit.\n"
        "  --log-level       STR   Set log level. Valid choices: (TRACE, DEBUG, INFO, WARN, FATAL). [DEBUG]\n"
        "  --log-file        FILE  Log to a file, instead of stderr.\n"
        "  -j,--num-threads  INT   Number of threads to use, 0 means autodetection. [0]\n"
        "  --version               Show application version and exit.\n"};

    Interface i{"frobber"};
    i.DefaultLogLevel(PacBio::Logging::LogLevel::DEBUG);
    InterfaceHelpPrinter help{i};
    const auto formattedText = help.Options();
    EXPECT_EQ(expectedText, formattedText);
}

TEST(CLI2_InterfaceHelpPrinter, formats_option_group)
{
//                                                                               80
//                                                                               v
    const std::string expectedText{
"Test Group:\n"
"  -f,--force              Overwrite things.\n"
"  -p                      Show progress during copy.\n"
"  -n,--no-op              Dry run. Report actions that would be taken but do not\n"
"                          perform them.\n"
/*
"  -j,--num-threads  INT   Number of threads to use, 0 means autodetection. [0]        <-- initial spacing to description based on this built-in
*/
    };

    const std::vector<Option> options{
        CLI_v2_InterfaceHelpPrinterTests::Options::Force,
        CLI_v2_InterfaceHelpPrinterTests::Options::Progress,
        CLI_v2_InterfaceHelpPrinterTests::Options::DryRun};

    auto optionsData = OptionTranslator::Translate(options);

    Interface i{"frobber"};
    i.AddOptionGroup("Test Group", options);
    InterfaceHelpPrinter help{i, 80};
    const auto formattedText = help.OptionGroup({"Test Group", optionsData});
    EXPECT_EQ(expectedText, formattedText);
}

TEST(CLI2_InterfaceHelpPrinter, formats_positional_argument)
{
    const std::string expectedText{
"  source            FILE  Source file to copy."
    };
/*
"  -j,--num-threads  INT   Number of threads to use, 0 means autodetection. [0]       <-- initial spacing to type based on this built-in
*/

    Interface i{"frobber"};
    i.AddPositionalArgument(CLI_v2_InterfaceHelpPrinterTests::Options::Source);

    const auto posArgData = PositionalArgumentTranslator::Translate(CLI_v2_InterfaceHelpPrinterTests::Options::Source);

    InterfaceHelpPrinter help{i, 80};
    const auto formattedText = help.PositionalArgument(posArgData);
    EXPECT_EQ(expectedText, formattedText);
}

TEST(CLI2_InterfaceHelpPrinter, formats_positional_argument_with_wordwrapped_description)
{
//                                                                               80
//                                                                               v
   const std::string expectedText{
"  source            FILE  Source file to copy.\n"
"  dest              DIR   Destination directory. Essentially where we want to\n"
"                          drop things, but really just making a long\n"
"                          description.\n"
    };
/*
"  -j,--num-threads  INT   Number of threads to use, 0 means autodetection. [0]        <-- initial spacing to description based on this built-in
*/

    Interface i{"frobber"};
    i.AddPositionalArguments({
        CLI_v2_InterfaceHelpPrinterTests::Options::Source,
        CLI_v2_InterfaceHelpPrinterTests::Options::Dest
    });

    const auto posArgData = PositionalArgumentTranslator::Translate(CLI_v2_InterfaceHelpPrinterTests::Options::Source);

    InterfaceHelpPrinter help{i, 80};
    const auto formattedText = help.PositionalArguments();
    EXPECT_EQ(expectedText, formattedText);
}

TEST(CLI2_InterfaceHelpPrinter, prints_expected_full_interface_help)
{
    const std::string expectedText{R"(frobber - Frobb your files in a most delightful, nobbly way

Usage:
  frobber [options] <source> <dest> [stats]

  source            FILE   Source file to copy.
  dest              DIR    Destination directory. Essentially where we want to
                           drop things, but really just making a long
                           description.
  stats             FILE   Write stats to file, rather than to stdout.

Output Options:
  -p                       Show progress during copy.
  -f,--force               Overwrite things.
  -t,--target-dir   DIR    Copy all source files into <DIR>. [my/default/dir]
  -n,--no-op               Dry run. Report actions that would be taken but do
                           not perform them.
  --timeout         INT    Abort execution after <INT> milliseconds. [5000]

Algorithm Options:
  --delta           FLOAT  Some delta for things. [0.01]
  -e,--element      STR    Choice of element indicates mood. Science. Valid
                           choices: (earth, wind, fire, water). [fire]
  --ploidy          STR    Genome ploidy. Valid choices: (haploid, diploid).
                           [haploid]

  -h,--help                Show this help and exit.
  --log-level       STR    Set log level. Valid choices: (TRACE, DEBUG, INFO,
                           WARN, FATAL). [WARN]
  --log-file        FILE   Log to a file, instead of stderr.
  -j,--num-threads  INT    Number of threads to use, 0 means autodetection. [0]
  --version                Show application version and exit.

)"};

    Interface i {
        "frobber",
        "Frobb your files in a most delightful, nobbly way",
        "3.14"
    };

    i.AddOptionGroup("Output Options", {
        CLI_v2_InterfaceHelpPrinterTests::Options::Progress,
        CLI_v2_InterfaceHelpPrinterTests::Options::Force,
        CLI_v2_InterfaceHelpPrinterTests::Options::TargetDirectory,
        CLI_v2_InterfaceHelpPrinterTests::Options::DryRun,
        CLI_v2_InterfaceHelpPrinterTests::Options::Timeout
    });
    i.AddOptionGroup("Algorithm Options",{
        CLI_v2_InterfaceHelpPrinterTests::Options::Delta,
        CLI_v2_InterfaceHelpPrinterTests::Options::Element,
        CLI_v2_InterfaceHelpPrinterTests::Options::Ploidy
    });

    i.AddPositionalArguments({
        CLI_v2_InterfaceHelpPrinterTests::Options::Source,
        CLI_v2_InterfaceHelpPrinterTests::Options::Dest,
        CLI_v2_InterfaceHelpPrinterTests::Options::OptionalStats
    });

    const InterfaceHelpPrinter help{i, 80};
    std::ostringstream out;
    help.Print(out);
    EXPECT_EQ(expectedText, out.str());

    // reset ostream and check operator<<
    out.str("");
    out << help;
    EXPECT_EQ(expectedText, out.str());
}

// clang-format on
