// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_MULTITOOLINTERFACEHELPPRINTER_H
#define PBCOPPER_CLI_v2_MULTITOOLINTERFACEHELPPRINTER_H

#include <iosfwd>
#include <string>

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/cli2/MultiToolInterface.h>
#include <pbcopper/cli2/internal/InterfaceHelpPrinter.h>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

class MultiToolInterfaceHelpPrinter
{
public:
    explicit MultiToolInterfaceHelpPrinter(MultiToolInterface multiToolInterface);
    MultiToolInterfaceHelpPrinter(MultiToolInterface interface, const size_t maxColumn);

    void Print(std::ostream& out) const;

private:
    void CalculateMetrics();
    void MakeHelpText();

private:
    MultiToolInterface interface_;
    HelpMetrics metrics_;
    std::string text_;
};

std::ostream& operator<<(std::ostream& os, const MultiToolInterfaceHelpPrinter& printer);

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_MULTITOOLINTERFACEHELPPRINTER_H