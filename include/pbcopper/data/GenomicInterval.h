// Author: Derek Barnett

#ifndef PBCOPPER_DATA_GENOMICINTERVAL_H
#define PBCOPPER_DATA_GENOMICINTERVAL_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/data/Interval.h>
#include <pbcopper/data/Position.h>

#include <cstddef>
#include <string>

namespace PacBio {
namespace Data {

/// \brief The GenomicInterval class represents a genomic interval (reference
///        name and 0-based coordinates).
///
class GenomicInterval
{
public:
    /// \name Constructors & Related Methods
    ///  \{

    /// \brief Creates an empty genomic interval
    GenomicInterval() = default;

    /// \brief Creates a genomic interval on sequence with \p name, using range:
    ///       [\p start, \p stop)
    GenomicInterval(std::string name, Position start, Position stop);

    /// \brief Creates a genomic interval, using REGION string
    ///
    /// "<ref>:<start>-<stop>" ("chr8:200-600")
    ///
    /// \note The htslib/samtools REGION string expects start positions to be
    ///       1-based. However, throughout pbbam (including the rest of this
    ///       class), we stick to 0-based start coordinates. Thus, while the
    ///       syntax matches that of samtools, we are using a 0-based start
    ///       coordinate here.
    ///
    GenomicInterval(const std::string& zeroBasedRegionString);

    /// \}

public:
    /// \name Comparison Operators
    /// \{

    /// \returns true if same id & underlying interval
    bool operator==(const GenomicInterval& other) const noexcept;

    /// \returns true if either ids or underlying intervals differ
    bool operator!=(const GenomicInterval& other) const noexcept;

    /// \}

public:
    /// \name Interval Operations
    /// \{

    /// \returns true if same id and underlying Interval::ContainedBy() other.
    bool ContainedBy(const GenomicInterval& other) const;

    /// \returns true if same id and underlying Interval::Contains() other.
    bool Contains(const GenomicInterval& other) const;

    /// \returns true if same id and underlying Interval::Intersects() other.
    bool Intersects(const GenomicInterval& other) const;

    /// \returns true if underlying Interval::IsValid(), and id/endpoints are
    ///          non-negative.
    ///
    bool IsValid() const;

    /// \returns length of underlying
    size_t Length() const;

    /// \}

public:
    /// \name Attributes
    /// \{

    /// \returns interval reference name
    std::string Name() const;

    /// \returns underlying Interval object
    class Interval Interval() const;

    /// \returns interval start coordinate
    Position Start() const;

    /// \returns interval stop coordinate
    Position Stop() const;

    /// \}

public:
    /// \name Attributes
    /// \{

    /// Sets this interval's reference name.
    ///
    /// \param[in] name
    /// \returns reference to this interval
    ///
    GenomicInterval& Name(std::string name);

    /// Sets this underlying Interval
    ///
    /// \param[in] interval
    /// \returns reference to this interval
    ///
    GenomicInterval& Interval(class Interval interval);

    /// Sets this interval's start coordinate.
    ///
    /// \param[in] start
    /// \returns reference to this interval
    ///
    GenomicInterval& Start(const Position start);

    /// Sets this interval's stop coordinate.
    ///
    /// \param[in] stop
    /// \returns reference to this interval
    ///
    GenomicInterval& Stop(const Position stop);

    /// \}

private:
    std::string name_;
    class Interval interval_;
};

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_GENOMICINTERVAL_H
