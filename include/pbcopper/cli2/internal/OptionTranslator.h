#ifndef PBCOPPER_CLI_v2_OPTIONTRANSLATOR_H
#define PBCOPPER_CLI_v2_OPTIONTRANSLATOR_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/cli2/Option.h>
#include <pbcopper/cli2/OptionValue.h>
#include <pbcopper/cli2/internal/OptionData.h>

#include <string>
#include <vector>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

///
/// This translator is responsible for converting JSON definitions into option properties
///
struct OptionTranslator
{
    ///
    /// Fetch names only from option, e.g. {"r," "reference-file"}
    ///
    static std::vector<std::string> OptionNames(const Option& option);

    ///
    /// Translates an option's JSON definition into a usable data structure
    ///
    static OptionData Translate(const Option& opt);

    ///
    /// Translates the JSON definitions for several options into usable data structures
    ///
    static std::vector<OptionData> Translate(const std::vector<Option>& options);
};

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_OPTIONTRANSLATOR_H
