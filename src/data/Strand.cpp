#include <pbcopper/data/Strand.h>

#include <array>
#include <ostream>
#include <string_view>

namespace PacBio {
namespace Data {

std::ostream& operator<<(std::ostream& os, const Strand& strand)
{
    constexpr std::array<std::string_view, 3> STRAND_NAME{
        "FORWARD",
        "REVERSE",
        "UNMAPPED",
    };
    return os << STRAND_NAME[static_cast<int>(strand)];
}

}  // namespace Data
}  // namespace PacBio
