#ifndef PBCOPPER_DAGCON_ALIGNMENT_H
#define PBCOPPER_DAGCON_ALIGNMENT_H

#include <pbcopper/PbcopperConfig.h>

#include <cstdint>

#include <string>

namespace PacBio {
namespace Dagcon {

///
/// Super-simple alignment representation.  Represents an alignment between two
/// PacBio reads, one of which we're trying to correct.  The read to correct
/// may be either the target or the query, depending on how the alignment was
/// done.
///
struct Alignment
{
    Alignment() = default;  // icpc

    // length of the sequence we are trying to correct
    uint32_t TargetLength = 0;

    // conforming offsets are 1-based
    uint32_t Start = 0;
    uint32_t End = 0;

    // ID of the read we're trying to correct (target)
    std::string Id;

    // ID of the supporting read (query)
    std::string SId;

    // strand: '+' or '-'
    char Strand = '+';

    // query and target strings must be equal length
    std::string Query;
    std::string Target;
};

///
/// Simplifies the alignment by normalizing gaps.  Converts mismatches into
/// indels ...
///
///      query: CAC        query:  C-AC
///             | |  --->          |  |
///     target: CGC       target:  CG-C
///
/// Shifts equivalent gaps to the right in the reference ...
///      query: CAACAT        query: CAACAT
///             | | ||  --->         |||  |
///     target: C-A-AT       target: CAA--T
///
/// Shifts equivalent gaps to the right in the read ...
///      query: -C--CGT       query: CCG--T
///              |  | |  --->        |||  |
///     target: CCGAC-T      target: CCGACT
///
/// Allow optional gap pushing, some aligners may not need it and I'd like
/// to get rid of it anyway.
///
void NormalizeGaps(Alignment& alignment, bool push = true);

///
/// Trim \p length bases from each of end of \p alignment 's target sequence,
/// along with the appropriate query bases.
///
void TrimAlignment(Alignment& alignment, int trimLength = 50);

}  // namespace Dagcon
}  // namespace PacBio

#endif  // PBCOPPER_DAGCON_ALIGNMENT_H
