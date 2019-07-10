#ifndef PBCOPPER_DATA_CLIPPING_IMPL_H
#define PBCOPPER_DATA_CLIPPING_IMPL_H

#include <cstddef>

#include <pbcopper/data/Clipping.h>

namespace PacBio {
namespace Data {

class SimpleRead;
class MappedSimpleRead;

namespace internal {

void ClipSimpleRead(SimpleRead& read, const ClipResult& result, size_t start, size_t end);

// NOTE: 'result' is moved into here, so we can take the CIGAR
void ClipMappedRead(MappedSimpleRead& read, ClipResult result);

}  // namespace internal
}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_CLIPPING_IMPL_H