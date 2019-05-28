// Author: Derek Barnett

#ifndef PBCOPPER_CLI_POSITIONALARG_H
#define PBCOPPER_CLI_POSITIONALARG_H

#include <pbcopper/PbcopperConfig.h>

#include <string>

namespace PacBio {
namespace CLI {

///
/// \brief The PositionalArg struct
///
struct PositionalArg
{
    std::string name_;
    std::string description_;
    std::string syntax_;
};

}  // namespace CLI
}  // namespace PacBio

#endif  // PBCOPPER_CLI_POSITIONALARG_H
