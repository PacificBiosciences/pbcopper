#ifndef PBCOPPER_CLI_TOOLCONTRACT_CONFIG_H
#define PBCOPPER_CLI_TOOLCONTRACT_CONFIG_H

#include <string>

#include <pbcopper/PbcopperConfig.h>
#include "pbcopper/cli/toolcontract/Task.h"
#include "pbcopper/cli/toolcontract/Driver.h"

namespace PacBio {
namespace CLI {
namespace ToolContract {

///
/// \brief The ToolContractConfig class
///
class Config
{
public:
    /// \name Constructors & Related Methods
    /// \{

    /// \brief Contructs a tool contract configuration with the provided task
    ///        settings and default driver settings.
    ///
    /// \param task
    ///
    Config(const ToolContract::Task& task);

    /// \brief Contructs a tool contract configuration with the provided task &
    ///        driver settings.
    ///
    /// \param task
    /// \param driver
    ///
    Config(const ToolContract::Task& task,
           const ToolContract::Driver& driver);

    Config(const Config& other) = default;
    Config(Config&& other) = default;
    Config& operator=(const Config& other) = default;
    Config& operator=(Config&& other) = default;
    ~Config(void) = default;

    /// \}

public:
    /// \name Main Components
    /// \{

    ///
    /// \brief Driver
    /// \return
    ///
    const ToolContract::Driver& Driver(void) const;

    ///
    /// \brief Task
    /// \return
    ///
    const ToolContract::Task& Task(void) const;

    /// \}

public:
    /// \name Other Attributes
    /// \{

    std::string Version(void) const;

    /// \}

public:
    Config& Version(const std::string& version);

private:
    ToolContract::Task task_;
    ToolContract::Driver driver_;
    std::string version_;
};

} // namespace ToolContract
} // namespace CLI
} // namespace PacBio

#include "pbcopper/cli/toolcontract/internal/Config-inl.h"

#endif // PBCOPPER_CLI_TOOLCONTRACT_CONFIG_H
