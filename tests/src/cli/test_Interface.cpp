
#include <gtest/gtest.h>
#include <pbcopper/cli/Interface.h>
using namespace PacBio;
using namespace PacBio::CLI;

namespace InterfaceTests {

static inline std::string AppName(void)
{
    static const std::string appName{"frobber"};
    return appName;
}

static inline std::string AppDescription(void)
{
    static const std::string appDesc{"does the frobbing"};
    return appDesc;
}

static inline std::string AppVersion(void)
{
    static const std::string appVersion{"3.14"};
    return appVersion;
}

static bool HasOption(const Interface& i, const std::string& name)
{
    const auto options = i.RegisteredOptions();
    for (auto&& option : options) {
        const auto aliases = option.Names();
        for (auto&& alias : aliases) {
            if (alias == name) return true;
        }
    }
    return false;
}

}  // namespace InterfaceTests

TEST(CLI_Interface, can_be_constructed_from_name_only)
{
    PacBio::CLI::Interface cl{InterfaceTests::AppName()};
    EXPECT_EQ(InterfaceTests::AppName(), cl.ApplicationName());
    EXPECT_TRUE(cl.ApplicationDescription().empty());
    EXPECT_TRUE(cl.ApplicationVersion().empty());
}

TEST(CLI_Interface, can_be_constructed_from_name_and_description)
{
    PacBio::CLI::Interface cl{InterfaceTests::AppName(), InterfaceTests::AppDescription()};
    EXPECT_EQ(InterfaceTests::AppName(), cl.ApplicationName());
    EXPECT_EQ(InterfaceTests::AppDescription(), cl.ApplicationDescription());
    EXPECT_TRUE(cl.ApplicationVersion().empty());
}

TEST(CLI_Interface, can_be_constructed_from_name_description_and_version)
{
    PacBio::CLI::Interface cl{InterfaceTests::AppName(), InterfaceTests::AppDescription(),
                              InterfaceTests::AppVersion()};
    EXPECT_EQ(InterfaceTests::AppName(), cl.ApplicationName());
    EXPECT_EQ(InterfaceTests::AppDescription(), cl.ApplicationDescription());
    EXPECT_EQ(InterfaceTests::AppVersion(), cl.ApplicationVersion());
}

TEST(CLI_Interface, construction_with_empty_name_throws)
{
    EXPECT_THROW(PacBio::CLI::Interface{""}, std::runtime_error);
}

TEST(CLI_Interface, add_single_option_with_a_single_alias)
{
    PacBio::CLI::Interface cl{InterfaceTests::AppName()};
    cl.AddOption({"opt_id", "o", "write output"});  //, "outFile", "default"});

    const auto registered = cl.RegisteredOptions();
    EXPECT_EQ(1, registered.size());
    EXPECT_EQ(std::vector<std::string>({"o"}), registered.at(0).Names());
}

TEST(CLI_Interface, add_single_option_with_multiple_aliases)
{
    PacBio::CLI::Interface cl{InterfaceTests::AppName()};
    cl.AddOption({"opt_id", {"o", "output"}, "write output"});  //, "outFile", "default"});

    const auto registered = cl.RegisteredOptions();
    EXPECT_EQ(1, registered.size());
    EXPECT_EQ(std::vector<std::string>({"o", "output"}), registered.at(0).Names());
}

TEST(CLI_Interface, add_multiple_options_one_at_a_time)
{
    PacBio::CLI::Interface cl{InterfaceTests::AppName()};
    cl.AddOption({"opt_id", {"o", "output"}, "write output"});  //, "outFile", "defaultOut"});
    cl.AddOption({"opt_id2", {"i", "input"}, "write input"});   //, "inFile", "defaultIn"});

    const auto registered = cl.RegisteredOptions();
    EXPECT_EQ(2, registered.size());
    EXPECT_EQ(std::vector<std::string>({"o", "output"}), registered.at(0).Names());
    EXPECT_EQ(std::vector<std::string>({"i", "input"}), registered.at(1).Names());
}

TEST(CLI_Interface, add_multple_options_as_batch)
{
    PacBio::CLI::Interface cl{InterfaceTests::AppName()};
    cl.AddOptions({
        {"opt_id", {"o", "output"}, "write output"},  //, "outFile", "defaultOut"},
        {"opt_id2", {"i", "input"}, "write input"}    //, "inFile", "defaultIn"}
    });

    const auto registered = cl.RegisteredOptions();
    EXPECT_EQ(2, registered.size());
    EXPECT_EQ(std::vector<std::string>({"o", "output"}), registered.at(0).Names());
    EXPECT_EQ(std::vector<std::string>({"i", "input"}), registered.at(1).Names());
}

TEST(CLI_Interface, add_single_positional_arg)
{
    PacBio::CLI::Interface cl{InterfaceTests::AppName()};
    cl.AddPositionalArgument({"source", "Source file to copy.", {}});

    const auto registered = cl.RegisteredPositionalArgs();
    EXPECT_EQ(1, registered.size());
    EXPECT_EQ(std::string{"source"}, registered.at(0).name_);
}

TEST(CLI_Interface, add_multiple_positional_args_one_at_a_time)
{
    PacBio::CLI::Interface cl{InterfaceTests::AppName()};
    cl.AddPositionalArgument({"source", "Source file to copy.", {}});
    cl.AddPositionalArgument({"dest", "Target destination", {}});

    const auto registered = cl.RegisteredPositionalArgs();
    EXPECT_EQ(2, registered.size());
    EXPECT_EQ(std::string{"source"}, registered.at(0).name_);
    EXPECT_EQ(std::string{"dest"}, registered.at(1).name_);
}

TEST(CLI_Interface, add_multiple_positional_args_as_batch)
{
    PacBio::CLI::Interface cl{InterfaceTests::AppName()};
    cl.AddPositionalArguments(
        {{"source", "Source file to copy.", {}}, {"dest", "Target destination", {}}});

    const auto registered = cl.RegisteredPositionalArgs();
    EXPECT_EQ(2, registered.size());
    EXPECT_EQ(std::string{"source"}, registered.at(0).name_);
    EXPECT_EQ(std::string{"dest"}, registered.at(1).name_);
}

TEST(CLI_Interface, does_not_have_help_option_by_default)
{
    PacBio::CLI::Interface cl{InterfaceTests::AppName()};
    EXPECT_FALSE(InterfaceTests::HasOption(cl, "help"));
}

TEST(CLI_Interface, has_help_option_if_requested)
{
    PacBio::CLI::Interface cl{InterfaceTests::AppName()};
    cl.AddHelpOption();
    EXPECT_TRUE(InterfaceTests::HasOption(cl, "help"));
}

TEST(CLI_Interface, does_not_have_verbose_option_by_default)
{
    PacBio::CLI::Interface cl{InterfaceTests::AppName()};
    EXPECT_FALSE(InterfaceTests::HasOption(cl, "verbose"));
}

TEST(CLI_Interface, has_verbose_option_if_requested)
{
    PacBio::CLI::Interface cl{InterfaceTests::AppName()};
    cl.AddVerboseOption();
    EXPECT_TRUE(InterfaceTests::HasOption(cl, "verbose"));
}

TEST(CLI_Interface, does_not_have_version_option_by_default)
{
    PacBio::CLI::Interface cl{InterfaceTests::AppName()};
    EXPECT_FALSE(InterfaceTests::HasOption(cl, "version"));
}

TEST(CLI_Interface, has_version_option_if_requested)
{
    PacBio::CLI::Interface cl{InterfaceTests::AppName()};
    cl.AddVersionOption();
    EXPECT_TRUE(InterfaceTests::HasOption(cl, "version"));
}

TEST(CLI_Interface, add_group)
{
    PacBio::CLI::Interface cl{InterfaceTests::AppName()};

    cl.AddGroup("group1", {{"opt_id", {"o", "output"}, "write output"},
                           {"opt_id2", {"i", "input"}, "write input"}});

    cl.AddGroup("group2", {{"opt_id3", {"p"}, "progress"}});

    const auto groups = cl.Groups();
    EXPECT_EQ(2, groups.size());
    EXPECT_EQ("group1", groups.at(0));
    EXPECT_EQ("group2", groups.at(1));

    const auto group1 = cl.GroupOptions("group1");
    EXPECT_EQ(2, group1.size());
    EXPECT_EQ(std::string{"opt_id"}, group1.at(0).Id());
    EXPECT_EQ(std::string{"opt_id2"}, group1.at(1).Id());

    const auto group2 = cl.GroupOptions("group2");
    EXPECT_EQ(1, group2.size());
    EXPECT_EQ(std::string{"opt_id3"}, group2.at(0).Id());
}
