#ifndef PBCOPPER_CLI_v2_OPTIONVALUE_H
#define PBCOPPER_CLI_v2_OPTIONVALUE_H

#include <pbcopper/PbcopperConfig.h>

#include <iosfwd>
#include <string>
#include <variant>
#include <cstdint>

// clang-format off

namespace PacBio {
namespace CLI_v2 {

///
/// Represents the possible values types associated with an interface option.
///
enum class OptionValueType
{
    INT,
    UINT,
    FLOAT,
    BOOL,
    STRING,
    FILE,
    DIR
};

///
/// This variant carries the value associated with an option - parsed from the
/// command line or a resolved tool contract.
///
using OptionValue = std::variant<
    int8_t,
    int16_t,
    int32_t,
    int64_t,
    uint8_t,
    uint16_t,
    uint32_t,
    uint64_t,
    double,
    bool,
    std::string>;

///
/// \return value type enum associated with keyword (e.g. "int" or "string")
///
OptionValueType ValueType(std::string typeString);

///
/// \return true if option is interpreted as a number on command line
///
constexpr bool IsNumeric(OptionValueType type)
{
    return (type == OptionValueType::INT) ||
           (type == OptionValueType::UINT) ||
           (type == OptionValueType::FLOAT);
}

///
/// \return true if option may be interpreted as a negative number on command line
///
constexpr bool CanBeNegative(OptionValueType type)
{
    return (type == OptionValueType::INT) ||
           (type == OptionValueType::FLOAT);
}

///
/// \return true if option is interpreted as string on command line
///
constexpr bool IsStringLike(OptionValueType type) {
    return (type == OptionValueType::STRING) ||
           (type == OptionValueType::FILE) ||
           (type == OptionValueType::DIR);
}

///
/// \return int64_t value of option
/// \throws if not a signed integer type (any width)
///
int64_t OptionValueToInt(const OptionValue& value);

///
/// \return uint64_t value of option
/// \throws if not an unsigned integer type (any width)
///
uint64_t OptionValueToUInt(const OptionValue& value);

///
/// \return double value of option
/// \throws if not a floating-point type
///
double OptionValueToDouble(const OptionValue& value);

///
/// \return bool value of option
/// \throws if not a bool type
///
bool OptionValueToBool(const OptionValue& value);

///
/// \return string value of option
/// \throws if not a string type
//
std::string OptionValueToString(const OptionValue& value);

///
/// print to stream, depending on underlying type
///
std::ostream& operator<<(std::ostream& out, const OptionValue& value);

}  // namespace CLI_v2
}  // namespace PacBio

// clang-format on

#endif  // PBCOPPER_CLI_v2_OPTIONVALUE_H
