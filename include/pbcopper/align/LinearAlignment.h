#ifndef PBCOPPER_ALIGN_LINEARALIGNMENT_H
#define PBCOPPER_ALIGN_LINEARALIGNMENT_H

//
// Linear-space basic alignment following Hirschberg
//

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/align/PairwiseAlignment.h>

#include <string>

namespace PacBio {
namespace Align {

class PairwiseAlignment;

PairwiseAlignment* AlignLinear(const std::string& target, const std::string& query,
                               AlignConfig config = AlignConfig::Default());

PairwiseAlignment* AlignLinear(const std::string& target, const std::string& query, int* score,
                               AlignConfig config = AlignConfig::Default());

}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_LINEARALIGNMENT_H
