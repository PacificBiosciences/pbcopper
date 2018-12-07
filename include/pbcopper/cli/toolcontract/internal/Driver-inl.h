// Author: Derek Barnett

#ifndef PBCOPPER_CLI_TOOLCONTRACT_DRIVER_INL_H
#define PBCOPPER_CLI_TOOLCONTRACT_DRIVER_INL_H

#include <pbcopper/cli/toolcontract/Driver.h>

namespace PacBio {
namespace CLI {
namespace ToolContract {

inline Driver::Driver(std::string exe) : exe_{std::move(exe)} {}

inline Driver::Driver(std::string exe, Environment env, std::string serialization)
    : exe_{std::move(exe)}, serialization_{std::move(serialization)}, env_{std::move(env)}
{
}

inline const Driver::Environment& Driver::Env() const { return env_; }

inline Driver& Driver::Env(Driver::Environment env)
{
    env_ = std::move(env);
    return *this;
}

inline const std::string& Driver::Exe() const { return exe_; }

inline Driver& Driver::Exe(std::string exe)
{
    exe_ = std::move(exe);
    return *this;
}

inline const std::string& Driver::Serialization() const { return serialization_; }

inline Driver& Driver::Serialization(std::string serialization)
{
    serialization_ = std::move(serialization);
    return *this;
}

}  // namespace ToolContract
}  // namespace CLI
}  // namespace PacBio

#endif  // PBCOPPER_CLI_TOOLCONTRACT_DRIVER_INL_H
