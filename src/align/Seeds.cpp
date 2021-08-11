#include <pbcopper/align/Seeds.h>

#include <type_traits>

namespace PacBio {
namespace Align {

bool Seeds::AddSeed(const Seed& s)
{
    data_.insert(s);
    return true;
}

bool Seeds::TryMergeSeed(const Seed& s)
{
    Seed left;
    for (auto it = data_.begin(); it != data_.end(); ++it) {
        if (CanMergeSeeds(*it, s)) {
            left = *it;
            left += s;
            data_.erase(it);
            data_.insert(left);
            return true;
        } else if (CanMergeSeeds(s, *it)) {
            left = s;
            left += *it;
            data_.erase(it);
            data_.insert(left);
            return true;
        }
    }
    return false;
}

bool CanMergeSeeds(const Seed& lhs, const Seed& rhs)
{
    // rhs must be to right of lhs AND seeds must overlap
    const auto ok = rhs.BeginPositionH() >= lhs.BeginPositionH() &&
                    rhs.BeginPositionV() >= lhs.BeginPositionV() &&
                    rhs.BeginPositionH() <= lhs.EndPositionH() &&
                    rhs.BeginPositionV() <= lhs.EndPositionV();
    return ok;
}

}  // namespace Align
}  // namespace PacBio
