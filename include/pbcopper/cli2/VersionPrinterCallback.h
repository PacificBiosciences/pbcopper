#ifndef PBCOPPER_CLI_v2_VERSIONPRINTERCALLBACK_H
#define PBCOPPER_CLI_v2_VERSIONPRINTERCALLBACK_H

#include <pbcopper/PbcopperConfig.h>

#include <functional>

namespace PacBio {
namespace CLI_v2 {

class Interface;
class MultiToolInterface;

///
/// Allows an application interface to override the default version display.
///
using VersionPrinterCallback = std::function<void(const Interface&)>;

///
/// Allows a multi-tool application interface to override the default version display.
///
using MultiToolVersionPrinterCallback = std::function<void(const MultiToolInterface&)>;

}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_VERSIONPRINTERCALLBACK_H
