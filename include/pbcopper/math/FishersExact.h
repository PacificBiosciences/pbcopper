#ifndef PBCOPPER_MATH_FISHERSEXACT_H
#define PBCOPPER_MATH_FISHERSEXACT_H

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Math {

///
/// \brief Use Fisher's exact test to calculate a p-value for
///        contingency table data
///
/// \verbatim
///
///    n11  n12  | n1_
///    n21  n22  | n2_
///   -----------+----
///    n_1  n_2  | n
///
/// \endverbatim
///
/// \note implementation pulled from htslib/kfunc
///       https://github.com/samtools/htslib/blob/develop/htslib/kfunc.h
///
double FishersExact(int n11, int n12, int n21, int n22);

}  // namespace Math
}  // namespace PacBio

#endif  // PBCOPPER_MATH_FISHERSEXACT_H
