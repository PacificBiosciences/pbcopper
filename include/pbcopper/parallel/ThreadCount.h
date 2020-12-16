#ifndef PBCOPPER_PARALLEL_THREADCOUNT_H
#define PBCOPPER_PARALLEL_THREADCOUNT_H

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Parallel {

///
/// \brief Returns a "normalized" thread count - clamping the requested thread
///        count between 1 and the number of concurrent threads supported.
///
/// Set \p requestedNumThreads to zero for "auto-detection", i.e. use the hardware
/// max.
///
/// \param requestedNumThreads      desired thread count
///
unsigned int NormalizedThreadCount(unsigned int requestedNumThreads);

}  // namespace Parallel
}  // namespace PacBio

#endif  // PBCOPPER_PARALLEL_THREADCOUNT_H
