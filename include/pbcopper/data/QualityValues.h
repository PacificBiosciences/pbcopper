#ifndef PBCOPPER_DATA_QUALITYVALUES_H
#define PBCOPPER_DATA_QUALITYVALUES_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/data/QualityValue.h>

#include <iosfwd>
#include <string>
#include <vector>

#include <cstdint>

namespace PacBio {
namespace Data {

/// \brief The QualityValues class represents a sequence of FASTQ-compatible
///        quality values. See QualityValue documentation for more details.
///
class QualityValues : public std::vector<QualityValue>
{
public:
    /// \brief Creates a QualityValues object from a FASTQ-encoded string.
    ///
    /// \param[in] fastq    FASTQ-encoded string
    /// \returns corresponding QualityValues object
    ///
    static QualityValues FromFastq(const std::string& fastq);

public:
    /// \name Constructors & Related Methods
    ///  \{

    /// \brief Default constructor - creates an empty QualityValues object.

    /// \brief Creates a QualityValues object from a FASTQ-encoded string.
    ///
    /// \param[in] fastqString  FASTQ-encoded string
    ///
    explicit QualityValues(const std::string& fastqString);

    /// \brief Creates a QualityValues object from a vector of QualityValue
    ///        elements.
    ///
    /// \param[in] quals    vector of QualityValue elements
    ///
    QualityValues(std::vector<QualityValue> quals);

    /// \brief Creates a QualityValues object from a vector of (numeric) quality
    ///        values.
    ///
    /// \param[in] quals    vector of quality value numbers
    ///
    explicit QualityValues(const std::vector<std::uint8_t>& quals);

    /// \brief Creates a QualityValues object from the contents of the range:
    ///        [first, last)
    ///
    /// \param[in] first    input iterator, whose element is a numeric quality
    /// \param[in] last     input iterator, whose element is a numeric quality
    ///
    QualityValues(std::vector<std::uint8_t>::const_iterator first,
                  std::vector<std::uint8_t>::const_iterator last);

    /// \brief Creates a QualityValues object from the contents of the range:
    ///        [first, last)
    ///
    /// \param[in] first    input iterator, whose element is a QualityValue
    /// \param[in] last     input iterator, whose element is a QualityValue
    ///
    QualityValues(QualityValues::const_iterator first, QualityValues::const_iterator last);

    QualityValues() = default;

    QualityValues& operator=(std::vector<QualityValue> quals) noexcept;

    /// \}

public:
    /// \name Comparison Operators
    /// \{

    bool operator==(const std::string& other) const noexcept;
    bool operator!=(const std::string& other) const noexcept;

    /// \}

public:
    /// \name Iterators
    /// \{

    /// \returns a const_iterator to the beginning of the sequence
    std::vector<QualityValue>::const_iterator cbegin() const;

    /// \returns a const_iterator to the element following the last element
    std::vector<QualityValue>::const_iterator cend() const;

    /// \returns a const_iterator to the beginning of the sequence
    std::vector<QualityValue>::const_iterator begin() const;

    /// \returns a const_iterator to the element following the last element
    std::vector<QualityValue>::const_iterator end() const;

    /// \returns an iterator to the beginning of the sequence
    std::vector<QualityValue>::iterator begin();

    /// \returns an iterator to the element following the last element
    std::vector<QualityValue>::iterator end();

    /// \}

public:
    /// \name Conversion Methods
    /// \{

    /// \returns the FASTQ-encoded string for this sequence of quality values
    std::string Fastq() const;

    /// \}
};

std::ostream& operator<<(std::ostream& os, const QualityValues& qualityvalues);

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_QUALITYVALUES_H
