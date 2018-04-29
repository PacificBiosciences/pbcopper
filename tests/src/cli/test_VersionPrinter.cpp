#include <pbcopper/cli/VersionPrinter.h>

#include <gtest/gtest.h>
#include <pbcopper/cli/Interface.h>
#include <sstream>
#include <string>
using namespace PacBio;
using namespace PacBio::CLI;

TEST(CLI_VersionPrinter, prints_expected_version_output)
{
    const Interface interface{"myApp", "does the things", "3.14"};
    const std::string expectedText{"myApp 3.14\n"};

    std::ostringstream s;
    VersionPrinter::Print(interface, s);
    EXPECT_EQ(expectedText, s.str());
}
