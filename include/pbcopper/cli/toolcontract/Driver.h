#ifndef PBCOPPER_CLI_TOOLCONTRACT_DRIVER_H
#define PBCOPPER_CLI_TOOLCONTRACT_DRIVER_H

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
    ///
    Driver(void);

    ///
    /// \brief ToolDriver
    /// \param exe
    ///
    Driver(const std::string& exe);

    ///
    /// \brief ToolDriver
    /// \param exe
    /// \param env
    /// \param serialization
    ///
    Driver(const std::string& exe,
               const Environment& env,
               const std::string& serialization);

    Driver(const Driver& other) = default;
    Driver(Driver&& other) = default;
    Driver& operator=(const Driver& other) = default;
    Driver& operator=(Driver&& other) = default;
    ~Driver(void) = default;

    /// \}

public:
    /// \name Attributes
    /// \{

    ///
    /// \brief Env
    /// \return
    ///
    const Environment& Env(void) const;

    ///
    /// \brief Exe
    /// \return
    ///
    const std::string& Exe(void) const;

    ///
    /// \brief Serialization
    /// \return
    ///
    const std::string& Serialization(void) const;

    /// \}

public:
    /// \name Attributes
    /// \{

    ///
    /// \brief Env
    /// \param env
    /// \return
    ///
    Driver& Env(const Environment& env);

    ///
    /// \brief Exe
    /// \param exe
    /// \return
    ///
    Driver& Exe(const std::string& exe);

    ///
    /// \brief Serialization
    /// \param serialization
    /// \return
    ///
    Driver& Serialization(const std::string& serialization);

    /// \}

private:
    std::string exe_;
    std::string serialization_;
    std::unordered_map<std::string, std::string> env_;
};

} // namespace ToolContract
} // namespace CLI
} // namespace PacBio

#include "internal/Driver-inl.h"

#endif // PBCOPPER_CLI_TOOLCONTRACT_DRIVER_H
