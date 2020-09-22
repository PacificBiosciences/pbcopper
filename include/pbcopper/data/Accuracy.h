// Author: Derek Barnett

#ifndef PBCOPPER_DATA_ACCURACY_H
#define PBCOPPER_DATA_ACCURACY_H

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Data {

/// \brief The Accuracy class represents the expected accuracy of a BamRecord.
///
/// Values are clamped to fall within [0,1] or -1 as a special case for unpolished CCS reads.
///
class Accuracy
{
public:
    static const float MINUS_ONE;  ///< Unpolished accuracy value [-1.0]
    static const float MIN;        ///< Minimum valid accuracy value [0.0]
    static const float MAX;        ///< Maximum valid accuracy value [1.0]

public:
    /// \name Constructors & Related Methods
    /// \{

    /// Constructs an Accuracy object from a floating-point number.
    ///
    /// \note This is not an \b explicit ctor, to make it as easy as
    ///       possible to use in numeric operations. We really just want
    ///       to make sure that the acceptable range is respected.
    ///
    Accuracy(float accuracy);

    /// \}

public:
    /// \returns Accuracy as float primitive
    operator float() const noexcept;

private:
    float accuracy_;
};

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_ACCURACY_H
