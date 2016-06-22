#ifndef PBCOPPER_CLI_TOOLCONTRACT_DRIVER_INL_H
#define PBCOPPER_CLI_TOOLCONTRACT_DRIVER_INL_H

#include "pbcopper/cli/toolcontract/Driver.h"

namespace PacBio {
namespace CLI {
namespace ToolContract {

inline Driver::Driver(void) { }

inline Driver::Driver(const std::string& exe)
    : exe_(exe)
{ }

inline Driver::Driver(const std::string& exe,
        const Environment& env,
        const std::string& serialization)
    : exe_(exe)
    , serialization_(serialization)
    , env_(env)
{ }

inline const Driver::Environment& Driver::Env(void) const
{ return env_; }

inline Driver& Driver::Env(const Driver::Environment& env)
{ env_ = env; return *this; }

inline const std::string& Driver::Exe(void) const
{ return exe_; }

inline Driver& Driver::Exe(const std::string& exe)
{ exe_ = exe; return *this; }

inline const std::string& Driver::Serialization(void) const
{ return serialization_; }

inline Driver& Driver::Serialization(const std::string& serialization)
{ serialization_ = serialization; return *this; }

} // namespace ToolContract
} // namespace CLI
} // namespace PacBio

#endif // PBCOPPER_CLI_TOOLCONTRACT_DRIVER_INL_H
