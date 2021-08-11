#ifndef PBCOPPER_ALIGN_AFFINEALIGNMENT_H
#define PBCOPPER_ALIGN_AFFINEALIGNMENT_H

#include <string>

namespace PacBio {
namespace Align {

//
// Support for pairwise alignment with an affine gap penalty.
//

class PairwiseAlignment;

struct AffineAlignmentParams
{
    float MatchScore;
    float MismatchScore;
    float GapOpen;
    float GapExtend;
    float PartialMatchScore;

    AffineAlignmentParams(float matchScore, float mismatchScore, float gapOpen, float gapExtend,
                          float partialMatchScore = 0);
};

AffineAlignmentParams DefaultAffineAlignmentParams();
AffineAlignmentParams IupacAwareAffineAlignmentParams();

//
// Affine gap-penalty alignment.
//
PairwiseAlignment* AlignAffine(
    const std::string& target, const std::string& query,
    AffineAlignmentParams params = DefaultAffineAlignmentParams());  // NOLINT

//
// Affine gap-penalty alignment with partial awareness of IUPAC ambiguous bases---
// half-penalizes partial mismatches.  For example:  (M = IUPAC A/C)
//   T->A = -1,
//   T->M = -1,
//   A->M = -0.5
//
PairwiseAlignment* AlignAffineIupac(
    const std::string& target, const std::string& query,
    AffineAlignmentParams params = IupacAwareAffineAlignmentParams());  // NOLINT

}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_AFFINEALIGNMENT_H
