#include <pbcopper/cli/HelpPrinter.h>

#include <gtest/gtest.h>
#include <pbcopper/cli/Interface.h>
#include <vector>
using namespace PacBio;
using namespace PacBio::CLI;
using namespace std;

TEST(CLI_HelpPrinter, prints_expected_help_output)
{
    // clang-format off
    Interface i {
        "frobber",
        "Frobb your files in a most delightful, nobbly way",
        "3.14"
    };

    // common built-ins - ON by default ?
    i.AddHelpOption();
    i.AddVerboseOption();
    i.AddVersionOption();

    i.AddOptions({
        {"progress",   {"p"},               "Show progress during copy."},
        {"force",      {"f", "force"},      "Overwrite things." },
        {"target_dir", {"t", "target-dir"}, "Copy all source files into <DIR>.",         Option::StringType("my/default/dir")},
        {"timeout",    {"timeout"},         "Abort execution after <INT> milliseconds.", Option::IntType(5000)},
        {"modelPath",  {"M", "modelPath"},  "Path to a model file.",                     Option::StringType("")},  // empty default string should be omitted from help
        {"min_prevalence", {"minPrev"},     "Minimum prevalence to require things.",     Option::FloatType(0.09)},
        {"min_split_frac", {"minSplit"},    "Minimum fraction of reads.",                Option::FloatType(0.1)},
        {"secret_mode",    {"secret"},      "Secret mode, hidden from help.",            Option::BoolType(false), { }, OptionFlags::HIDE_FROM_HELP}
    });
    i.AddPositionalArguments({
        {"source", "Source file to copy.", {}},
        {"dest",   "Destination directory.", {}},
        {"extras", "Extra stuff to pass in here, optionally.", "[extras...]"}
    });

    const string expectedText{
        "Usage: frobber [options] source dest [extras...]\n"
        "Frobb your files in a most delightful, nobbly way\n"
        "\n"
        "Options:\n"
        "  -h,--help        Output this help.\n"
        "  -v,--verbose     Use verbose output.\n"
        "  --version        Output version info.\n"
        "  -p               Show progress during copy.\n"
        "  -f,--force       Overwrite things.\n"
        "  -t,--target-dir  Copy all source files into <DIR>. [\"my/default/dir\"]\n"
        "  --timeout        Abort execution after <INT> milliseconds. [5000]\n"
        "  -M,--modelPath   Path to a model file.\n"
        "  --minPrev        Minimum prevalence to require things. [0.09]\n"
        "  --minSplit       Minimum fraction of reads. [0.1]\n"
        "\n"
        "Arguments:\n"
        "  source           Source file to copy.\n"
        "  dest             Destination directory.\n"
        "  extras           Extra stuff to pass in here, optionally.\n"
        "\n"
    };
    // clang-format on

    stringstream s;
    HelpPrinter::Print(i, s);
    EXPECT_EQ(expectedText, s.str());
}

TEST(CLI_HelpPrinter, prints_expected_option_groups)
{
    // clang-format off
    Interface i {
        "frobber",
        "Frobb your files in a most delightful, nobbly way",
        "3.14"
    };

    // common built-ins - ON by default ?
    i.AddHelpOption();
    i.AddVerboseOption();
    i.AddVersionOption();

    i.AddGroup("Foo Options",
    {
       {"progress", {"p"},          "Show progress during copy."},
       {"force",    {"f", "force"}, "Overwrite things." }
    });
    i.AddGroup("Bar Options",
    {
       {"target_dir", {"t", "target-dir"}, "Copy all source files into <DIR>.",         Option::StringType("my/default/dir")},
       {"timeout",    {"timeout"},         "Abort execution after <INT> milliseconds.", Option::IntType(5000)},
       {"modelPath",  {"M", "modelPath"},  "Path to a model file.",                     Option::StringType("")}
    });

    i.AddPositionalArguments({
        {"source", "Source file to copy.", {}},
        {"dest",   "Destination directory.", {}},
        {"extras", "Extra stuff to pass in here, optionally.", "[extras...]"}
    });

    const string expectedText{
        "Usage: frobber [options] source dest [extras...]\n"
        "Frobb your files in a most delightful, nobbly way\n"
        "\n"
        "Foo Options:\n"
        "  -p               Show progress during copy.\n"
        "  -f,--force       Overwrite things.\n"
        "\n"
        "Bar Options:\n"
        "  -t,--target-dir  Copy all source files into <DIR>. [\"my/default/dir\"]\n"
        "  --timeout        Abort execution after <INT> milliseconds. [5000]\n"
        "  -M,--modelPath   Path to a model file.\n"
        "\n"
        "Options:\n"
        "  -h,--help        Output this help.\n"
        "  -v,--verbose     Use verbose output.\n"
        "  --version        Output version info.\n"
        "\n"
        "Arguments:\n"
        "  source           Source file to copy.\n"
        "  dest             Destination directory.\n"
        "  extras           Extra stuff to pass in here, optionally.\n"
        "\n"
    };
    // clang-format on

    stringstream s;
    HelpPrinter::Print(i, s);
    EXPECT_EQ(expectedText, s.str());
}
