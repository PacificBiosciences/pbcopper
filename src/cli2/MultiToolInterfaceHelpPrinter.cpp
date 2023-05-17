#include <pbcopper/cli2/internal/MultiToolInterfaceHelpPrinter.h>

#include <pbcopper/cli2/internal/OptionData.h>
#include "PbBoilerplateDisclaimer.h"

#include <iomanip>
#include <ios>
#include <ostream>
#include <sstream>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

MultiToolInterfaceHelpPrinter::MultiToolInterfaceHelpPrinter(MultiToolInterface multiToolInterface,
                                                             HiddenOptionMode hiddenOptionMode)
    : metrics_{multiToolInterface, hiddenOptionMode}
    , interface_{std::move(multiToolInterface)}
    , showHiddenOptions_{hiddenOptionMode == HiddenOptionMode::SHOW}
{
    MakeHelpText();
}

MultiToolInterfaceHelpPrinter::MultiToolInterfaceHelpPrinter(MultiToolInterface multiToolInterface,
                                                             const std::size_t maxColumn,
                                                             HiddenOptionMode hiddenOptionMode)
    : metrics_{multiToolInterface, maxColumn, hiddenOptionMode}
    , interface_{std::move(multiToolInterface)}
    , showHiddenOptions_{hiddenOptionMode == HiddenOptionMode::SHOW}
{
    MakeHelpText();
}

void MultiToolInterfaceHelpPrinter::MakeHelpText()
{
    std::ostringstream result;
    result << interface_.ApplicationName();

    const std::string description = interface_.ApplicationDescription();
    if (!description.empty()) {
        result << " - " << description;
    }
    result << '\n';

    result << '\n'
           << "Usage:\n"
           << "  " << interface_.ApplicationName() << " <tool>\n"
           << '\n';

    result << metrics_.HelpEntry(interface_.HelpOption()) << '\n'
           << metrics_.HelpEntry(interface_.VersionOption()) << "\n\n";

    bool hasExamples = false;
    result << "Tools:\n";

    const auto& tools = interface_.Tools();
    for (const auto& tool : tools) {
        if ((tool.visibility == ToolVisibility::HIDDEN) && !showHiddenOptions_) {
            continue;
        }
        const auto& i = tool.interface;
        result << "  " << std::setw(10) << std::left << tool.name << " "
               << i.ApplicationDescription() << '\n';
        if (!tool.interface.Example().empty()) {
            hasExamples = true;
        }
    }
    result << '\n';

    if (hasExamples) {
        result << "Examples:\n";
        for (const auto& tool : tools) {
            if (tool.visibility == ToolVisibility::HIDDEN) {
                continue;
            }
            const auto& example = tool.interface.Example();
            if (!example.empty()) {
                result << "  " << example << '\n';
            }
        }
        result << '\n';
    }

    const auto& footer = interface_.HelpFooter();
    if (!footer.empty()) {
        result << footer << '\n';

        // Some application footers contain trailing newlines, some do not.
        // Ensure we have at least one empty line between footer and disclaimer.
        if (footer.back() != '\n') {
            result << '\n';
        }
    }

    result << PB_BOILERPLATE_DISCLAIMER;

    text_ = result.str();
}

void MultiToolInterfaceHelpPrinter::Print(std::ostream& out) const { out << text_; }

std::ostream& operator<<(std::ostream& out, const MultiToolInterfaceHelpPrinter& printer)
{
    printer.Print(out);
    return out;
}

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio
