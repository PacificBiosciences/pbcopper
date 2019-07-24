#include <sstream>

#include <gtest/gtest.h>

#include <pbcopper/cli2/Option.h>
#include <pbcopper/cli2/PositionalArgument.h>
#include <pbcopper/cli2/Results.h>
#include <pbcopper/cli2/internal/OptionTranslator.h>
#include <pbcopper/cli2/internal/PositionalArgumentTranslator.h>

using Option = PacBio::CLI_v2::Option;
using OptionTranslator = PacBio::CLI_v2::internal::OptionTranslator;
using PositionalArgument = PacBio::CLI_v2::PositionalArgument;
using PositionalArgumentTranslator = PacBio::CLI_v2::internal::PositionalArgumentTranslator;
using Result = PacBio::CLI_v2::Result;
using Results = PacBio::CLI_v2::Results;
using SetByMode = PacBio::CLI_v2::SetByMode;

// clang-format off

namespace CLI_v2_ResultsTests {

static const Option Force
{
R"({
    "names" : ["f", "force"],
    "description" : "Overwrite things.",
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

static const Option DoubleDelta
{
R"({
    "names" : ["double-delta"],
    "description" : "Some double delta for things.",
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

} // CLI_v2_ResultsTests

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

    // This should not even compile.
    // const std::string resultValue = result;

    EXPECT_THROW({const unsigned int resultValue = result;(void)resultValue;}, boost::bad_get);
    EXPECT_THROW({const float resultValue = result;(void)resultValue;}, boost::bad_get);
    EXPECT_THROW({const double resultValue = result;(void)resultValue;}, boost::bad_get);
    EXPECT_THROW({const bool resultValue = result;(void)resultValue;}, boost::bad_get);
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
        Settings(const Results& results)
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
        Settings(const Results& results)
            : force(results[CLI_v2_ResultsTests::Force])
            , timeout(results[CLI_v2_ResultsTests::Timeout])
            , delta(results[CLI_v2_ResultsTests::Delta])
            , doubleDelta(results[CLI_v2_ResultsTests::DoubleDelta])
            , ploidy(results[CLI_v2_ResultsTests::Ploidy])
        { }

        bool force;
        int timeout;
        float delta;
        double doubleDelta;
        std::string ploidy;
    };

    Results results;
    results.AddObservedValue("force", true, SetByMode::USER);
    results.AddObservedValue("timeout", 300, SetByMode::USER);
    results.AddObservedValue("delta", 2.77, SetByMode::USER);
    results.AddObservedValue("double-delta", 35.6, SetByMode::USER);
    results.AddObservedValue("ploidy", std::string{"diploid"}, SetByMode::USER);

    const Settings s{results};
    EXPECT_TRUE(s.force);
    EXPECT_EQ(300, s.timeout);
    EXPECT_EQ(2.77f, s.delta);
    EXPECT_EQ(35.6, s.doubleDelta);
    EXPECT_EQ("diploid", s.ploidy);
}

TEST(CLI2_Results, can_add_and_fetch_options_and_pos_args)
{
    struct Settings
    {
        Settings(const Results& results)
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

// clang-format on
