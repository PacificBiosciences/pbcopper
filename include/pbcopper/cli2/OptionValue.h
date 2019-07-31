// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_OPTIONVALUE_H
#define PBCOPPER_CLI_v2_OPTIONVALUE_H

#include <pbcopper/PbcopperConfig.h>

#include <string>

#include <boost/variant.hpp>

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
using OptionValue = boost::variant<
    int,
    unsigned int,
    double,
    bool,
    std::string>;

///
/// \return value type enum associated with keyword (e.g. "int" or "string")
///
OptionValueType ValueType(std::string typeString);

}  // namespace CLI_v2
}  // namespace PacBio

// clang-format on

#endif  // PBCOPPER_CLI_v2_OPTIONVALUE_H
