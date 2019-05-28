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

namespace PacBio {
namespace CLI_v2 {

class MultiToolInterface
{
public:
    MultiToolInterface(std::string name, std::string description = std::string{},
                       std::string version = std::string{});

    MultiToolInterface(const MultiToolInterface&);
    MultiToolInterface(MultiToolInterface&&) noexcept;
    MultiToolInterface& operator=(const MultiToolInterface&);
    MultiToolInterface& operator=(MultiToolInterface&&) noexcept(
        std::is_nothrow_move_assignable<internal::MultiToolInterfaceData>::value);
    ~MultiToolInterface();

public:
    MultiToolInterface& AddTool(const Tool& tool);

    MultiToolInterface& AddTools(const std::vector<Tool>& tools);

    ///
    /// Set help display footer text.
    ///
    MultiToolInterface& HelpFooter(std::string footer);

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

    ///
    /// \return tool for provided name
    ///
    const Tool& ToolFor(const std::string& toolName) const;

    ///
    /// \return multi-interface's sub-tools
    ///
    const std::vector<Tool>& Tools() const;

private:
    internal::MultiToolInterfaceData data_;
};

}  // namespace CLI_v2
}  // namespace PacBio

#endif  //  PBCOPPER_CLI_v2_MULTITOOLINTERFACE_H
