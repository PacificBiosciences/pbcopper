#ifndef PBCOPPER_DATA_QUALITYVALUE_H
#define PBCOPPER_DATA_QUALITYVALUE_H

#include <pbcopper/PbcopperConfig.h>

#include <string>
#include <vector>

#include <cstdint>

namespace PacBio {
namespace Data {

/// \brief The QualityValue class represents a FASTQ-compatible quality value.
///
/// Integers are clamped to [0, 93] (corresponding to ASCII printable chars
/// [!-~]).
///
/// Use QualityValue::FromFastq for constructing entries from FASTQ encoding
/// characters. Otherwise, the resulting QualityValue will be interpreted using
/// the character's numeric value (ignoring the FASTQ offset of 33).
///
class QualityValue
{
public:
    static const std::uint8_t MAX;

public:
    /// \name Conversion Methods
    /// \{

    /// \brief Creates a QualityValue from a FASTQ-encoding character.
    ///
    /// \param[in] c    FASTQ character
    /// \returns quality value representing (c - 33)
    ///
    static QualityValue FromFastq(char c);

    /// \}

public:
    /// \name Constructors & Related Methods
    ///  \{

    /// \brief Creates a QualityValue with specified value.
    ///
    /// \param[in] value    quality value
    ///
    QualityValue(std::uint8_t value);
    QualityValue() = default;

    /// \}

public:
    /// \name Conversion Methods
    /// \{

    /// \returns the FASTQ-encoding char for this QualityValue
    char Fastq() const;

    /// \returns the integer value of this QualityValue
    operator std::uint8_t() const noexcept;

    /// \}

private:
    std::uint8_t value_ = 0;
};

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_QUALITYVALUE_H
