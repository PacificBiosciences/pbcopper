#include <pbcopper/cli2/internal/VersionPrinter.h>

#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include <pbcopper/cli2/Interface.h>
#include <pbcopper/cli2/MultiToolInterface.h>

using namespace PacBio;

TEST(CLI2_VersionPrinter, prints_application_name_and_version)
{
    const CLI_v2::Interface i{"frobber", "Frob all the things", "2.0"};
    const std::string expectedText{"frobber 2.0\n"};

    std::ostringstream out;
    CLI_v2::internal::VersionPrinter::Print(i, out);
    EXPECT_EQ(expectedText, out.str());
}

TEST(CLI2_VersionPrinter, prints_multitool_name_and_version)
{
    const CLI_v2::MultiToolInterface i{"frobber", "Frob all the things", "2.0"};
    const std::string expectedText{"frobber 2.0\n"};

    std::ostringstream out;
    CLI_v2::internal::VersionPrinter::Print(i, out);
    EXPECT_EQ(expectedText, out.str());
}
