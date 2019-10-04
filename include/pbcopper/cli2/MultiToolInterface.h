// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_MULTITOOLINTERFACE_H
#define PBCOPPER_CLI_v2_MULTITOOLINTERFACE_H

#include <pbcopper/PbcopperConfig.h>

#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <pbcopper/cli2/Tool.h>
#include <pbcopper/cli2/internal/MultiToolInterfaceData.h>
#include <pbcopper/cli2/internal/OptionData.h>
#include <pbcopper/logging/LogConfig.h>

namespace PacBio {
namespace CLI_v2 {

class MultiToolInterface
{
public:
    MultiToolInterface(std::string name, std::string description = std::string{},
                       std::string version = std::string{});

public:
    MultiToolInterface& AddTool(Tool tool);

    MultiToolInterface& AddTools(std::vector<Tool> tools);

    ///
    /// Set help display footer text.
    ///
    MultiToolInterface& HelpFooter(std::string footer);

    ///
    /// Set logging configuration for application.
    ///
    /// This config overrides individual tool settings. To set per-tool
    /// configs, use their interfaces and do not use this method.
    ///
    MultiToolInterface& LogConfig(Logging::LogConfig config);

public:
    ///
    /// \return application's description
    ///
    const std::string& ApplicationDescription() const;

    ///
    /// \return application's name
    ///
    const std::string& ApplicationName() const;

    ///
    /// \return application's version
    ///
    const std::string& ApplicationVersion() const;

    ///
    /// \return true if requested name is a subtool of this interface
    ///
    bool HasTool(const std::string& toolName) const;

    ///
    /// \return help display footer text.
    ///
    const std::string& HelpFooter() const;

    /// \internal
    /// \return (translated) help option.
    ///
    const internal::OptionData& HelpOption() const;

    ///
    /// \return (optional) log configuration
    ///
    const boost::optional<Logging::LogConfig>& LogConfig() const;

    ///
    /// \return tool for provided name
    ///
    const Tool& ToolFor(const std::string& toolName) const;

    ///
    /// \return multi-interface's sub-tools
    ///
    const std::vector<Tool>& Tools() const;

    /// \internal
    /// \return (translated) help option.
    ///
    const internal::OptionData& VersionOption() const;

private:
    internal::MultiToolInterfaceData data_;
};

}  // namespace CLI_v2
}  // namespace PacBio

#endif  //  PBCOPPER_CLI_v2_MULTITOOLINTERFACE_H
