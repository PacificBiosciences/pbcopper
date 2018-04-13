// Author: Derek Barnett

#ifndef PBCOPPER_CLI_TOOLCONTRACT_CONFIG_INL_H
#define PBCOPPER_CLI_TOOLCONTRACT_CONFIG_INL_H

#include <pbcopper/cli/toolcontract/Config.h>

namespace PacBio {
namespace CLI {
namespace ToolContract {

inline Config::Config(const ToolContract::Task& task) : task_(task) {}

inline Config::Config(const ToolContract::Task& task, const ToolContract::Driver& driver)
    : task_(task), driver_(driver)
{
}

inline const Driver& Config::Driver(void) const { return driver_; }

inline const Task& Config::Task(void) const { return task_; }

inline std::string Config::Version(void) const { return version_; }

inline Config& Config::Version(const std::string& version)
{
    version_ = version;
    return *this;
}

}  // namespace ToolContract
}  // namespace CLI
}  // namespace PacBio

#endif  // PBCOPPER_CLI_TOOLCONTRACT_CONFIG_INL_H
