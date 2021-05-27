#ifndef PBCOPPER_DATA_ORIENTATION_H
#define PBCOPPER_DATA_ORIENTATION_H

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Data {

/// \brief This enum defines the orientations for sequence data.
///
/// Orientation::NATIVE indicates that data should be presented in the subread's
/// original form, regardless of aligned Strand.
///
/// Orientation::GENOMIC indicates that data should be presented relative to
/// genomic forward strand. This means that data will be reversed (or
/// reverse-complemented) if the subread was aligned to the reverse strand.
///
enum class Orientation
{
    NATIVE,
    GENOMIC
};

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_ORIENTATION_H
