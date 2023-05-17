#ifndef PBCOPPER_CLI_v2_MULTITOOLINTERFACEHELPPRINTER_H
#define PBCOPPER_CLI_v2_MULTITOOLINTERFACEHELPPRINTER_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/cli2/MultiToolInterface.h>
#include <pbcopper/cli2/internal/InterfaceHelpPrinter.h>

#include <iosfwd>
#include <string>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

class MultiToolInterfaceHelpPrinter
{
public:
    MultiToolInterfaceHelpPrinter(MultiToolInterface multiToolInterface,
                                  HiddenOptionMode hiddenOptionMode);
    MultiToolInterfaceHelpPrinter(MultiToolInterface interface, std::size_t maxColumn,
                                  HiddenOptionMode hiddenOptionMode);

    void Print(std::ostream& out) const;

private:
    void MakeHelpText();

private:
    HelpMetrics metrics_;
    MultiToolInterface interface_;
    std::string text_;
    bool showHiddenOptions_ = false;
};

std::ostream& operator<<(std::ostream& os, const MultiToolInterfaceHelpPrinter& printer);

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_MULTITOOLINTERFACEHELPPRINTER_H
