
#include <pbcopper/cli/Results.h>
#include <gtest/gtest.h>
using namespace PacBio;
using namespace PacBio::CLI;
using namespace std;

namespace PacBio {
namespace CLI {
namespace tests {

static PacBio::CLI::Interface makeInterface(void)
{
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
        {"progress",   "p",                  "Show progress during copy."},
        {"force",      {"f", "force"},       "Overwrite things." },
        {"target_dir", {"t", "target-dir"},  "Copy all source files into <DIR>.", Option::StringType("my/default/dir")},
        {"timeout",    "timeout",            "Abort execution after <INT> milliseconds.", Option::IntType(5000)}
    });
    i.AddPositionalArguments({
        {"source", "Source file to copy."},
        {"dest",   "Destination directory."},
        {"extras", "Extra stuff to pass in here, optionally.", "[extras...]"}
    });

    return i;
}

} // namespace tests
} // namespace CLI
} // namespace PacBio

TEST(CLI_Results, option_default_values_respected)
{
    Results r{ tests::makeInterface() };
    EXPECT_FALSE(r["force"]);
    EXPECT_FALSE(r["timeout"].empty());     // has a default
    EXPECT_TRUE(r.PositionalArguments().empty());
}

TEST(CLI_Results, add_observed_option_value)
{
    Results r{ tests::makeInterface() };
    r.RegisterOptionValue("timeout", "42");
    EXPECT_EQ(string("42"), r["timeout"]);
}

TEST(CLI_Results, adding_positional_args)
{
    Results r{ tests::makeInterface() };
    r.RegisterPositionalArg("source_file")
     .RegisterPositionalArg("dest_file");

    // lookup by index
    const auto& resultPositionalArgs = r.PositionalArguments();
    EXPECT_EQ(2, resultPositionalArgs.size());
    EXPECT_EQ(string("source_file"),   resultPositionalArgs.at(0));
    EXPECT_EQ(string("dest_file"), resultPositionalArgs.at(1));

//    // lookup by name (order-independent at this point)
//    EXPECT_EQ(string("dest_file"),   r.PositionalArgument("dest"));
//    EXPECT_EQ(string("source_file"), r.PositionalArgument("source"));
}

