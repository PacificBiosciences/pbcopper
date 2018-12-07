// Author: Derek Barnett

#ifndef PBCOPPER_CLI_TOOLCONTRACT_CONFIG_H
#define PBCOPPER_CLI_TOOLCONTRACT_CONFIG_H

#include <string>

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/cli/toolcontract/Driver.h>
#include <pbcopper/cli/toolcontract/Task.h>

namespace PacBio {
namespace CLI {
namespace ToolContract {

///
/// \brief The ToolContractConfig struct
///
struct Config
{
public:
    /// \name Constructors & Related Methods
    /// \{

    /// \brief Contructs a tool contract configuration with the provided task
    ///        settings and default driver settings.
    ///
    /// \param task
    ///
    Config(Task task) : task_{std::move(task)} {}

    /// \brief Contructs a tool contract configuration with the provided task &
    ///        driver settings.
    ///
    /// \param task
    /// \param driver
    ///
    Config(ToolContract::Task task, ToolContract::Driver driver)
        : task_{std::move(task)}, driver_{std::move(driver)}
    {
    }

    Config(const Config&) = default;
    Config(Config&&) = default;
    Config& operator=(const Config&) = default;
    Config& operator=(Config&&) = default;
    ~Config() = default;

    /// \}

public:
    ToolContract::Task task_;
    ToolContract::Driver driver_;
    std::string version_;
};

}  // namespace ToolContract
}  // namespace CLI
}  // namespace PacBio

#endif  // PBCOPPER_CLI_TOOLCONTRACT_CONFIG_H
