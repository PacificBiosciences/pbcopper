#include <pbcopper/cli2/internal/CommandLineParser.h>

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <pbcopper/cli2/internal/OptionTranslator.h>

using CommandLineParser = PacBio::CLI_v2::internal::CommandLineParser;
using Interface = PacBio::CLI_v2::Interface;
using Option = PacBio::CLI_v2::Option;
using OptionTranslator = PacBio::CLI_v2::internal::OptionTranslator;
using PositionalArgument = PacBio::CLI_v2::PositionalArgument;

// clang-format off

namespace CLI_v2_CommandLineParserTests {
namespace Options {

const Option TargetDirectory
{
R"({
    "names" : ["t", "target-dir"],
    "description" : "Copy all source files into <DIR>.",
    "type" : "string",
    "default" : "my/default/dir"
})"
};

const Option Force
{
R"({
    "names" : ["f", "force"],
    "description" : "Overwrite things.",
    "type" : "boolean"
})"
};

const Option Progress
{
R"({
    "names" : ["p"],
    "description" : "Show progress during copy.",
    "type" : "boolean"
})"
};

const Option DryRun
{
R"({
    "names" : ["n", "no-op"],
    "description" : "Dry run. Report actions that would be taken but do not perform them.",
    "type" : "boolean"
})"
};

const Option Timeout
{
R"({
    "names" : ["timeout"],
    "description" : "Abort execution after <INT> milliseconds.",
    "type" : "integer",
    "default" : 5000
})"
};

const Option Delta
{
R"({
    "names" : ["delta"],
    "description" : "Some delta for things.",
    "type" : "float",
    "default" : 0.01
})"
};

const Option Ploidy
{
R"({
    "names" : ["ploidy"],
    "description" : "Genome ploidy.",
    "type" : "string",
    "default" : "haploid",
    "choices" : ["haploid", "diploid"]
})"
};

const Option Element
{
R"({
    "names" : ["e", "element"],
    "description" : "Choice of element indicates mood. Science.",
    "type" : "string",
    "default" : "fire",
    "choices" : ["earth", "wind", "fire", "water"]
})"
};

const Option ToolContractOnly
{
R"({
    "names" : ["tc-only"],
    "description" : "Hidden from cmdline help, but still available from TC.",
    "hidden" : true,
    "type" : "boolean"
})"
};

const PositionalArgument Source
{
R"({
    "name" : "source",
    "description" : "Source file to copy.",
    "syntax" : "FILE"
})"
};

const PositionalArgument Dest
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
    } catch (const std::exception& e) {
        const std::string message{e.what()};
        EXPECT_NE("unordered_map::at: key not found", message);
        EXPECT_TRUE(message.find("unknown option") != std::string::npos);
        EXPECT_TRUE(message.find("badarg") != std::string::npos);
    }

    try {
        const std::vector<std::string> args{"frobber", "-X"};
        const auto results = parser.Parse(args);
    } catch (const std::exception& e) {
        const std::string message{e.what()};
        EXPECT_NE("unordered_map::at: key not found", message);
        EXPECT_TRUE(message.find("unknown option") != std::string::npos);
        EXPECT_TRUE(message.find("X") != std::string::npos);
    }
}

TEST(CLI2_CommandLineParser, can_interpret_consecutive_short_options)
{
    const Option ShortFlag_X{
    R"({
        "names" : ["x"],
        "description" : "short flag"
    })"};
    const Option ShortFlag_Y{
    R"({
        "names" : ["y"],
        "description" : "short flag"
    })"};
    const Option ShortFlag_Z{
    R"({
        "names" : ["z"],
        "description" : "short flag"
    })"};

    Interface i {
        "frobber",
        "Frobb your files in a most delightful, nobbly way",
        "3.14"
    };
    i.AddOptions(
    {
        ShortFlag_X,
        ShortFlag_Y,
        ShortFlag_Z
    });

    const std::vector<std::string> args{
        "frobber",
        "-xz"
    };

    CommandLineParser parser{i};
    const auto results = parser.Parse(args);

    EXPECT_TRUE(results[ShortFlag_X]);
    EXPECT_FALSE(results[ShortFlag_Y]);
    EXPECT_TRUE(results[ShortFlag_Z]);
}

TEST(CLI2_CommandLineParser, can_interpret_negative_values_for_options)
{
    const Option ShortOptionAttached{
    R"({
        "names" : ["x"],
        "description" : "short attached",
        "type" : "int",
        "default" : 0
    })"};

    const Option ShortOptionDetached{
    R"({
        "names" : ["y"],
        "description" : "short detached",
        "type" : "int",
        "default" : 0
    })"};

    const Option ShortFlag{
    R"({
        "names" : ["z"],
        "description" : "short flag"
    })"};

    const Option LongOptionAttached{
    R"({
        "names" : ["attached"],
        "description" : "attached",
        "type" : "int",
        "default" : 0
    })"};

    const Option LongOptionDetached{
    R"({
        "names" : ["detached"],
        "description" : "detached",
        "type" : "int",
        "default" : 0
    })"};

    Interface i {
        "frobber",
        "Frobb your files in a most delightful, nobbly way",
        "3.14"
    };
    i.AddOptions(
    {
        ShortOptionAttached,
        ShortOptionDetached,
        ShortFlag,
        LongOptionAttached,
        LongOptionDetached
    });

    const std::vector<std::string> args{
        "frobber",
        "-x=-1",            // short option, attached via '='
        "-y",               // short option, detached
        "-6",
        "--attached=-8",    // long option, attached via '='
        "--detached",       // long option, detached
        "-42"
    };

    const CommandLineParser parser{i};
    const auto results = parser.Parse(args);

    const int shortAttachedValue = results[ShortOptionAttached];
    const int shortDetachedValue = results[ShortOptionDetached];
    const int longAttachedValue = results[LongOptionAttached];
    const int longDetachedValue = results[LongOptionDetached];
    EXPECT_EQ(-1,  shortAttachedValue);
    EXPECT_EQ(-6,  shortDetachedValue);
    EXPECT_EQ(-8,  longAttachedValue);
    EXPECT_EQ(-42, longDetachedValue);
}
TEST(CLI2_CommandLineParser, throws_if_expected_value_is_next_option_instead)
{
    const Option ShortOption{
    R"({
        "names" : ["x"],
        "description" : "short option",
        "type" : "int",
        "default" : 0
    })"};

    const Option LongOption{
    R"({
        "names" : ["long-y"],
        "description" : "long option",
        "type" : "int",
        "default" : 0
    })"};

    Interface i {
        "frobber",
        "Frobb your files in a most delightful, nobbly way",
        "3.14"
    };

    i.AddOptions(
    {
        ShortOption,
        LongOption
    });

    const CommandLineParser parser{i};

    {
        const std::vector<std::string> args{
            "frobber",
            "-x",           // value expected for short option, but given another option instead
            "-z"
        };

        try {
            const auto results = parser.Parse(args);
            ASSERT_TRUE(false); // should not execute
        } catch (const std::exception& e)
        {
            const std::string msg{e.what()};
            EXPECT_TRUE(msg.find("value is missing for option 'x'") != std::string::npos);
        }
    }
    {
        const std::vector<std::string> args{
            "frobber",
            "--long-y",     // value expected for long option, but given another option instead
            "-x"
        };

        try {
            const auto results = parser.Parse(args);
            ASSERT_TRUE(false); // should not execute
        } catch (const std::exception& e)
        {
            const std::string msg{e.what()};
            EXPECT_TRUE(msg.find("value is missing for option 'long-y'") != std::string::npos);
        }
    }
}

TEST(CLI2_CommandLineParser, does_not_allow_negative_value_for_unsigned_option)
{
    const Option UIntOption{
    R"({
        "names" : ["x"],
        "description" : "x",
        "type" : "unsigned int",
        "default" : 0
    })"};
    Interface i {
        "frobber",
        "Frobb your files in a most delightful, nobbly way",
        "3.14"
    };
    i.AddOption(UIntOption);

    try {
        const CommandLineParser parser{i};
        parser.Parse({"frobber", "-x", "-42"});
        ASSERT_TRUE(false); // should not execute
    } catch (const std::exception& e) {
        const std::string msg{e.what()};
        EXPECT_TRUE(msg.find("negative value -42 is not allowed for option 'x'") != std::string::npos);
    }
}

TEST(CLI2_CommandLineParser, does_not_allow_negative_value_for_string_option)
{
    const Option StringOption{
    R"({
        "names" : ["x"],
        "description" : "x",
        "type" : "string"
    })"};
    Interface i {
        "frobber",
        "Frobb your files in a most delightful, nobbly way",
        "3.14"
    };
    i.AddOption(StringOption);

    try {
        const CommandLineParser parser{i};
        parser.Parse({"frobber", "-x", "-42"});
        ASSERT_TRUE(false); // should not execute
    } catch (const std::exception& e) {
        const std::string msg{e.what()};
        EXPECT_TRUE(msg.find("negative value -42 is not allowed for option 'x'") != std::string::npos);
    }
}

TEST(CLI2_CommandLineParser, can_interpret_numerics_with_si_suffix)
{
    const Option UIntOption{
    R"({
        "names" : ["x"],
        "description" : "x",
        "type" : "unsigned int",
        "default" : 0
    })"};
    const Option IntOption{
    R"({
        "names" : ["yoink"],
        "description" : "y",
        "type" : "int",
        "default" : 0
    })"};
    const Option FloatOption{
    R"({
        "names" : ["z"],
        "description" : "z",
        "type" : "float",
        "default" : 0.0
    })"};

    Interface i {
        "frobber",
        "Frobb your files in a most delightful, nobbly way",
        "3.14"
    };
    i.AddOption(UIntOption)
     .AddOption(IntOption)
     .AddOption(FloatOption);

    const CommandLineParser parser{i};
    const auto results = parser.Parse({
        "frobber",
        "-x", "42K",
        "--yoink=300m",
        "-z", "3.14G"
    });

    const uint64_t x = results[UIntOption];
    const int64_t y = results[IntOption];
    const double z = results[FloatOption];

    EXPECT_EQ(x, 42'000);
    EXPECT_EQ(y, 300'000'000);
    EXPECT_EQ(z, 3'140'000'000);
}

TEST(CLI2_CommandLineParser, can_interpret_numerics_with_scientific_notation)
{
    const Option UIntOption{
    R"({
        "names" : ["x"],
        "description" : "x",
        "type" : "unsigned int",
        "default" : 0
    })"};
    const Option IntOption{
    R"({
        "names" : ["yoink"],
        "description" : "y",
        "type" : "int",
        "default" : 0
    })"};
    const Option FloatOption{
    R"({
        "names" : ["z"],
        "description" : "z",
        "type" : "float",
        "default" : 0.0
    })"};

    Interface i {
        "frobber",
        "Frobb your files in a most delightful, nobbly way",
        "3.14"
    };
    i.AddOption(UIntOption)
     .AddOption(IntOption)
     .AddOption(FloatOption);

    const CommandLineParser parser{i};
    const auto results = parser.Parse({
        "frobber",
        "-x", "1.5e6",
        "--yoink=-3E7",
        "-z", "1.2e-4"
    });

    const uint64_t x = results[UIntOption];
    const int64_t y = results[IntOption];
    const double z = results[FloatOption];

    EXPECT_EQ(x, 1'500'000);
    EXPECT_EQ(y, -30'000'000);
    EXPECT_EQ(z, 0.00012);
}

TEST(CLI2_CommandLineParser, can_interpret_dash_option_value)
{
    const PacBio::CLI_v2::PositionalArgument Input {
    R"({
        "name" : "input",
        "description" : "Input file(s)."
    })"};

    const PacBio::CLI_v2::PositionalArgument StatsFile {
    R"({
        "name" : "stats",
        "description" : "Stats file."
    })"};

    const PacBio::CLI_v2::Option OutputPrefix {
    R"({
        "names" : ["o", "output"],
        "description" : [
            "Prefix of output filenames, '-' implies streaming. Streaming not supported ",
            "with compression nor with split_barcodes"
        ],
        "type" : "string"
    })"};

    const PacBio::CLI_v2::Option CompressionLevel {
    R"({
        "names" : ["c"],
        "description" : "Gzip compression level [1-9]",
        "type" : "int",
        "default" : 1
    })"};

    PacBio::CLI_v2::Interface i{"app", "desc", "0.0"};
    i.DisableLogLevelOption()
     .DisableLogFileOption()
     .DisableNumThreadsOption();

    i.AddPositionalArguments({Input, StatsFile});
    i.AddOptionGroup("Options", {OutputPrefix, CompressionLevel});

    const CommandLineParser parser{i};
    const auto results = parser.Parse({
        "app",
        "input.bam",
        "-o", "-",
        "-c", "7",
        "stats.txt"
    });

    const std::string inputFile = results[Input];
    const std::string statsFile = results[StatsFile];
    const std::string outputPrefix = results[OutputPrefix];
    const int compressionLevel = results[CompressionLevel];

    EXPECT_EQ("input.bam", inputFile);
    EXPECT_EQ("stats.txt", statsFile);
    EXPECT_EQ("-", outputPrefix);
    EXPECT_EQ(7, compressionLevel);
}

// clang-format on
