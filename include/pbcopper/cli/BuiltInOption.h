#ifndef PBCOPPER_CLI_BUILTINOPTION_H
#define PBCOPPER_CLI_BUILTINOPTION_H

namespace PacBio {
namespace CLI {

///
/// \brief The BuiltInOption enum
///
enum class BuiltInOption
{
    Help
  , Version
  , Verbose
  , EmitToolContract
  , ResolvedToolContract
};

} // namespace CLI
} // namespace PacBio

#endif // PBCOPPER_CLI_BUILTINOPTION_H
