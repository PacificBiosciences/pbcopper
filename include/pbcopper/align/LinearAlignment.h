// Author: David Alexander

//
// Linear-space basic alignment following Hirschberg
//

#ifndef PBCOPPER_ALIGN_LINEARALIGNMENT_H
#define PBCOPPER_ALIGN_LINEARALIGNMENT_H

#include <string>

#include <pbcopper/align/PairwiseAlignment.h>

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
