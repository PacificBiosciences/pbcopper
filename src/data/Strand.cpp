#include <pbcopper/data/Strand.h>

#include <array>
#include <ostream>

namespace PacBio {
namespace Data {

std::ostream& operator<<(std::ostream& os, const Strand& strand)
{
    constexpr std::array<const char*, 3> StrandName{
        "FORWARD", "REVERSE", "UNMAPPED",
    };
    return os << StrandName[static_cast<int>(strand)];
}

}  // namespace Data
}  // namespace PacBio
