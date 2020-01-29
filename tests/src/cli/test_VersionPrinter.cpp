#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include <pbcopper/cli/Interface.h>
#include <pbcopper/cli/VersionPrinter.h>

TEST(CLI_VersionPrinter, prints_expected_version_output)
{
    const PacBio::CLI::Interface interface{"myApp", "does the things", "3.14"};
    const std::string expectedText{"myApp 3.14\n"};

    std::ostringstream s;
    PacBio::CLI::VersionPrinter::Print(interface, s);
    EXPECT_EQ(expectedText, s.str());
}
