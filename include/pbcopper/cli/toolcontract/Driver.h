// Author: Derek Barnett

#ifndef PBCOPPER_CLI_TOOLCONTRACT_DRIVER_H
#define PBCOPPER_CLI_TOOLCONTRACT_DRIVER_H

#include <pbcopper/PbcopperConfig.h>

#include <string>
#include <unordered_map>

namespace PacBio {
namespace CLI {
namespace ToolContract {

///
/// \brief The ToolDriver class
///
class Driver
{
public:
    typedef std::unordered_map<std::string, std::string> Environment;

public:
    /// \name Constructors & Related Methods
    /// \{

    ///
    /// \brief ToolDriver
    /// \param exe
    ///
    explicit Driver(std::string exe);

    ///
    /// \brief ToolDriver
    /// \param exe
    /// \param env
    /// \param serialization
    ///
    Driver(std::string exe, Environment env, std::string serialization);

    Driver() = default;
    Driver(const Driver&) = default;
    Driver(Driver&&) noexcept = default;
    Driver& operator=(const Driver&) = default;
    Driver& operator=(Driver&&) noexcept = default;
    ~Driver() = default;

    /// \}

public:
    /// \name Attributes
    /// \{

    ///
    /// \brief Env
    /// \return
    ///
    const Environment& Env() const;

    ///
    /// \brief Exe
    /// \return
    ///
    const std::string& Exe() const;

    ///
    /// \brief Serialization
    /// \return
    ///
    const std::string& Serialization() const;

    /// \}

public:
    /// \name Attributes
    /// \{

    ///
    /// \brief Env
    /// \param env
    /// \return
    ///
    Driver& Env(Environment env);

    ///
    /// \brief Exe
    /// \param exe
    /// \return
    ///
    Driver& Exe(std::string exe);

    ///
    /// \brief Serialization
    /// \param serialization
    /// \return
    ///
    Driver& Serialization(std::string serialization);

    /// \}

private:
    std::string exe_;
    std::string serialization_;
    std::unordered_map<std::string, std::string> env_;
};

}  // namespace ToolContract
}  // namespace CLI
}  // namespace PacBio

#include <pbcopper/cli/toolcontract/internal/Driver-inl.h>

#endif  // PBCOPPER_CLI_TOOLCONTRACT_DRIVER_H
