#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <pbcopper/cli2/Interface.h>
#include <pbcopper/cli2/Option.h>
#include <pbcopper/cli2/PositionalArgument.h>
#include <pbcopper/cli2/Results.h>
#include <pbcopper/cli2/internal/CommandLineParser.h>
#include <pbcopper/cli2/internal/OptionTranslator.h>
#include <pbcopper/cli2/internal/PositionalArgumentTranslator.h>

using CommandLineParser = PacBio::CLI_v2::internal::CommandLineParser;
using Interface = PacBio::CLI_v2::Interface;
using Option = PacBio::CLI_v2::Option;
using OptionTranslator = PacBio::CLI_v2::internal::OptionTranslator;
using PositionalArgument = PacBio::CLI_v2::PositionalArgument;
using PositionalArgumentTranslator = PacBio::CLI_v2::internal::PositionalArgumentTranslator;
using Result = PacBio::CLI_v2::Result;
using Results = PacBio::CLI_v2::Results;
using SetByMode = PacBio::CLI_v2::SetByMode;

// clang-format off

namespace CLI_v2_ResultsTests {

const Option Force
{
R"({
    "names" : ["f", "force"],
    "description" : "Overwrite things.",
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

const Option Zshort
{
R"({
    "names" : ["Z"],
    "description" : "Short name-only switch.",
    "type" : "boolean"
})"
};

const Option Yshort
{
R"({
    "names" : ["Y"],
    "description" : "Short name-only option.",
    "type" : "integer",
    "default" : 7
})"
};

const Option UnsignedTimeout
{
R"({
    "names" : ["u-timeout"],
    "description" : "Abort execution after <INT> milliseconds.",
    "type" : "unsigned integer",
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

const Option DoubleDelta
{
R"({
    "names" : ["double-delta"],
    "description" : "Some double delta for things.",
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

const Option Int8{
R"({
    "names" : ["int8"],
    "description" : "int8_t",
    "type" : "int",
    "default" : 0
})"
};

const Option UInt8{
R"({
    "names" : ["uint8"],
    "description" : "uint8_t",
    "type" : "unsigned int",
    "default" : 0
})"
};

const Option Int16{
R"({
    "names" : ["int16"],
    "description" : "int16_t",
    "type" : "int",
    "default" : 0
})"
};

const Option UInt16{
R"({
    "names" : ["uint16"],
    "description" : "uint16_t",
    "type" : "unsigned int",
    "default" : 0
})"
};

const Option Int32{
R"({
    "names" : ["int32"],
    "description" : "int32_t",
    "type" : "int",
    "default" : 0
})"
};

const Option UInt32{
R"({
    "names" : ["uint32"],
    "description" : "uint32_t",
    "type" : "unsigned int",
    "default" : 0
})"
};

const Option Int64{
R"({
    "names" : ["int64"],
    "description" : "int64_t",
    "type" : "int",
    "default" : 0
})"
};

const Option UInt64{
R"({
    "names" : ["uint64"],
    "description" : "uint64_t",
    "type" : "unsigned int",
    "default" : 0
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

}  // namespace CLI_v2_ResultsTests

TEST(CLI2_Result, can_construct_from_basic_types)
{
    {   // int
        const int input = 42;
        const Result result{input};
        const int resultValue = result;
        EXPECT_EQ(input, resultValue);
    }
    {   // unsigned int
        const unsigned int input = 42;
        const Result result{input};
        const unsigned int resultValue = result;
        EXPECT_EQ(input, resultValue);
    }
    {   // double
        const double input = 3.14;
        const Result result{input};
        const double resultValue = result;
        EXPECT_EQ(input, resultValue);
    }
    {   // bool
        const bool input = false;
        const Result result{input};
        const bool resultValue = result;
        EXPECT_EQ(input, resultValue);
    }
    {   // string
        const std::string input = "foo";
        const Result result{input};
        const std::string resultValue = result;
        EXPECT_EQ(input, resultValue);
    }
}

TEST(CLI2_Result, will_not_allow_invalid_conversion)
{
    const int input = 42;
    const Result result{input};

    EXPECT_THROW({const std::string resultValue = result;(void)resultValue;}, std::exception);
    EXPECT_THROW({const unsigned int resultValue = result;(void)resultValue;}, std::exception);
    EXPECT_THROW({const float resultValue = result;(void)resultValue;}, std::exception);
    EXPECT_THROW({const double resultValue = result;(void)resultValue;}, std::exception);
    EXPECT_THROW({const bool resultValue = result;(void)resultValue;}, std::exception);

    EXPECT_NO_THROW({const int resultValue = result;(void)resultValue;});
}

TEST(CLI2_Results, can_add_and_fetch_positional_args_via_index_number)
{
    const auto posArgs = PositionalArgumentTranslator::Translate(
    {
        CLI_v2_ResultsTests::Source,
        CLI_v2_ResultsTests::Dest

    });

    Results results;
    results.PositionalArguments(posArgs);
    results.AddPositionalArgument("in");
    results.AddPositionalArgument("out");

    const auto& posArgValues = results.PositionalArguments();
    ASSERT_EQ(2, posArgValues.size());
    EXPECT_EQ("in", posArgValues.at(0));
    EXPECT_EQ("out", posArgValues.at(1));
}

TEST(CLI2_Results, can_fetch_positional_args_via_pos_arg_object)
{
    struct Settings
    {
        explicit Settings(const Results& results)
            : source(results[CLI_v2_ResultsTests::Source])
            , dest(results[CLI_v2_ResultsTests::Dest])
        { }

        std::string source;
        std::string dest;
    };

    const auto posArgs = PositionalArgumentTranslator::Translate(
    {
        CLI_v2_ResultsTests::Source,
        CLI_v2_ResultsTests::Dest
    });

    Results results;
    results.PositionalArguments(posArgs);
    results.AddPositionalArgument("inFile.txt");
    results.AddPositionalArgument("outFile.txt");

    const Settings s{results};
    EXPECT_EQ("inFile.txt", s.source);
    EXPECT_EQ("outFile.txt", s.dest);
}

TEST(CLI2_Results, can_add_and_fetch_option_values)
{
    struct Settings
    {
        explicit Settings(const Results& results)
            : force(results[CLI_v2_ResultsTests::Force])
            , timeout(results[CLI_v2_ResultsTests::Timeout])
            , uTimeout(results[CLI_v2_ResultsTests::UnsignedTimeout])
            , delta(results[CLI_v2_ResultsTests::Delta])
            , doubleDelta(results[CLI_v2_ResultsTests::DoubleDelta])
            , ploidy(results[CLI_v2_ResultsTests::Ploidy])
            , int8(results[CLI_v2_ResultsTests::Int8])
            , uint8(results[CLI_v2_ResultsTests::UInt8])
            , int16(results[CLI_v2_ResultsTests::Int16])
            , uint16(results[CLI_v2_ResultsTests::UInt16])
            , int32(results[CLI_v2_ResultsTests::Int32])
            , uint32(results[CLI_v2_ResultsTests::UInt32])
            , int64(results[CLI_v2_ResultsTests::Int64])
            , uint64(results[CLI_v2_ResultsTests::UInt64])
        { }

        // "general" types
        bool force;
        int timeout;
        unsigned int uTimeout;
        float delta;
        double doubleDelta;
        std::string ploidy;

        // specific-width integer types
        int8_t int8;
        uint8_t uint8;
        int16_t int16;
        uint16_t uint16;
        int32_t int32;
        uint32_t uint32;
        int64_t int64;
        uint64_t uint64;
    };

    const bool force = true;
    const int timeout = 300;
    const unsigned int uTimeout = 300;
    const float delta = 2.77;
    const double doubleDelta = 35.6;
    const std::string ploidy{"dipoid"};
    const int8_t int8 = -1;
    const uint8_t uint8 = 1;
    const int16_t int16 = -1;
    const uint16_t uint16 = 1;
    const int32_t int32 = -1;
    const uint32_t uint32 = 1;
    const int64_t int64 = -1;
    const uint64_t uint64 = 1;

    Results results;
    results.AddObservedValue("force", force, SetByMode::USER);
    results.AddObservedValue("timeout", timeout, SetByMode::USER);
    results.AddObservedValue("u-timeout", uTimeout, SetByMode::USER);
    results.AddObservedValue("delta", delta, SetByMode::USER);
    results.AddObservedValue("double-delta", doubleDelta, SetByMode::USER);
    results.AddObservedValue("ploidy", ploidy, SetByMode::USER);
    results.AddObservedValue("int8", int8, SetByMode::USER);
    results.AddObservedValue("uint8", uint8, SetByMode::USER);
    results.AddObservedValue("int16", int16, SetByMode::USER);
    results.AddObservedValue("uint16", uint16, SetByMode::USER);
    results.AddObservedValue("int32", int32, SetByMode::USER);
    results.AddObservedValue("uint32", uint32, SetByMode::USER);
    results.AddObservedValue("int64", int64, SetByMode::USER);
    results.AddObservedValue("uint64", uint64, SetByMode::USER);

    const Settings s{results};
    EXPECT_TRUE(s.force);
    EXPECT_EQ(timeout, s.timeout);
    EXPECT_EQ(uTimeout, s.uTimeout);
    EXPECT_EQ(delta, s.delta);
    EXPECT_EQ(doubleDelta, s.doubleDelta);
    EXPECT_EQ(ploidy, s.ploidy);

    EXPECT_EQ(int8, s.int8);
    EXPECT_EQ(uint8, s.uint8);
    EXPECT_EQ(int16, s.int16);
    EXPECT_EQ(uint16, s.uint16);
    EXPECT_EQ(int32, s.int32);
    EXPECT_EQ(uint32, s.uint32);
    EXPECT_EQ(int64, s.int64);
    EXPECT_EQ(uint64, s.uint64);
}

TEST(CLI2_Results, can_add_and_fetch_options_and_pos_args)
{
    struct Settings
    {
        explicit Settings(const Results& results)
            : force(results[CLI_v2_ResultsTests::Force])
            , timeout(results[CLI_v2_ResultsTests::Timeout])
            , delta(results[CLI_v2_ResultsTests::Delta])
            , doubleDelta(results[CLI_v2_ResultsTests::DoubleDelta])
            , ploidy(results[CLI_v2_ResultsTests::Ploidy])
            , source(results[CLI_v2_ResultsTests::Source])
            , dest(results[CLI_v2_ResultsTests::Dest])
        { }

        bool force;
        int timeout;
        float delta;
        double doubleDelta;
        std::string ploidy;

        std::string source;
        std::string dest;
    };

    Results results;
    results.AddObservedValue("force", true, SetByMode::USER);
    results.AddObservedValue("timeout", 300, SetByMode::USER);
    results.AddObservedValue("delta", 2.77, SetByMode::USER);
    results.AddObservedValue("double-delta", 35.6, SetByMode::USER);
    results.AddObservedValue("ploidy", std::string{"diploid"}, SetByMode::USER);

    const auto posArgs = PositionalArgumentTranslator::Translate(
    {
        CLI_v2_ResultsTests::Source,
        CLI_v2_ResultsTests::Dest
    });
    results.PositionalArguments(posArgs);
    results.AddPositionalArgument("inFile.txt");
    results.AddPositionalArgument("outFile.txt");

    const Settings s{results};
    EXPECT_TRUE(s.force);
    EXPECT_EQ(300, s.timeout);
    EXPECT_EQ(2.77f, s.delta);
    EXPECT_EQ(35.6, s.doubleDelta);
    EXPECT_EQ("diploid", s.ploidy);
    EXPECT_EQ("inFile.txt", s.source);
    EXPECT_EQ("outFile.txt", s.dest);
}

TEST(CLI2_Results, can_provide_effective_command_line_from_default_parameters_only)
{
    Interface i{"frobber", "Frobb your files in a most delightful, nobbly way", "3.14"};
    i.AddOptions({
        CLI_v2_ResultsTests::Force,
        CLI_v2_ResultsTests::Timeout,
        CLI_v2_ResultsTests::Delta,
        CLI_v2_ResultsTests::Ploidy
    });
    i.AddPositionalArguments({
        CLI_v2_ResultsTests::Source,
        CLI_v2_ResultsTests::Dest
    });

    CommandLineParser parser{i};
    const std::vector<std::string> args{
        i.ApplicationName(),
        "requiredIn",
        "requiredOut"
    };
    const auto results = parser.Parse(args);
    const auto cmdLine = results.EffectiveCommandLine();

    EXPECT_NE(std::string::npos, cmdLine.find("--log-level=WARN"));
    EXPECT_NE(std::string::npos, cmdLine.find("--delta=0.01"));
    EXPECT_NE(std::string::npos, cmdLine.find("--ploidy=haploid"));
    EXPECT_NE(std::string::npos, cmdLine.find("--num-threads=0"));
    EXPECT_NE(std::string::npos, cmdLine.find("--timeout=5000"));
    EXPECT_NE(std::string::npos, cmdLine.find("requiredIn"));
    EXPECT_NE(std::string::npos, cmdLine.find("requiredOut"));
}

TEST(CLI2_Results, can_provide_effective_command_line_from_user_parameters_only)
{
    Interface i{"frobber", "Frobb your files in a most delightful, nobbly way", "3.14"};
    i.AddOptions({
        CLI_v2_ResultsTests::Force,
        CLI_v2_ResultsTests::Timeout,
        CLI_v2_ResultsTests::Delta,
        CLI_v2_ResultsTests::Ploidy
    });
    i.AddPositionalArguments({
        CLI_v2_ResultsTests::Source,
        CLI_v2_ResultsTests::Dest
    });

    CommandLineParser parser{i};
    const std::vector<std::string> args{
        i.ApplicationName(),
        "-f",
        "--delta=0.22",
        "--ploidy", "diploid",
        "--log-level", "DEBUG",
        "--log-file", "out.log",
        "-j", "10",
        "--timeout=99",
        "requiredIn",
        "requiredOut"
    };
    const auto results = parser.Parse(args);
    const auto cmdLine = results.EffectiveCommandLine();

    EXPECT_NE(std::string::npos, cmdLine.find("--delta=0.22"));
    EXPECT_NE(std::string::npos, cmdLine.find("--force"));
    EXPECT_NE(std::string::npos, cmdLine.find("--ploidy=diploid"));
    EXPECT_NE(std::string::npos, cmdLine.find("--num-threads=10"));
    EXPECT_NE(std::string::npos, cmdLine.find("--timeout=99"));
    EXPECT_NE(std::string::npos, cmdLine.find("--log-level=DEBUG"));
    EXPECT_NE(std::string::npos, cmdLine.find("--log-file=out.log"));
    EXPECT_NE(std::string::npos, cmdLine.find("requiredIn"));
    EXPECT_NE(std::string::npos, cmdLine.find("requiredOut"));
}

TEST(CLI2_Results, can_provide_effective_command_line_from_mixed_user_and_default_parameters)
{
    Interface i{"frobber", "Frobb your files in a most delightful, nobbly way", "3.14"};
    i.AddOptions({
        CLI_v2_ResultsTests::Force,
        CLI_v2_ResultsTests::Timeout,
        CLI_v2_ResultsTests::Delta,
        CLI_v2_ResultsTests::Ploidy,
        CLI_v2_ResultsTests::Zshort,
        CLI_v2_ResultsTests::Yshort
    });
    i.AddPositionalArguments({
        CLI_v2_ResultsTests::Source,
        CLI_v2_ResultsTests::Dest
    });

    CommandLineParser parser{i};
    const std::vector<std::string> args{
        i.ApplicationName(),
        "-f",
        "-j", "10",
        "--timeout=99",
        "-Z",
        "-Y", "12",
        "requiredIn",
        "requiredOut"
    };
    const auto results = parser.Parse(args);
    const auto cmdLine = results.EffectiveCommandLine();

    EXPECT_NE(std::string::npos, cmdLine.find("--log-level=WARN"));
    EXPECT_NE(std::string::npos, cmdLine.find("--delta=0.01"));
    EXPECT_NE(std::string::npos, cmdLine.find("--force"));
    EXPECT_NE(std::string::npos, cmdLine.find("--ploidy=haploid"));
    EXPECT_NE(std::string::npos, cmdLine.find("-Z"));
    EXPECT_NE(std::string::npos, cmdLine.find("--num-threads=10"));
    EXPECT_NE(std::string::npos, cmdLine.find("-Y=12"));
    EXPECT_NE(std::string::npos, cmdLine.find("--timeout=99"));
    EXPECT_NE(std::string::npos, cmdLine.find("requiredIn"));
    EXPECT_NE(std::string::npos, cmdLine.find("requiredOut"));
}

// clang-format on
