#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <pbcopper/cli2/CLI.h>
#include <pbcopper/cli2/Option.h>
#include <pbcopper/cli2/PositionalArgument.h>
#include <pbcopper/cli2/Results.h>
#include <pbcopper/utility/Unused.h>

#include "../../include/OStreamRedirector.h"
#include "PbcopperTestData.h"

// clang-format off

using namespace PacBio;
using namespace PacBio::CLI_v2;

namespace CLI_v2_CLITests {

const Option RefineTestOption
{
R"({
    "names" : ["test"],
    "description" : "Should see this entry",
    "type" : "boolean"
})"
};

const Option RefineCountOption
{
R"({
    "names" : ["count"],
    "description" : "Should see this entry too",
    "type" : "integer",
    "default" : 5
})"
};

Interface MakeRefineInterface()
{
    Interface refine{"refine", "Remove concatemers and optionally poly-A tails (FL to FLNC)"};
    refine.AddOptions({
        RefineTestOption,
        RefineCountOption});
    refine.Example("isoseq3 refine movie.consensusreadset.xml primers.barcodeset.xml movie.flnc.bam");
    return refine;
}

int RefineRunner(const PacBio::CLI_v2::Results& results)
{
    // "$ isoseq3 refine --test --count 10"

    const bool test = results[RefineTestOption];
    const int count = results[RefineCountOption];
    EXPECT_TRUE(test);
    EXPECT_EQ(10, count);

    return EXIT_SUCCESS;
}

Interface MakeClusterInterface()
{
    Interface cluster{"cluster", "Cluster FLNC reads and generate unpolished transcripts (FLNC to UNPOLISHED"};
    cluster.Example("isoseq3 cluster movie.flnc.bam unpolished.bam");
    return cluster;
}

int ClusterRunner(const PacBio::CLI_v2::Results&)
{
    return EXIT_SUCCESS;
}

Interface MakePolishInterface()
{
    Interface polish{"polish", "Polish transcripts using subreads (UNPOLISHED to POLISHED)"};
    polish.Example("isoseq3 polish unpolished.bam movie.subreadset.xml polished.bam");
    return polish;
}

int PolishRunner(const PacBio::CLI_v2::Results&)
{
    return EXIT_SUCCESS;
}

Interface MakeSummarizeInterface()
{
    Interface summarize{"summarize", "Create barcode overview from transcripts (POLISHED to CSV)"};
    summarize.Example("isoseq3 summarize polished.bam summary.csv");
    return summarize;
}

int SummarizeRunner(const PacBio::CLI_v2::Results&)
{
    std::cerr << "SummarizeRunner\n";
    return EXIT_SUCCESS;
}

} // namespace CLI_v2_CLITests

TEST(CLI2_CLI, can_print_version)
{
    const std::string expectedText{"frobber v3.1\n"};
    const std::vector<std::string> args {
        "frobber", "--version"
    };
    auto runner = [](const Results&)
    {
        return EXIT_SUCCESS;
    };

    std::ostringstream s;
    tests::CoutRedirect redirect(s.rdbuf());
    UNUSED(redirect);

    const Interface i{"frobber", "Frob all the things", "v3.1"};
    const int result = CLI_v2::Run(args, i, runner);

    EXPECT_EQ(EXIT_SUCCESS, result);
    EXPECT_EQ(expectedText, s.str());
}

TEST(CLI2_CLI, can_print_help)
{
    const std::string expectedText{
"frobber - Frob all the things.\n"
"\n"
"Usage:\n"
"  frobber [options]\n"
"\n"
"Options:\n"
"  -h,--help          Show this help and exit.\n"
"  --log-level  STR   Set log level. Valid choices: (TRACE, DEBUG, INFO, WARN, FATAL). [WARN]\n"
"  --log-file   FILE  Log to a file, instead of stderr.\n"
"  --version          Show application version and exit.\n"
"\n"
};

    const std::vector<std::string> args {
        "frobber", "-h"
    };
    auto runner = [](const Results&)
    {
        return EXIT_SUCCESS;
    };

    std::ostringstream s;
    tests::CoutRedirect redirect(s.rdbuf());
    UNUSED(redirect);

    const Interface i{"frobber", "Frob all the things.", "v3.1"};
    const int result = CLI_v2::Run(args, i, runner);

    EXPECT_EQ(EXIT_SUCCESS, result);
    EXPECT_EQ(expectedText, s.str());
}

TEST(CLI2_CLI, can_print_top_help_for_multitoolinterface)
{
        const std::string expectedText{R"(isoseq3 - De Novo Transcript Reconstruction

Usage:
  isoseq3 <tool>

Tools:
  refine     Remove concatemers and optionally poly-A tails (FL to FLNC)
  cluster    Cluster FLNC reads and generate unpolished transcripts (FLNC to UNPOLISHED
  polish     Polish transcripts using subreads (UNPOLISHED to POLISHED)
  summarize  Create barcode overview from transcripts (POLISHED to CSV)

Examples:
  isoseq3 refine movie.consensusreadset.xml primers.barcodeset.xml movie.flnc.bam
  isoseq3 cluster movie.flnc.bam unpolished.bam
  isoseq3 polish unpolished.bam movie.subreadset.xml polished.bam
  isoseq3 summarize polished.bam summary.csv

Typical workflow:
  1. Generate consensus sequences from your raw subread data
     $ ccs movie.subreads.bam movie.ccs.bam --noPolish --minPasses 1

  2. Generate full-length reads by primer removal and demultiplexing
     $ cat primers.fasta
       >primer_5p
       AAGCAGTGGTATCAACGCAGAGTACATGGGG
       >primer_3p
       AAGCAGTGGTATCAACGCAGAGTAC
     $ lima movie.ccs.bam primers.fasta movie.fl.bam --isoseq --no-pbi

  3. Remove noise from FL reads
     $ isoseq3 refine movie.fl.P5--P3.bam primers.fasta movie.flnc.bam

  4. Cluster consensus sequences to generate unpolished transcripts
     $ isoseq3 cluster movie.flnc.bam unpolished.bam --verbose

  5. Polish transcripts using subreads
     $ isoseq3 polish unpolished.bam movie.subreads.bam polished.bam
)"};

    MultiToolInterface i{"isoseq3", "De Novo Transcript Reconstruction", "3.1.2"};
    i.AddTools({{"refine",    CLI_v2_CLITests::MakeRefineInterface(),    &CLI_v2_CLITests::RefineRunner},
                {"cluster",   CLI_v2_CLITests::MakeClusterInterface(),   &CLI_v2_CLITests::ClusterRunner},
                {"polish",    CLI_v2_CLITests::MakePolishInterface(),    &CLI_v2_CLITests::PolishRunner},
                {"summarize", CLI_v2_CLITests::MakeSummarizeInterface(), &CLI_v2_CLITests::SummarizeRunner}});

    i.HelpFooter(R"(Typical workflow:
  1. Generate consensus sequences from your raw subread data
     $ ccs movie.subreads.bam movie.ccs.bam --noPolish --minPasses 1

  2. Generate full-length reads by primer removal and demultiplexing
     $ cat primers.fasta
       >primer_5p
       AAGCAGTGGTATCAACGCAGAGTACATGGGG
       >primer_3p
       AAGCAGTGGTATCAACGCAGAGTAC
     $ lima movie.ccs.bam primers.fasta movie.fl.bam --isoseq --no-pbi

  3. Remove noise from FL reads
     $ isoseq3 refine movie.fl.P5--P3.bam primers.fasta movie.flnc.bam

  4. Cluster consensus sequences to generate unpolished transcripts
     $ isoseq3 cluster movie.flnc.bam unpolished.bam --verbose

  5. Polish transcripts using subreads
     $ isoseq3 polish unpolished.bam movie.subreads.bam polished.bam)");

    const std::vector<std::string> args {
        "isoseq3", "-h"
    };

    std::ostringstream s;
    tests::CoutRedirect redirect(s.rdbuf());
    UNUSED(redirect);

    const int result = CLI_v2::Run(args, i);
    EXPECT_EQ(EXIT_SUCCESS, result);

    EXPECT_EQ(s.str(), expectedText);
}

TEST(CLI2_CLI, can_print_subtool_help_for_multitoolinterface)
{
    const std::string expectedText{R"(refine - Remove concatemers and optionally poly-A tails (FL to FLNC)

Usage:
  refine [options]

  --test             Should see this entry
  --count      INT   Should see this entry too [5]

  -h,--help          Show this help and exit.
  --log-level  STR   Set log level. Valid choices: (TRACE, DEBUG, INFO, WARN, FATAL). [WARN]
  --log-file   FILE  Log to a file, instead of stderr.
  --version          Show application version and exit.

)"};


    MultiToolInterface i{"isoseq3", "De Novo Transcript Reconstruction", "3.1.2"};
    i.AddTools({{"refine",    CLI_v2_CLITests::MakeRefineInterface(),    &CLI_v2_CLITests::RefineRunner},
                {"cluster",   CLI_v2_CLITests::MakeClusterInterface(),   &CLI_v2_CLITests::ClusterRunner},
                {"polish",    CLI_v2_CLITests::MakePolishInterface(),    &CLI_v2_CLITests::PolishRunner},
                {"summarize", CLI_v2_CLITests::MakeSummarizeInterface(), &CLI_v2_CLITests::SummarizeRunner}});

    const std::vector<std::string> args {
        "isoseq3", "refine", "-h"
    };

    std::ostringstream s;
    tests::CoutRedirect redirect(s.rdbuf());
    UNUSED(redirect);

    const int result = CLI_v2::Run(args, i);
    EXPECT_EQ(EXIT_SUCCESS, result);
    EXPECT_EQ(s.str(), expectedText);
}

TEST(CLI2_CLI, can_run_from_command_line_args)
{
    const Option MaxNumLines
    {
    R"({
        "names" : ["n"],
        "description" : "Max Number of lines to Copy",
        "type" : "int",
        "default" : 10,
        "tool_contract.option" : {
            "title" : "Maximum Line Count",
            "id" : "max_nlines"
        }
    })"
    };

    const std::vector<std::string> args {
        "frobber", "-n", "27"
    };
    auto runner = [&MaxNumLines](const Results& results)
    {
        const int expectedNumLines = 27;
        const int maxNumLines = results[MaxNumLines];

        EXPECT_EQ(expectedNumLines, maxNumLines);
        return EXIT_SUCCESS;
    };

    Interface i{"frobber", "Frob all the things.", "v3.1"};
    i.AddOption(MaxNumLines);

    std::ostringstream s;
    tests::CoutRedirect redirect(s.rdbuf());
    UNUSED(redirect);

    const int result = CLI_v2::Run(args, i, runner);
    EXPECT_EQ(EXIT_SUCCESS, result);
}

TEST(CLI2_CLI, can_run_multitoolinterface_subtool_from_command_line_args)
{
    MultiToolInterface i{"isoseq3", "De Novo Transcript Reconstruction", "3.1.2"};
    i.AddTools({{"refine",    CLI_v2_CLITests::MakeRefineInterface(),    &CLI_v2_CLITests::RefineRunner},
                {"cluster",   CLI_v2_CLITests::MakeClusterInterface(),   &CLI_v2_CLITests::ClusterRunner},
                {"polish",    CLI_v2_CLITests::MakePolishInterface(),    &CLI_v2_CLITests::PolishRunner},
                {"summarize", CLI_v2_CLITests::MakeSummarizeInterface(), &CLI_v2_CLITests::SummarizeRunner}});

    const std::vector<std::string> args {
        "isoseq3", "refine", "--test", "--count", "10"
    };

    std::ostringstream s;
    tests::CoutRedirect redirect(s.rdbuf());
    UNUSED(redirect);

    const int result = CLI_v2::Run(args, i);
    EXPECT_EQ(EXIT_SUCCESS, result);
}

TEST(CLI2_CLI, can_fetch_default_log_level_from_results)
{
    const Option MaxNumLines
    {
    R"({
        "names" : ["n"],
        "description" : "Max Number of lines to Copy",
        "type" : "int",
        "default" : 10,
        "tool_contract.option" : {
            "title" : "Maximum Line Count",
            "id" : "max_nlines"
        }
    })"
    };

    const std::vector<std::string> args {
        "frobber", "-n", "27"
    };

    Interface i{"frobber", "Frob all the things.", "v3.1"};
    i.AddOption(MaxNumLines);
    const auto defaultLogLevel = i.DefaultLogLevel();

    std::ostringstream s;
    tests::CoutRedirect redirect(s.rdbuf());
    UNUSED(redirect);

    auto runner = [&MaxNumLines, &defaultLogLevel](const Results& results)
    {
        const int expectedNumLines = 27;
        const int maxNumLines = results[MaxNumLines];

        EXPECT_EQ(expectedNumLines, maxNumLines);
        EXPECT_EQ(defaultLogLevel, results.LogLevel());
        return EXIT_SUCCESS;
    };

    const int result = CLI_v2::Run(args, i, runner);
    EXPECT_EQ(EXIT_SUCCESS, result);
}

TEST(CLI2_CLI, can_fetch_overriden_default_log_level_from_results)
{
    const Option MaxNumLines
    {
    R"({
        "names" : ["n"],
        "description" : "Max Number of lines to Copy",
        "type" : "int",
        "default" : 10,
        "tool_contract.option" : {
            "title" : "Maximum Line Count",
            "id" : "max_nlines"
        }
    })"
    };

    const std::vector<std::string> args {
        "frobber", "-n", "27"
    };

    Interface i{"frobber", "Frob all the things.", "v3.1"};
    i.AddOption(MaxNumLines);
    const auto defaultLogLevel = PacBio::Logging::LogLevel::DEBUG;
    i.DefaultLogLevel(defaultLogLevel);

    std::ostringstream s;
    tests::CoutRedirect redirect(s.rdbuf());
    UNUSED(redirect);

    auto runner = [&MaxNumLines, &defaultLogLevel](const Results& results)
    {
        const int expectedNumLines = 27;
        const int maxNumLines = results[MaxNumLines];

        EXPECT_EQ(expectedNumLines, maxNumLines);
        EXPECT_EQ(defaultLogLevel, results.LogLevel());
        return EXIT_SUCCESS;
    };

    const int result = CLI_v2::Run(args, i, runner);
    EXPECT_EQ(EXIT_SUCCESS, result);
}

TEST(CLI2_CLI, can_fetch_log_level_from_results)
{
    const Option MaxNumLines
    {
    R"({
        "names" : ["n"],
        "description" : "Max Number of lines to Copy",
        "type" : "int",
        "default" : 10,
        "tool_contract.option" : {
            "title" : "Maximum Line Count",
            "id" : "max_nlines"
        }
    })"
    };

    const std::vector<std::string> args {
        "frobber", "-n", "27", "--log-level", "DEBUG"
    };
    auto runner = [&MaxNumLines](const Results& results)
    {
        const int expectedNumLines = 27;
        const int maxNumLines = results[MaxNumLines];

        EXPECT_EQ(expectedNumLines, maxNumLines);
        EXPECT_EQ(PacBio::Logging::LogLevel::DEBUG, results.LogLevel());
        return EXIT_SUCCESS;
    };

    Interface i{"frobber", "Frob all the things.", "v3.1"};
    i.AddOption(MaxNumLines);

    std::ostringstream s;
    tests::CoutRedirect redirect(s.rdbuf());
    UNUSED(redirect);

    const int result = CLI_v2::Run(args, i, runner);
    EXPECT_EQ(EXIT_SUCCESS, result);
}

// clang-format on
