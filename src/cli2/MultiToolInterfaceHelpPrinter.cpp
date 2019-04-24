#include <pbcopper/cli2/internal/MultiToolInterfaceHelpPrinter.h>

#include <sys/ioctl.h>
#include <unistd.h>

#include <iomanip>
#include <iostream>
#include <sstream>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

MultiToolInterfaceHelpPrinter::MultiToolInterfaceHelpPrinter(MultiToolInterface multiToolInterface)
    : interface_{std::move(multiToolInterface)}
{
    // determine column count from terminal width
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    if (ws.ws_col >= 2) metrics_.maxColumn = ws.ws_col - 1;
    constexpr const size_t MaxColumn = 119;
    metrics_.maxColumn = std::min(metrics_.maxColumn, MaxColumn);

    CalculateMetrics();
    MakeHelpText();
}

MultiToolInterfaceHelpPrinter::MultiToolInterfaceHelpPrinter(MultiToolInterface multiToolInterface,
                                                             const size_t maxColumn)
    : interface_{std::move(multiToolInterface)}
{
    // use client-provided column count
    metrics_.maxColumn = maxColumn;

    CalculateMetrics();
    MakeHelpText();
}

void MultiToolInterfaceHelpPrinter::CalculateMetrics()
{
    // TODO: calculate metrics, a la standard interface help printer, with
    //       explicit terminal widths, etc.
}

void MultiToolInterfaceHelpPrinter::MakeHelpText()
{
    std::ostringstream result;
    result << interface_.ApplicationName();

    const std::string description = interface_.ApplicationDescription();
    if (!description.empty()) result << " - " << description;
    result << '\n';

    result << '\n'
           << "Usage:\n"
           << "  " << interface_.ApplicationName() << " <tool>\n"
           << '\n';

    bool hasExamples = false;
    result << "Tools:\n";

    const auto& tools = interface_.Tools();
    for (const auto& tool : tools) {
        const auto& i = tool.interface;
        result << "  " << std::setw(11) << std::left << tool.name << i.ApplicationDescription()
               << '\n';
        if (!tool.interface.Example().empty()) hasExamples = true;
    }
    result << '\n';

    if (hasExamples) {
        result << "Examples:\n";
        for (const auto& tool : tools) {
            const auto& example = tool.interface.Example();
            if (!example.empty()) {
                result << "  " << example << '\n';
            }
        }
        result << '\n';
    }

    const auto& footer = interface_.HelpFooter();
    if (!footer.empty()) result << footer << '\n';

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