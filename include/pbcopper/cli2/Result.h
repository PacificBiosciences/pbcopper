// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_RESULT_H
#define PBCOPPER_CLI_v2_RESULT_H

#include <pbcopper/PbcopperConfig.h>

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include <pbcopper/cli2/OptionValue.h>

namespace PacBio {
namespace CLI_v2 {

///
/// Represents the source of a result's value.
///
enum class SetByMode
{
    DEFAULT,
    USER
};

class Results;

///
/// Represents an option's value, as determined via command line or resolved
/// tool contract.
///
/// In most cases, this class can be ignored in client code. Standard usage will
/// be something like:
///
///    settings.x = results[Option::X];
///
/// where this class acts as the "middle man" between the CLI::Results and a basic
/// type.
///
class Result
{
public:
    Result();
    Result(OptionValue data, SetByMode mode = SetByMode::DEFAULT);

    Result(const Result&) = default;
    Result(Result&&) noexcept = default;
    Result& operator=(const Result&) = default;
    Result& operator=(Result&&) noexcept = default;
    ~Result() = default;

    ///
    /// Convience method for querying value's source.
    ///
    /// \return true if this value was set via command line
    ///
    bool IsUserProvided() const;

    ///
    /// Convience method for querying value's source.
    ///
    /// \return true if no value was provided, and is thus the default from the
    ///         option's definition.
    ///
    bool IsDefault() const;

    ///
    /// \return enum representing the value's source
    ///
    SetByMode SetBy() const;

    ///
    /// Sets the value's source
    ///
    Result& SetBy(SetByMode setBy);

    //
    // \name Conversion operators & type-related methods
    // \{
    ///
    operator int() const;
    operator unsigned int() const;
    operator float() const;
    operator bool() const;
    operator std::string() const;
    size_t which() const;
    ///
    /// \}

private:
    OptionValue data_;
    SetByMode setBy_;
};

}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_RESULT_H
