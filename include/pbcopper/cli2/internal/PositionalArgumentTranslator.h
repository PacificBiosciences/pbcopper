// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_POSITIONALARGUMENTTRANSLATOR_H
#define PBCOPPER_CLI_v2_POSITIONALARGUMENTTRANSLATOR_H

#include <string>
#include <vector>

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/cli2/PositionalArgument.h>
#include <pbcopper/cli2/internal/PositionalArgumentData.h>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

///
/// This translator is responsible for converting JSON definitions into
/// positional argument properties
///
struct PositionalArgumentTranslator
{
    ///
    /// Fetch only the name for a positional argument, e.g. "source"
    ///
    static std::string PositionalArgName(const PositionalArgument& posArg);

    ///
    /// Translates a positional argument's JSON definition text into a usable
    /// data structure.
    ///
    static PositionalArgumentData Translate(const PositionalArgument& posArg);

    ///
    /// Translates the JSON definitions for several positional arguments into
    /// usable data structures.
    ///
    static std::vector<PositionalArgumentData> Translate(
        const std::vector<PositionalArgument>& posArgs);
};

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_POSITIONALARGUMENTTRANSLATOR_H
