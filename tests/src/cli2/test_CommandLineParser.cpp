
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <pbcopper/cli2/internal/CommandLineParser.h>
#include <pbcopper/cli2/internal/OptionTranslator.h>

using CommandLineParser = PacBio::CLI_v2::internal::CommandLineParser;
using Interface = PacBio::CLI_v2::Interface;
using Option = PacBio::CLI_v2::Option;
using OptionTranslator = PacBio::CLI_v2::internal::OptionTranslator;
using PositionalArgument = PacBio::CLI_v2::PositionalArgument;

// clang-format off

namespace CLI_v2_CommandLineParserTests {
namespace Options {

static const Option TargetDirectory
{
R"({
    "names" : ["t", "target-dir"],
    "description" : "Copy all source files into <DIR>.",
    "type" : "string",
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
    "description" : "Choice of element indicates mood. Science.",
    "type" : "string",
    "default" : "fire",
    "choices" : ["earth", "wind", "fire", "water"]
})"
};

static const Option ToolContractOnly
{
R"({
    "names" : ["tc-only"],
    "description" : "Hidden from cmdline help, but still available from TC.",
    "hidden" : true,
    "type" : "boolean"
})"
};

static const PositionalArgument Source
{
R"({
    "name" : "source",
    "description" : "Source file to copy.",
    "syntax" : "FILE"
})"
};

static const PositionalArgument Dest
{
R"({
    "name" : "dest",
    "description" : "Destination directory.",
    "syntax" : "DIR"
})"
};

} // namespace Options

Interface makeInterface()
{
    Interface i {
        "frobber",
        "Frobb your files in a most delightful, nobbly way",
        "3.14"
    };

    i.AddOptions({
        Options::TargetDirectory,
        Options::Force,
        Options::Progress,
        Options::DryRun,
        Options::Timeout,
        Options::Delta,
        Options::Ploidy,
        Options::Element,
        Options::ToolContractOnly
    });

    i.AddPositionalArguments({
        Options::Source,
        Options::Dest
    });

    return i;
}

}  // namespace CLI_v2_CommandLineParserTests

using namespace CLI_v2_CommandLineParserTests;

TEST(CLI2_CommandLineParser, can_parse_boolean_arg_after_long_option)
{
    auto argIsTrue = [](const std::string& arg) {
        const auto i = makeInterface();
        const std::vector<std::string> args{i.ApplicationName(), "--force=" + arg, "requiredIn",
                                            "requiredOut"};
        CommandLineParser parser{i};
        const auto results = parser.Parse(args);
        const bool force = results[Options::Force];
        return force;
    };

    EXPECT_TRUE(argIsTrue("true"));
    EXPECT_TRUE(argIsTrue("TRUE"));
    EXPECT_TRUE(argIsTrue("True"));
    EXPECT_TRUE(argIsTrue("on"));
    EXPECT_TRUE(argIsTrue("ON"));
    EXPECT_TRUE(argIsTrue("On"));

    EXPECT_FALSE(argIsTrue("false"));
    EXPECT_FALSE(argIsTrue("FALSE"));
    EXPECT_FALSE(argIsTrue("False"));
    EXPECT_FALSE(argIsTrue("off"));
    EXPECT_FALSE(argIsTrue("OFF"));
    EXPECT_FALSE(argIsTrue("Off"));

    // invalid switch labels
    EXPECT_THROW(argIsTrue(""), std::runtime_error);
    EXPECT_THROW(argIsTrue("junk"), std::runtime_error);
}

TEST(CLI2_CommandLineParser, can_parse_standard_command_line)
{
    const auto i = makeInterface();
    const std::vector<std::string> args{i.ApplicationName(),
                                        "-f",
                                        "--timeout=42",
                                        "--delta=0.2",
                                        "--ploidy=diploid",
                                        "-e",
                                        "water",
                                        "--target-dir",
                                        "/path/to/dir/",
                                        "--logLevel=DEBUG",
                                        "requiredIn",
                                        "requiredOut"};

    CommandLineParser parser{i};
    const auto results = parser.Parse(args);

    const std::string targetDir = results[Options::TargetDirectory];
    const bool force = results[Options::Force];
    const bool progress = results[Options::Progress];
    const bool dryRun = results[Options::DryRun];
    const int timeout = results[Options::Timeout];
    const float delta = results[Options::Delta];
    const std::string ploidy = results[Options::Ploidy];
    const std::string element = results[Options::Element];

    EXPECT_EQ("/path/to/dir/", targetDir);
    EXPECT_TRUE(force);
    EXPECT_FALSE(progress);
    EXPECT_FALSE(dryRun);
    EXPECT_EQ(42, timeout);
    EXPECT_FLOAT_EQ(0.2, delta);
    EXPECT_EQ("diploid", ploidy);
    EXPECT_EQ("water", element);
}

TEST(CLI2_CommandLineParser, throws_on_empty_args)
{
    const auto i = makeInterface();
    const std::vector<std::string> args{};

    CommandLineParser parser{i};
    EXPECT_THROW({ const auto results = parser.Parse(args); }, std::runtime_error);
}

TEST(CLI2_CommandLineParser, can_parse_positional_args_after_double_dash)
{
    const auto i = makeInterface();
    const std::vector<std::string> args{i.ApplicationName(),
                                        "-f",
                                        "--timeout=42",
                                        "--delta=0.2",
                                        "--ploidy=diploid",
                                        "-e",
                                        "water",
                                        "--target-dir",
                                        "/path/to/dir/",
                                        "--logLevel=DEBUG",
                                        "--",
                                        "requiredIn",
                                        "requiredOut"};

    CommandLineParser parser{i};
    const auto results = parser.Parse(args);

    const std::string targetDir = results[Options::TargetDirectory];
    const bool force = results[Options::Force];
    const bool progress = results[Options::Progress];
    const bool dryRun = results[Options::DryRun];
    const int timeout = results[Options::Timeout];
    const float delta = results[Options::Delta];
    const std::string ploidy = results[Options::Ploidy];
    const std::string element = results[Options::Element];

    EXPECT_EQ("/path/to/dir/", targetDir);
    EXPECT_TRUE(force);
    EXPECT_FALSE(progress);
    EXPECT_FALSE(dryRun);
    EXPECT_EQ(42, timeout);
    EXPECT_FLOAT_EQ(0.2, delta);
    EXPECT_EQ("diploid", ploidy);
    EXPECT_EQ("water", element);
}

TEST(CLI2_CommandLineParser, long_option_expecting_value_throws_on_missing_value)
{
    const auto i = makeInterface();
    const std::vector<std::string> args{i.ApplicationName(),
                                        "-f",
                                        "--timeout=42",
                                        "--delta=0.2",
                                        "--ploidy",
                                        "-e",
                                        "water",
                                        "--target-dir",
                                        "/path/to/dir/",
                                        "--logLevel=DEBUG",
                                        "requiredIn",
                                        "requiredOut"};

    CommandLineParser parser{i};
    EXPECT_THROW({ const auto results = parser.Parse(args); }, std::runtime_error);
}

TEST(CLI2_CommandLineParser, can_parse_switch_with_true_value)
{
    const auto i = makeInterface();
    const std::vector<std::string> args{i.ApplicationName(),
                                        "--force=ON",
                                        "--timeout=42",
                                        "--delta=0.2",
                                        "--ploidy=diploid",
                                        "-e",
                                        "water",
                                        "--target-dir",
                                        "/path/to/dir/",
                                        "--logLevel=DEBUG",
                                        "requiredIn",
                                        "requiredOut"};

    CommandLineParser parser{i};
    const auto results = parser.Parse(args);

    const std::string targetDir = results[Options::TargetDirectory];
    const bool force = results[Options::Force];
    const bool progress = results[Options::Progress];
    const bool dryRun = results[Options::DryRun];
    const int timeout = results[Options::Timeout];
    const float delta = results[Options::Delta];
    const std::string ploidy = results[Options::Ploidy];
    const std::string element = results[Options::Element];

    EXPECT_EQ("/path/to/dir/", targetDir);
    EXPECT_TRUE(force);
    EXPECT_FALSE(progress);
    EXPECT_FALSE(dryRun);
    EXPECT_EQ(42, timeout);
    EXPECT_FLOAT_EQ(0.2, delta);
    EXPECT_EQ("diploid", ploidy);
    EXPECT_EQ("water", element);
}

TEST(CLI2_CommandLineParser, can_parse_switch_with_false_value)
{
    const auto i = makeInterface();
    const std::vector<std::string> args{i.ApplicationName(),
                                        "--force=OFF",
                                        "--timeout=42",
                                        "--delta=0.2",
                                        "--ploidy=diploid",
                                        "-e",
                                        "water",
                                        "--target-dir",
                                        "/path/to/dir/",
                                        "--logLevel=DEBUG",
                                        "requiredIn",
                                        "requiredOut"};

    CommandLineParser parser{i};
    const auto results = parser.Parse(args);

    const std::string targetDir = results[Options::TargetDirectory];
    const bool force = results[Options::Force];
    const bool progress = results[Options::Progress];
    const bool dryRun = results[Options::DryRun];
    const int timeout = results[Options::Timeout];
    const float delta = results[Options::Delta];
    const std::string ploidy = results[Options::Ploidy];
    const std::string element = results[Options::Element];

    EXPECT_EQ("/path/to/dir/", targetDir);
    EXPECT_FALSE(force);
    EXPECT_FALSE(progress);
    EXPECT_FALSE(dryRun);
    EXPECT_EQ(42, timeout);
    EXPECT_FLOAT_EQ(0.2, delta);
    EXPECT_EQ("diploid", ploidy);
    EXPECT_EQ("water", element);
}

TEST(CLI2_CommandLineParser, throws_on_switch_with_invalid_value)
{
    const auto i = makeInterface();
    const std::vector<std::string> args{i.ApplicationName(),
                                        "--force=foo",
                                        "--timeout=42",
                                        "--delta=0.2",
                                        "--ploidy=diploid",
                                        "-e",
                                        "water",
                                        "--target-dir",
                                        "/path/to/dir/",
                                        "--logLevel=DEBUG",
                                        "requiredIn",
                                        "requiredOut"};

    CommandLineParser parser{i};
    EXPECT_THROW({ const auto results = parser.Parse(args); }, std::runtime_error);
}

TEST(CLI2_CommandLineParser, can_handle_short_name_switch_before_positional_args)
{
    const auto i = makeInterface();
    const std::vector<std::string> args{i.ApplicationName(),
                                        "--timeout=42",
                                        "--delta=0.2",
                                        "--ploidy=diploid",
                                        "-e",
                                        "water",
                                        "--target-dir",
                                        "/path/to/dir/",
                                        "--logLevel=DEBUG",
                                        "-f",
                                        "requiredIn",
                                        "requiredOut"};

    CommandLineParser parser{i};
    const auto results = parser.Parse(args);

    const std::string targetDir = results[Options::TargetDirectory];
    const bool force = results[Options::Force];
    const bool progress = results[Options::Progress];
    const bool dryRun = results[Options::DryRun];
    const int timeout = results[Options::Timeout];
    const float delta = results[Options::Delta];
    const std::string ploidy = results[Options::Ploidy];
    const std::string element = results[Options::Element];

    EXPECT_EQ("/path/to/dir/", targetDir);
    EXPECT_TRUE(force);
    EXPECT_FALSE(progress);
    EXPECT_FALSE(dryRun);
    EXPECT_EQ(42, timeout);
    EXPECT_FLOAT_EQ(0.2, delta);
    EXPECT_EQ("diploid", ploidy);
    EXPECT_EQ("water", element);
}

TEST(CLI2_CommandLineParser, can_handle_long_name_switch_before_positional_args)
{
    const auto i = makeInterface();
    const std::vector<std::string> args{i.ApplicationName(),
                                        "--timeout=42",
                                        "--delta=0.2",
                                        "--ploidy=diploid",
                                        "-e",
                                        "water",
                                        "--target-dir",
                                        "/path/to/dir/",
                                        "--logLevel=DEBUG",
                                        "--force",
                                        "requiredIn",
                                        "requiredOut"};

    CommandLineParser parser{i};
    const auto results = parser.Parse(args);

    const std::string targetDir = results[Options::TargetDirectory];
    const bool force = results[Options::Force];
    const bool progress = results[Options::Progress];
    const bool dryRun = results[Options::DryRun];
    const int timeout = results[Options::Timeout];
    const float delta = results[Options::Delta];
    const std::string ploidy = results[Options::Ploidy];
    const std::string element = results[Options::Element];

    EXPECT_EQ("/path/to/dir/", targetDir);
    EXPECT_TRUE(force);
    EXPECT_FALSE(progress);
    EXPECT_FALSE(dryRun);
    EXPECT_EQ(42, timeout);
    EXPECT_FLOAT_EQ(0.2, delta);
    EXPECT_EQ("diploid", ploidy);
    EXPECT_EQ("water", element);
}

TEST(CLI2_CommandLineParser, displays_useful_message_on_bad_args)
{
    // rather than ungraceful 'unordered_map::at: key not found'

    CommandLineParser parser{makeInterface()};

    try {
        const std::vector<std::string> args{"frobber", "--badarg"};
        const auto results = parser.Parse(args);
    } catch (std::exception& e) {
        const std::string message{e.what()};
        EXPECT_NE("unordered_map::at: key not found", message);
        EXPECT_TRUE(message.find("unknown option") != std::string::npos);
        EXPECT_TRUE(message.find("badarg") != std::string::npos);
    }

    try {
        const std::vector<std::string> args{"frobber", "-X"};
        const auto results = parser.Parse(args);
    } catch (std::exception& e) {
        const std::string message{e.what()};
        EXPECT_NE("unordered_map::at: key not found", message);
        EXPECT_TRUE(message.find("unknown option") != std::string::npos);
        EXPECT_TRUE(message.find("X") != std::string::npos);
    }
}

// clang-format on
