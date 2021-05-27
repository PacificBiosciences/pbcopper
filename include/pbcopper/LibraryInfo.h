#ifndef PBCOPPER_LIBRARYINFO_H
#define PBCOPPER_LIBRARYINFO_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/library/Bundle.h>
#include <pbcopper/library/Info.h>

namespace PacBio {
namespace Pbcopper {

///
/// \return pbcopper library info (e.g. name, version)
///
Library::Info LibraryInfo();

///
/// \returns bundle of pbcopper library info, plus its dependencies
///
Library::Bundle LibraryBundle();

///
/// \return boost library info (pbcopper dependency)
///
Library::Info BoostLibraryInfo();

}  // namespace Pbcopper
}  // namespace PacBio

#endif  // PBCOPPER_LIBRARYINFO_H
