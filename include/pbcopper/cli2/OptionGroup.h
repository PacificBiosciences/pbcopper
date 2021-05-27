#ifndef PBCOPPER_CLI_v2_OPTIONGROUP_H
#define PBCOPPER_CLI_v2_OPTIONGROUP_H

#include <pbcopper/PbcopperConfig.h>

#include <string>
#include <vector>

#include <pbcopper/cli2/Option.h>

namespace PacBio {
namespace CLI_v2 {

///
/// Represents an option group. This is mainly used for organizing help display.
///
struct OptionGroup
{
    std::string name;
    std::vector<Option> options;
};

}  // namespace CLI_v2
}  // namespace PacBio

#endif  //  PBCOPPER_CLI_v2_OPTIONGROUP_H
