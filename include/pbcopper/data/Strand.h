#ifndef PBCOPPER_DATA_STRAND_H
#define PBCOPPER_DATA_STRAND_H

#include <pbcopper/PbcopperConfig.h>

#include <cstdint>

#include <iosfwd>

namespace PacBio {
namespace Data {

/// \brief This enum defines the strand orientations used for reporting
///        alignment-related information.
///
enum class Strand : uint8_t
{
    FORWARD,  ///< Forward strand
    REVERSE,  ///< Reverse strand
    UNMAPPED,
};

std::ostream& operator<<(std::ostream& os, const Strand& strand);

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_STRAND_H
