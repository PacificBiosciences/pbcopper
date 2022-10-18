#include <pbcopper/cli2/Interface.h>

#include <exception>
#include <stdexcept>
#include "pbcopper/cli2/Option.h"

#include <gtest/gtest.h>

using namespace PacBio;

TEST(CLI2_Interface, throws_on_empty_application_name)
{
    const auto ShouldThrowSetupError = [](const std::string& name, const std::string& description,
                                          const std::string& version) {
        try {
            const CLI_v2::Interface i{name, description, version};
            EXPECT_FALSE("got here");
        } catch (const std::exception& e) {
            EXPECT_NE(std::string{e.what()}.find("command line interface setup ERROR"),
                      std::string::npos);
        }
    };

    ShouldThrowSetupError("", "", "");
    ShouldThrowSetupError("", "", "version");
    ShouldThrowSetupError("", "description", "");
    ShouldThrowSetupError("", "description", "version");
}

TEST(CLI2_Interface, throws_on_duplicate_option_name)
{
    // clang-format off
    const PacBio::CLI_v2::Option Method
    {
    R"({
        "names" : ["m", "method"],
        "description" : "Test",
        "type" : "string",
        "default" : "standard"
    })"
    };
    const PacBio::CLI_v2::Option Min
    {
    R"({
        "names" : ["m"],
        "description" : "Minimum thing",
        "type" : "int",
        "default" : 10
    })"
    };
    // clang-format on

    CLI_v2::Interface i{"app", "description", "version"};
    i.AddOption(Method);
    try {
        i.AddOption(Min);
        EXPECT_FALSE("got here");
    } catch (const std::exception& e) {
        EXPECT_NE(std::string{e.what()}.find("command line interface setup ERROR"),
                  std::string::npos);
    }
}

TEST(CLI2_Interface, throws_on_duplicate_option_group_name)
{
    CLI_v2::Interface i{"app", "description", "version"};
    try {
        i.AddOptionGroup("group1", {});
        i.AddOptionGroup("group1", {});
        EXPECT_FALSE("got here");
    } catch (const std::exception& e) {
        EXPECT_NE(std::string{e.what()}.find("command line interface setup ERROR"),
                  std::string::npos);
    }
}
