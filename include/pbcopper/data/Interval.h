#ifndef PBCOPPER_DATA_INTERVAL_H
#define PBCOPPER_DATA_INTERVAL_H

#include <cstddef>

#define BOOST_ICL_USE_STATIC_BOUNDED_INTERVALS
#include <boost/icl/discrete_interval.hpp>
#include <boost/icl/interval_traits.hpp>

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Data {

/// \brief Represents a half-open (right-open) interval [start, end)
///
/// \note This class is agnostic whether the values are 0-based or 1-based.
///
template<typename T>
class Interval
{
public:
    typedef boost::icl::discrete_interval<T> interval_type;

public:
    /// \name Constructors & Related Methods
    /// \{

    /// \brief Creates an empty interval [0,0)
    Interval(void);

    /// \brief Creates a 'singleton' interval [val,val+1)
    Interval(const T val);

    /// \brief Creates an interval from [start, end) */
    Interval(const T start, const T end);

    /// \brief Copy constructs an interval
    Interval(const Interval<T>& other);

    /// \brief Move constructs an interval
    Interval(Interval<T>&& other);

    Interval<T>& operator=(const Interval<T>& other) = default;
    Interval<T>& operator=(Interval<T>&& other) = default;
    ~Interval<T>(void) = default;

    /// \}

public:
    /// \name Comparison Operators
    /// \{

    /// \returns true if both intervals share the same endpoints
    bool operator==(const Interval<T>& other) const;

    /// \returns true if either interval's endpoints differ
    bool operator!=(const Interval<T>& other) const;

    /// \}

public:
    /// \name Attributes
    /// \{

    /// \returns interval's start coordinate
    T Start(void) const;

    /// Sets this interval's start coordinate.
    ///
    /// \param[in] start
    /// \returns reference to this interval
    ///
    Interval<T>& Start(const T& start);

    /// \returns interval's end coordinate
    T End(void) const;

    /// Sets this interval's end coordinate.
    ///
    /// \param[in] end
    /// \returns reference to this interval
    ///
    Interval<T>& End(const T& end);

    /// \}

public:
    /// \name Interval Operations

    /// \returns true if this interval is fully covered by (or contained in) \p other
    bool CoveredBy(const Interval<T>& other) const;

    //// \returns true if this interval covers (or contains) \p other
    bool Covers(const Interval<T>& other) const;

    /// \returns true if intervals interset
    bool Intersects(const Interval<T>& other) const;

    /// \returns true if interval is valid (e.g. start < stop)
    bool IsValid(void) const;

    /// \returns interval length
    size_t Length(void) const;

    /// \}

private:
    interval_type data_;
};

} // namespace Data
} // namespace PacBio

#include "pbcopper/data/internal/Interval-inl.h"

#endif // PBCOPPER_DATA_INTERVAL_H
