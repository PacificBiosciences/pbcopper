#include "pbcopper/cli/HelpPrinter.h"
#include "pbcopper/cli/Interface.h"
#include "pbcopper/json/JSON.h"

#include <cstddef>
#include <sys/ioctl.h>
#include <unistd.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

using Json = PacBio::JSON::Json;

namespace PacBio {
namespace CLI {
namespace internal {

static
bool shouldIncludeDefaultValue(const Json& defaultValue)
{
    // omit if switch OR null
    if (defaultValue.is_boolean() || defaultValue.is_null())
        return false;

    // omit if empty string default on string-type option
    if (defaultValue.is_string()) {
        const std::string val = defaultValue;
        if (val.empty())
            return false;
    }

    // otherwise include default value
    return true;
}

static
std::string formatOption(std::string optionOutput,
                         size_t longestOptionLength,
                         std::string description,
                         Json defaultValue)
{
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    const size_t ncol = (ws.ws_col < 2) ? 79 : ws.ws_col - 1;

    std::stringstream result("");
    auto fullDescription = description;

    // maybe add default value to description
    if (shouldIncludeDefaultValue(defaultValue)) {

        fullDescription += " [";

        // We need to bypass nlohmann::json's dump() method for floating point
        // values. It ends up trying to output _far_ too many digits. Without
        // special handling, we would get:
        //
        //  --foo  Description of foo. [0.0900000035762787]
        //  --bar  Description of bar. [0.100000001490116]
        //
        // when we are expecting:
        //
        //  --foo  Description of foo. [0.09]
        //  --bar  Description of bar. [0.1]
        //
        // std::ostream APIs do the right thing in this case.
        //
        // NOTE: For future reference, std::to_string sounds right here, but
        //       isn't. It uses a fixed precision of 6 for floats.
        //
        if (defaultValue.is_number_float()) {
            const float f = defaultValue;
            std::stringstream s;
            s << f;
            fullDescription += s.str();
        } else {
            fullDescription += defaultValue.dump();
        }

        fullDescription += "]";
    }

    // option names
    optionOutput.resize(longestOptionLength, ' ');
    result << "  " << optionOutput << " ";

    // maybe wrap description
    const auto indent = result.str().length();
    auto lineStart     = size_t{ 0 };
    auto lastBreakable = std::string::npos;
    const auto max     = size_t{ ncol - indent };
    auto x             = size_t{ 0 };
    const auto len = fullDescription.length();

    for (size_t i = 0; i < len; ++i) {
        ++x;
        const auto c = fullDescription.at(i);
        if (isspace(c))
            lastBreakable = i;

        auto breakAt = std::string::npos;
        auto nextLineStart = std::string::npos;

        if (x > max && lastBreakable != std::string::npos) {
            breakAt = lastBreakable;
            nextLineStart = lastBreakable + 1;
        } else if ( (x > max-1 && lastBreakable == std::string::npos) || i == len-1) {
            breakAt = i + 1;
            nextLineStart = breakAt;
        } else if (c == '\n') {
            breakAt = i;
            nextLineStart = i+1;
        }

        if (breakAt != std::string::npos) {
            const auto numChars = breakAt - lineStart;
            if (lineStart > 0)
                result << std::string(indent, ' ');
            result << fullDescription.substr(lineStart, numChars) << std::endl;
            x = 0;
            lastBreakable = std::string::npos;
            lineStart = nextLineStart;
            if (lineStart < len && isspace(fullDescription.at(lineStart)))
                ++lineStart;
            i = lineStart;
        }
    }
    return result.str();
}

static
std::string formatOptionNames(const Option& option)
{
    std::stringstream optionOutput("");
    auto first = true;
    for(const auto& name : option.Names()) {
        if (first)
            first = false;
        else
            optionOutput << ",";

        if (name.size() == 1)
            optionOutput << "-";
        else
            optionOutput << "--";

        optionOutput << name;
    }

    if (!option.ValueName().empty())
        optionOutput << " <" << option.ValueName() << ">";

    return optionOutput.str();
}

static
std::string makeHelpText(const Interface& interface)
{
    std::stringstream result("");

    const auto options = interface.RegisteredOptions();
    const auto posArgs = interface.RegisteredPositionalArgs();

    // setup usage output
    {
        std::stringstream usage("");
        usage << interface.ApplicationName();
        if (!options.empty())
            usage << " [options]";
        for (const auto& posArg : posArgs)
            usage << " " << posArg.syntax_;
        result << "Usage: " << usage.str() << std::endl;
    }

    // description
    const auto appDescription = interface.ApplicationDescription();
    if (!appDescription.empty())
        result << appDescription << std::endl;

    // empty line
    result << std::endl;

    // options
    auto formattedOptions = std::map<std::string, std::string>{ }; // id -> formatted output
    auto longestOptionOutputLength = size_t{ 0 };

    // determine longest option names & store formatting for use later
    for(const auto& option : options) {
        if (option.IsHidden())
            continue;
        const auto optionOutputString = formatOptionNames(option);
        formattedOptions[option.Id()] = optionOutputString;
        longestOptionOutputLength = std::max(longestOptionOutputLength,
                                             optionOutputString.size());
    }

    // spacer
    ++longestOptionOutputLength;

    if (!options.empty())
    {
        auto printGroup = [](const Interface& interface,
                             const std::string& group,
                             const std::map<std::string, std::string>& formattedOptions,
                             const size_t longestOptionOutputLength,
                             std::stringstream& result)
        {
            const auto& opts = interface.GroupOptions(group);
            if (opts.empty())
                return;
            result << group << ":\n";
            for (const auto& opt : opts) {
                if (opt.IsHidden())
                    continue;
                result << formatOption(formattedOptions.at(opt.Id()),
                                       longestOptionOutputLength,
                                       opt.Description(),
                                       opt.DefaultValue());
            }

            result << '\n';
        };

        // print all non-default groups in the order they were added
        const auto& groups = interface.Groups();
        for (const auto& group : groups) {
            if (group != "Options") {
                printGroup(interface, group, formattedOptions,
                           longestOptionOutputLength, result);
            }
        }

        // print default group last (help, version, etc) or options added
        // to interface directly (no group)
        printGroup(interface, "Options", formattedOptions,
                   longestOptionOutputLength,result);
    }


    // positional args
    if (!posArgs.empty()) {
        result << "Arguments:\n";
        for (const auto& posArg : posArgs) {
            result << formatOption(posArg.name_,
                                   longestOptionOutputLength,
                                   posArg.description_,
                                   Json());
        }
    }

    return result.str();
}

} // namespace internal

void HelpPrinter::Print(const Interface& interface, std::ostream& out)
{
    out << internal::makeHelpText(interface) << std::endl;
}

} // namespace CLI
} // namespace PacBio
