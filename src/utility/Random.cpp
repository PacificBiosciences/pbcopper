#include <pbcopper/utility/Random.h>
#include <cstdint>

namespace PacBio {
namespace Utility {

uint64_t WyHash64Step(uint64_t& seed) noexcept
{
    seed += UINT64_C(0x60bee2bee120fc15);
    __uint128_t l = seed ^ 0xe7037ed1a0b428dbull;
    l *= seed;
    return l ^ (l >> 64);
}

}  // namespace Utility
}  // namespace PacBio
