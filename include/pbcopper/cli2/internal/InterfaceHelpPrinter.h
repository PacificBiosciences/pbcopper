// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_INTERFACEHELPPRINTER_H
#define PBCOPPER_CLI_v2_INTERFACEHELPPRINTER_H

#include <iosfwd>
#include <string>
#include <unordered_map>

#include <boost/optional.hpp>

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/cli2/Interface.h>
#include <pbcopper/cli2/MultiToolInterface.h>
#include <pbcopper/cli2/internal/OptionData.h>
#include <pbcopper/cli2/internal/PositionalArgumentData.h>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

///
/// Terminal & formatted text widths
///
struct FormattedEntry
{
    std::string nameString;  // e.g. "-h,--help"
    std::string typeString;  // e.g. "STR"
};

struct HelpMetrics
{
    std::unordered_map<OptionData, FormattedEntry> formattedOptionNames;
    std::unordered_map<PositionalArgumentData, FormattedEntry> formattedPosArgNames;
    size_t maxNameLength = 0;
    size_t maxTypeLength = 0;
    size_t maxColumn = 79;
};

///
/// Generates help display.
///
class InterfaceHelpPrinter
{
public:
    explicit InterfaceHelpPrinter(Interface interface);

    InterfaceHelpPrinter(Interface interface, const size_t maxColumn);

    ///
    /// Prints formatted text to output stream
    ///
    void Print(std::ostream& out) const;

    /// \internal
    /// \name Formatting helpers, exposed for testing only
    /// \{
    ///
    void CalculateMetrics();
    std::string Choices(const OptionData& option);
    std::string DefaultValue(const OptionData& option);
    std::string Description();
    const HelpMetrics& Metrics();
    std::string Option(const OptionData& option);
    std::string OptionDescription(const OptionData& option);
    std::string OptionGroup(const OptionGroupData& group);
    std::string OptionNames(const OptionData& option);
    std::string Options();
    std::string HelpEntry(std::string name, std::string type, const std::string& description);
    bool ShouldShowDefaultValue(const OptionData& option);
    std::string PositionalArgument(const PositionalArgumentData& posArg);
    std::string PositionalArguments();
    std::string Usage();
    ///
    /// \}

private:
    ///
    /// Generate formatted help text
    ///
    void MakeHelpText();

private:
    Interface interface_;
    HelpMetrics metrics_;
    std::string text_;
};

std::ostream& operator<<(std::ostream& os, const InterfaceHelpPrinter& printer);

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_INTERFACEHELPPRINTER_H
