#ifndef PBCOPPER_MATH_CONFUSIONMATRIX_H
#define PBCOPPER_MATH_CONFUSIONMATRIX_H

#include <pbcopper/PbcopperConfig.h>

#include <cassert>

namespace PacBio {
namespace Math {

///
/// Provides calculations on a confusion matrix (binary classification)
///
struct ConfusionMatrix
{
    int NumTP = 0;
    int NumTN = 0;
    int NumFP = 0;
    int NumFN = 0;

    int Size() const noexcept;

    double Accuracy() const noexcept;
    double F1Score() const noexcept;
    double FalseDiscoveryRate() const noexcept;
    double FalseNegativeRate() const noexcept;
    double FalseOmissionRate() const noexcept;
    double FalsePositiveRate() const noexcept;
    double Informedness() const noexcept;
    double Precision() const noexcept;
    double NPV() const noexcept;
    double PPV() const noexcept;
    double Recall() const noexcept;
    double Sensitivity() const noexcept;
    double Specificity() const noexcept;
    double TrueNegativeRate() const noexcept;
    double TruePositiveRate() const noexcept;
};

inline double ConfusionMatrix::Accuracy() const noexcept
{
    assert(Size() > 0);
    return (1. * NumTP + NumTN) / Size();
}

inline double ConfusionMatrix::F1Score() const noexcept
{
    assert((NumTP > 0) || (NumFP > 0) || (NumFN > 0));
    return (2. * NumTP) / (2 * NumTP + NumFP + NumFN);
}

inline double ConfusionMatrix::FalseDiscoveryRate() const noexcept
{
    assert((NumFP > 0) || (NumTP > 0));
    return (1. * NumFP) / (NumFP + NumTP);
}

inline double ConfusionMatrix::FalseNegativeRate() const noexcept
{
    assert((NumFN > 0) || (NumTP > 0));
    return (1. * NumFN) / (NumFN + NumTP);
}

inline double ConfusionMatrix::FalseOmissionRate() const noexcept
{
    assert((NumFN > 0) || (NumTN > 0));
    return (1. * NumFN) / (NumFN + NumTN);
}

inline double ConfusionMatrix::FalsePositiveRate() const noexcept
{
    assert((NumFP > 0) || (NumTN > 0));
    return (1. * NumFP) / (NumFP + NumTN);
}

inline double ConfusionMatrix::Informedness() const noexcept
{
    return Sensitivity() + Specificity() - 1.0;
}

inline double ConfusionMatrix::NPV() const noexcept
{
    assert((NumTN > 0) || (NumFN > 0));
    return (1. * NumTN) / (NumTN + NumFN);
}

inline double ConfusionMatrix::Precision() const noexcept
{
    assert((NumTP > 0) || (NumFP > 0));
    return (1. * NumTP) / (NumTP + NumFP);
}

inline double ConfusionMatrix::PPV() const noexcept { return Precision(); }

inline double ConfusionMatrix::Recall() const noexcept
{
    assert((NumTP > 0) || (NumFN > 0));
    return (1. * NumTP) / (NumTP + NumFN);
}

inline double ConfusionMatrix::Sensitivity() const noexcept { return Recall(); }

inline int ConfusionMatrix::Size() const noexcept { return NumTP + NumFP + NumTN + NumFN; }

inline double ConfusionMatrix::Specificity() const noexcept
{
    assert((NumTN > 0) || (NumFP > 0));
    return (1. * NumTN) / (NumTN + NumFP);
}

inline double ConfusionMatrix::TrueNegativeRate() const noexcept { return Specificity(); }

inline double ConfusionMatrix::TruePositiveRate() const noexcept { return Recall(); }

}  // namespace Math
}  // namespace PacBio

#endif  // PBCOPPER_MATH_CONFUSIONMATRIX_H
