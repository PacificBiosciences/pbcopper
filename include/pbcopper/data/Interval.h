// Author: Derek Barnett

#ifndef PBCOPPER_DATA_INTERVAL_H
#define PBCOPPER_DATA_INTERVAL_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/data/Position.h>

#define BOOST_ICL_USE_STATIC_BOUNDED_INTERVALS
#include <boost/icl/discrete_interval.hpp>
#include <boost/icl/interval_traits.hpp>

namespace PacBio {
namespace Data {

/// \brief Represents a half-open (right-open) interval [start, end)
///
/// \note This class is agnostic whether the values are 0-based or 1-based.
///

class Interval
{
public:
    using interval_type = boost::icl::discrete_interval<Data::Position>;

public:
    /// \name Constructors & Related Methods
    /// \{

    /// \brief Creates an empty interval [0,0)
    Interval() = default;

    /// \brief Creates a 'singleton' interval [val,val+1)
    Interval(const Position val);

    /// \brief Creates an interval from [start, end) */
    Interval(const Position start, const Position end);

    /// \brief Creates an interval from a string "start-end" */
    static Interval FromString(const std::string& str);

    /// \brief Reset Interval */
    void Reset(const Position start, const Position end);

    /// \brief Reset Interval */
    void Reset(const Interval& other);

    /// \}

public:
    /// \name Comparison Operators
    /// \{

    /// \returns true if both intervals share the same endpoints
    bool operator==(const Interval& other) const;

    /// \returns true if either interval's endpoints differ
    bool operator!=(const Interval& other) const;

    /// \returns true if this can be ordered before other
    bool operator<(const Interval& other) const;

    /// \}

public:
    /// \name Attributes
    /// \{

    /// \returns interval's start coordinate
    Position Start() const;

    /// Sets this interval's start coordinate.
    ///
    /// \param[in] start
    /// \returns reference to this interval
    ///
    Interval& Start(const Position& start);

    /// \returns interval's end coordinate
    Position End() const;

    /// Sets this interval's end coordinate.
    ///
    /// \param[in] end
    /// \returns reference to this interval
    ///
    Interval& End(const Position& end);

    /// \}

public:
    /// \name Interval Operations

    /// \returns true the length of the interval is 0
    bool IsEmpty() const;

    /// \returns true if this interval is fully covered by (or contained in) \p other
    bool ContainedBy(const Interval& other) const;

    //// \returns true if this interval contains (or covers) \p other
    bool Contains(const Interval& other) const;

    /// \returns true if intervals intersect
    bool Intersects(const Interval& other) const;

    /// \returns true if intervals intersect or are exactly adjacent
    bool Overlaps(const Interval& other) const;

    /// \returns true if interval is valid (e.g. start < stop)
    bool IsValid() const;

    /// \returns interval length
    Data::Position Length() const;

    /// \}

public:
    /// \name Interval Iterators

    /// \returns interval begin iterator
    Interval begin() const;

    /// \returns interval end iterator
    Interval end() const;

    /// \}

public:
    /// \name Increment operators

    /// \returns Postfix increment
    Interval& operator++();

    /// \returns Prefix increment
    Interval operator++(int);

    /// \returns Dereference operator
    Position operator*() const;

public:
    /// \name Interval Operations

    /// \returns create the union of overlapping intervals
    Interval Union(const Interval& other) const;

    /// \returns create the intersection of overlapping intervals
    Interval Intersect(const Interval& other) const;

    /// \returns write to output stream
    friend std::ostream& operator<<(std::ostream& os, const Interval& interval);

    /// \returns load interval from input stream
    friend std::istream& operator>>(std::istream& is, Interval& interval);

    /// \}

private:
    interval_type data_;
};

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_INTERVAL_H
