#include <pbcopper/math/FishersExact.h>

#include <pbcopper/third-party/htslib/kfunc.h>

namespace PacBio {
namespace Math {

double FishersExact(int n11, int n12, int n21, int n22)
{
    double left = 0.0;
    double right = 0.0;
    double two = 0.0;
    return kt_fisher_exact(n11, n12, n21, n22, &left, &right, &two);
}

}  // namespace Math
}  // namespace PacBio
