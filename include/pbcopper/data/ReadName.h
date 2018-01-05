#ifndef PBCOPPER_DATA_READNAME_H
#define PBCOPPER_DATA_READNAME_H

#include "pbcopper/data/MovieName.h"
#include "pbcopper/data/internal/ReadNameBase.h"

namespace PacBio {
namespace Data {

typedef internal::ReadNameBase<MovieName> ReadName;

} // namespace Data
} // namespace PacBio

#endif // PBCOPPER_DATA_READNAME_H
