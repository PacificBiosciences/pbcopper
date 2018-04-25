// Author: Derek Barnett

#ifndef PBCOPPER_DATA_RSREADNAME_H
#define PBCOPPER_DATA_RSREADNAME_H

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/data/RSMovieName.h>
#include <pbcopper/data/internal/ReadNameBase.h>

namespace PacBio {
namespace Data {

typedef internal::ReadNameBase<RSMovieName> RSReadName;

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_RSREADNAME_H
