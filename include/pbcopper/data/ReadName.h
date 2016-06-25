#ifndef PBCOPPER_DATA_READNAME_H
#define PBCOPPER_DATA_READNAME_H

#include "pbcopper/Config.h"

#include "pbcopper/data/MovieName.h"
#include "pbcopper/data/internal/ReadNameBase.h"

namespace PacBio {
namespace Data {

// change this to RSReadName
typedef internal::ReadNameBase<MovieName> ReadName;

} // namespace Data
} // namespace PacBio

#endif // PBCOPPER_DATA_READNAME_H
