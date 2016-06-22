
#include <pbcopper/cli/Interface.h>
#include <gtest/gtest.h>
using namespace PacBio;
using namespace PacBio::CLI;
using namespace std;

namespace PacBio {
namespace CLI {
namespace tests {

static inline string AppName(void)
{
    static const string appName = "frobber";
    return appName;
}

static inline string AppDescription(void)
{
    static const string appDesc = "does the frobbing";
    return appDesc;
}

static inline string AppVersion(void)
{
    static const string appVersion = "3.14";
    return appVersion;
}

static bool HasOption(const Interface& i, const string& name)
{
    const auto options = i.RegisteredOptions();
    for (auto&& option : options) {
        const auto aliases = option.Names();
        for (auto&& alias : aliases) {
            if (alias == name)
                return true;
        }
    }
    return false;
}

} // namespace tests
} // namespace CLI
} // namespace PacBio

TEST(CLI_Interface, can_be_constructed_from_name_only)
{
    PacBio::CLI::Interface cl{ tests::AppName() };
    EXPECT_EQ(tests::AppName(), cl.ApplicationName());
    EXPECT_TRUE(cl.ApplicationDescription().empty());
    EXPECT_TRUE(cl.ApplicationVersion().empty());
}

TEST(CLI_Interface, can_be_constructed_from_name_and_description)
{
    PacBio::CLI::Interface cl{ tests::AppName(), tests::AppDescription() };
    EXPECT_EQ(tests::AppName(),        cl.ApplicationName());
    EXPECT_EQ(tests::AppDescription(), cl.ApplicationDescription());
    EXPECT_TRUE(cl.ApplicationVersion().empty());
}

TEST(CLI_Interface, can_be_constructed_from_name_description_and_version)
{
    PacBio::CLI::Interface cl{ tests::AppName(), tests::AppDescription(), tests::AppVersion() };
    EXPECT_EQ(tests::AppName(),        cl.ApplicationName());
    EXPECT_EQ(tests::AppDescription(), cl.ApplicationDescription());
    EXPECT_EQ(tests::AppVersion(),     cl.ApplicationVersion());
}

TEST(CLI_Interface, construction_with_empty_name_throws)
{
    EXPECT_THROW(PacBio::CLI::Interface{""}, std::runtime_error);
}

TEST(CLI_Interface, add_single_option_with_a_single_alias)
{
    PacBio::CLI::Interface cl{ tests::AppName() };
    cl.AddOption({"opt_id","o", "write output"});//, "outFile", "default"});

    const auto registered = cl.RegisteredOptions();
    EXPECT_EQ(1, registered.size());
    EXPECT_EQ(vector<string>({"o"}), registered.at(0).Names());
}

TEST(CLI_Interface, add_single_option_with_multiple_aliases)
{
    PacBio::CLI::Interface cl{ tests::AppName() };
    cl.AddOption({"opt_id", {"o", "output"}, "write output"});//, "outFile", "default"});

    const auto registered = cl.RegisteredOptions();
    EXPECT_EQ(1, registered.size());
    EXPECT_EQ(vector<string>({"o", "output"}), registered.at(0).Names());
}

TEST(CLI_Interface, add_multiple_options_one_at_a_time)
{
    PacBio::CLI::Interface cl{ tests::AppName() };
    cl.AddOption({"opt_id", {"o", "output"}, "write output"});//, "outFile", "defaultOut"});
    cl.AddOption({"opt_id2", {"i", "input"}, "write input"});//, "inFile", "defaultIn"});

    const auto registered = cl.RegisteredOptions();
    EXPECT_EQ(2, registered.size());
    EXPECT_EQ(vector<string>({"o", "output"}), registered.at(0).Names());
    EXPECT_EQ(vector<string>({"i", "input"}), registered.at(1).Names());
}

TEST(CLI_Interface, add_multple_options_as_batch)
{
    PacBio::CLI::Interface cl{ tests::AppName() };
    cl.AddOptions({
        {"opt_id", {"o", "output"}, "write output"},//, "outFile", "defaultOut"},
        {"opt_id2", {"i", "input"}, "write input"}  //, "inFile", "defaultIn"}
    });

    const auto registered = cl.RegisteredOptions();
    EXPECT_EQ(2, registered.size());
    EXPECT_EQ(vector<string>({"o", "output"}), registered.at(0).Names());
    EXPECT_EQ(vector<string>({"i", "input"}), registered.at(1).Names());
}

TEST(CLI_Interface, add_single_positional_arg)
{
    PacBio::CLI::Interface cl{ tests::AppName() };
    cl.AddPositionalArgument({"source", "Source file to copy."});

    const auto registered = cl.RegisteredPositionalArgs();
    EXPECT_EQ(1, registered.size());
    EXPECT_EQ(string{"source"}, registered.at(0).name_);
}

TEST(CLI_Interface, add_multiple_positional_args_one_at_a_time)
{
    PacBio::CLI::Interface cl{ tests::AppName() };
    cl.AddPositionalArgument({"source", "Source file to copy."});
    cl.AddPositionalArgument({"dest", "Target destination"});

    const auto registered = cl.RegisteredPositionalArgs();
    EXPECT_EQ(2, registered.size());
    EXPECT_EQ(string{"source"}, registered.at(0).name_);
    EXPECT_EQ(string{"dest"}, registered.at(1).name_);
}

TEST(CLI_Interface, add_multiple_positional_args_as_batch)
{
    PacBio::CLI::Interface cl{ tests::AppName() };
    cl.AddPositionalArguments({
        {"source", "Source file to copy."},
        {"dest", "Target destination"}
    });

    const auto registered = cl.RegisteredPositionalArgs();
    EXPECT_EQ(2, registered.size());
    EXPECT_EQ(string{"source"}, registered.at(0).name_);
    EXPECT_EQ(string{"dest"}, registered.at(1).name_);
}

TEST(CLI_Interface, does_not_have_help_option_by_default)
{
    PacBio::CLI::Interface cl{ tests::AppName() };
    EXPECT_FALSE(tests::HasOption(cl, "help"));
}

TEST(CLI_Interface, has_help_option_if_requested)
{
    PacBio::CLI::Interface cl{ tests::AppName() };
    cl.AddHelpOption();
    EXPECT_TRUE(tests::HasOption(cl, "help"));
}

TEST(CLI_Interface, does_not_have_verbose_option_by_default)
{
    PacBio::CLI::Interface cl{ tests::AppName() };
    EXPECT_FALSE(tests::HasOption(cl, "verbose"));
}

TEST(CLI_Interface, has_verbose_option_if_requested)
{
    PacBio::CLI::Interface cl{ tests::AppName() };
    cl.AddVerboseOption();
    EXPECT_TRUE(tests::HasOption(cl, "verbose"));
}

TEST(CLI_Interface, does_not_have_version_option_by_default)
{
    PacBio::CLI::Interface cl{ tests::AppName() };
    EXPECT_FALSE(tests::HasOption(cl, "version"));
}

TEST(CLI_Interface, has_version_option_if_requested)
{
    PacBio::CLI::Interface cl{ tests::AppName() };
    cl.AddVersionOption();
    EXPECT_TRUE(tests::HasOption(cl, "version"));
}
