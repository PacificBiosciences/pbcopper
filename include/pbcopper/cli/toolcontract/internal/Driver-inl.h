// Author: Derek Barnett

#ifndef PBCOPPER_CLI_TOOLCONTRACT_DRIVER_INL_H
#define PBCOPPER_CLI_TOOLCONTRACT_DRIVER_INL_H

#include <pbcopper/cli/toolcontract/Driver.h>

#include <cassert>
#include <type_traits>

namespace PacBio {
namespace CLI {
namespace ToolContract {

static_assert(std::is_copy_constructible<Driver>::value, "Driver(const Driver&) is not = default");
static_assert(std::is_copy_assignable<Driver>::value,
              "Driver& operator=(const Driver&) is not = default");

static_assert(std::is_nothrow_move_constructible<Driver>::value,
              "Driver(Driver&&) is not = noexcept");
static_assert(std::is_nothrow_move_assignable<Driver>::value ==
                  std::is_nothrow_move_assignable<std::string>::value,
              "");

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
