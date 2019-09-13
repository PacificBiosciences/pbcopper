#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <pbcopper/cli2/Interface.h>
#include <pbcopper/cli2/MultiToolInterface.h>
#include <pbcopper/cli2/internal/HelpMetrics.h>
#include <pbcopper/cli2/internal/InterfaceHelpPrinter.h>
#include <pbcopper/cli2/internal/MultiToolInterfaceHelpPrinter.h>

using HelpMetrics = PacBio::CLI_v2::internal::HelpMetrics;
using Interface = PacBio::CLI_v2::Interface;
using InterfaceHelpPrinter = PacBio::CLI_v2::internal::InterfaceHelpPrinter;
using MultiToolInterface = PacBio::CLI_v2::MultiToolInterface;
using MultiToolInterfaceHelpPrinter = PacBio::CLI_v2::internal::MultiToolInterfaceHelpPrinter;

// clang-format off

namespace CLI_v2_MultiToolInterfaceHelpPrinterTests {

Interface MakeRefineInterface()
{
    Interface refine{"refine", "Remove concatemers and optionally poly-A tails (FL to FLNC)"};
    refine.Example("isoseq3 refine movie.consensusreadset.xml primers.barcodeset.xml movie.flnc.bam");
    return refine;
}

int RefineRunner(const PacBio::CLI_v2::Results&)
{
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

} // namespace CLI_v2_MultiToolInterfaceHelpPrinterTests

TEST(CLI2_MultiToolInterfaceHelpPrinter, can_print_top_level_help)
{
    using namespace CLI_v2_MultiToolInterfaceHelpPrinterTests;
    HelpMetrics::TestingFixedWidth = 80;

    const std::string expectedText{R"(isoseq3 - De Novo Transcript Reconstruction

Usage:
  isoseq3 <tool>

  -h,--help    Show this help and exit.
  --version    Show application version and exit.

Tools:
  refine7890123 Remove concatemers and optionally poly-A tails (FL to FLNC)
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
    i.AddTools({{"refine7890123",    MakeRefineInterface(),    &RefineRunner},
                {"cluster",   MakeClusterInterface(),   &ClusterRunner},
                {"polish",    MakePolishInterface(),    &PolishRunner},
                {"summarize", MakeSummarizeInterface(), &SummarizeRunner}});

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

    MultiToolInterfaceHelpPrinter help{i};
    std::ostringstream out;
    help.Print(out);
    EXPECT_EQ(expectedText, out.str());

    out.str("");
    out << help;
    EXPECT_EQ(expectedText, out.str());
}

// clanf-format on
