#ifndef PBCOPPER_CLI_v2_INTERFACEHELPPRINTER_H
#define PBCOPPER_CLI_v2_INTERFACEHELPPRINTER_H

#include <pbcopper/PbcopperConfig.h>

#include <iosfwd>
#include <string>
#include <unordered_map>

#include <boost/optional.hpp>

#include <pbcopper/cli2/Interface.h>
#include <pbcopper/cli2/MultiToolInterface.h>
#include <pbcopper/cli2/internal/HelpMetrics.h>
#include <pbcopper/cli2/internal/HiddenOptionMode.h>
#include <pbcopper/cli2/internal/OptionData.h>
#include <pbcopper/cli2/internal/PositionalArgumentData.h>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

///
/// Generates help display.
///
class InterfaceHelpPrinter
{
public:
    InterfaceHelpPrinter(Interface interface, HiddenOptionMode hiddenOptionMode);
    InterfaceHelpPrinter(Interface interface, size_t maxColumn, HiddenOptionMode hiddenOptionMode);

    ///
    /// Prints formatted text to output stream
    ///
    void Print(std::ostream& out) const;

    /// \internal
    /// \name Formatting helpers, exposed for testing only
    /// \{
    ///
    std::string Choices(const OptionData& option);
    std::string DefaultValue(const OptionData& option);
    std::string Description();
    const HelpMetrics& Metrics();
    std::string Option(const OptionData& option);
    std::string OptionDescription(const OptionData& option);
    std::string OptionGroup(const OptionGroupData& group);
    std::string Options();
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
    HelpMetrics metrics_;
    Interface interface_;
    std::string text_;
    bool showHiddenOptions_ = false;
};

std::ostream& operator<<(std::ostream& os, const InterfaceHelpPrinter& printer);

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_INTERFACEHELPPRINTER_H
