// Author: Derek Barnett

#ifndef PBCOPPER_DATA_STRAND_H
#define PBCOPPER_DATA_STRAND_H

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Data {

/// \brief This enum defines the strand orientations used for reporting
///        alignment-related information.
///
enum class Strand
{
    FORWARD,  ///< Forward strand
    REVERSE   ///< Reverse strand
};

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_STRAND_H
