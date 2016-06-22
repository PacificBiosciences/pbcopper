#include <pbcopper/cli/VersionPrinter.h>

#include <pbcopper/cli/Interface.h>
#include <gtest/gtest.h>
#include <sstream>
#include <string>
using namespace PacBio;
using namespace PacBio::CLI;
using namespace std;

TEST(CLI_VersionPrinter, prints_expected_version_output)
{
    const Interface interface{ "myApp", "does the things", "3.14" };
    const string expectedText = "myApp 3.14\n";

    stringstream s;
    VersionPrinter::Print(interface, s);
    EXPECT_EQ(expectedText, s.str());
}
