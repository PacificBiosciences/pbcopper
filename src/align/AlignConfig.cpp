// Authors: David Alexander, Lance Hepler

#include <pbcopper/align/AlignConfig.h>

namespace PacBio {
namespace Align {

AlignParams AlignParams::Default() { return {0, -1, -1, -1}; }

AlignConfig AlignConfig::Default() { return {AlignParams::Default(), AlignMode::GLOBAL}; }

}  // namespace Align
}  // namespace PacBio
