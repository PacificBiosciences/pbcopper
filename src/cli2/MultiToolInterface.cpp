#include <pbcopper/cli2/MultiToolInterface.h>

#include <pbcopper/cli2/internal/BuiltinOptions.h>
#include <pbcopper/cli2/internal/InterfaceData.h>
#include <pbcopper/cli2/internal/OptionTranslator.h>

#include <type_traits>
#include <utility>

#include <cassert>

using OptionTranslator = PacBio::CLI_v2::internal::OptionTranslator;

namespace PacBio {
namespace CLI_v2 {

MultiToolInterface::MultiToolInterface(std::string name, std::string description,
                                       std::string version)
    : data_{std::move(name),
            std::move(description),
            std::move(version),
            OptionTranslator::Translate(Builtin::Help),
            OptionTranslator::Translate(Builtin::Version),
            OptionTranslator::Translate(Builtin::ShowAllHelp)}
{}

MultiToolInterface& MultiToolInterface::AddTool(Tool tool)
{
    // ensure new subtools use the multi-tool's config
    if (data_.LogConfig) {
        tool.interface.LogConfig(*data_.LogConfig);
    }
    data_.Tools.emplace_back(std::move(tool));
    return *this;
}

MultiToolInterface& MultiToolInterface::AddTools(std::vector<Tool> tools)
{
    for (auto&& tool : tools) {
        AddTool(std::move(tool));
    }
    return *this;
}

const std::string& MultiToolInterface::ApplicationDescription() const
{
    return data_.AppDescription;
}

const std::string& MultiToolInterface::ApplicationName() const { return data_.AppName; }

const std::string& MultiToolInterface::ApplicationVersion() const { return data_.AppVersion; }

bool MultiToolInterface::HasTool(const std::string& toolName) const
{
    for (const auto& tool : data_.Tools) {
        if (tool.name == toolName) {
            return true;
        }
    }
    return false;
}

const std::string& MultiToolInterface::HelpFooter() const { return data_.HelpFooter; }

MultiToolInterface& MultiToolInterface::HelpFooter(std::string footer)
{
    data_.HelpFooter = std::move(footer);
    return *this;
}

const std::optional<Logging::LogConfig>& MultiToolInterface::LogConfig() const
{
    return data_.LogConfig;
}

MultiToolInterface& MultiToolInterface::LogConfig(Logging::LogConfig config)
{
    // ensure any current subtools use the new multi-tool config
    data_.LogConfig = config;
    for (auto& tool : data_.Tools) {
        tool.interface.LogConfig(config);
    }
    return *this;
}

const internal::OptionData& MultiToolInterface::HelpOption() const { return data_.HelpOption; }

void MultiToolInterface::PrintVersion() const { data_.VersionPrinter(*this); }

MultiToolInterface& MultiToolInterface::RegisterVersionPrinter(
    MultiToolVersionPrinterCallback printer)
{
    data_.VersionPrinter = printer;
    return *this;
}

const internal::OptionData& MultiToolInterface::ShowAllHelpOption() const
{
    return data_.ShowAllHelpOption;
}

const Tool& MultiToolInterface::ToolFor(const std::string& toolName) const
{
    for (const auto& tool : data_.Tools) {
        if (tool.name == toolName) {
            return tool;
        }
    }
    throw std::runtime_error{"[pbcopper] command line ERROR: unknown tool '" + toolName +
                             "' requested"};
}

const std::vector<Tool>& MultiToolInterface::Tools() const { return data_.Tools; }

const internal::OptionData& MultiToolInterface::VersionOption() const
{
    return data_.VersionOption;
}

}  // namespace CLI_v2
}  // namespace PacBio
