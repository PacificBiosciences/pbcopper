#include <sstream>

#include <gtest/gtest.h>

#include <pbcopper/cli2/Interface.h>
#include <pbcopper/cli2/MultiToolInterface.h>
#include <pbcopper/cli2/internal/VersionPrinter.h>

using VersionPrinter = PacBio::CLI_v2::internal::VersionPrinter;

// clang-format off

namespace CLI_v2_VersionPrinterTests {

void Check(const VersionPrinter& version)
{
    const std::string expectedText{"frobber 2.0"};

    std::ostringstream out;
    version.Print(out);
    EXPECT_EQ(expectedText, out.str());

    // reset ostream and check operator<<
    out.str("");
    out << version;
    EXPECT_EQ(expectedText, out.str());
}

} // namespace VersionPrinterTests

TEST(CLI2_VersionPrinter, prints_application_name_and_version)
{
    const PacBio::CLI_v2::Interface i{"frobber", "Frob all the things", "2.0"};
    CLI_v2_VersionPrinterTests::Check(VersionPrinter{i});
}

TEST(CLI2_VersionPrinter, prints_multitool_name_and_version)
{
    const PacBio::CLI_v2::MultiToolInterface i{"frobber", "Frob all the things", "2.0"};
    CLI_v2_VersionPrinterTests::Check(VersionPrinter{i});
}

// clang-format on
