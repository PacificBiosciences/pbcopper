#include <pbcopper/data/Interval.h>

#include <istream>
#include <ostream>
#include <sstream>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace PacBio {
namespace Data {

Interval::Interval(const Position val) : data_{val, val + 1} {}

Interval::Interval(const Position start, const Position end) : data_{start, end} {}

void Interval::Reset(const Position start, const Position end)
{
    assert(start <= end);
    *this = Interval{start, end};
}

void Interval::Reset(const Interval& other)
{
    assert(other.Start() <= other.End());
    *this = other;
}

bool Interval::operator==(const Interval& other) const noexcept { return data_ == other.data_; }

bool Interval::operator!=(const Interval& other) const noexcept { return !(data_ == other.data_); }

bool Interval::operator<(const Interval& other) const noexcept { return data_ < other.data_; }

bool Interval::IsEmpty() const { return Length() == 0; }

bool Interval::ContainedBy(const Interval& other) const
{
    return boost::icl::within(data_, other.data_);
}

bool Interval::Contains(const Interval& other) const
{
    return boost::icl::contains(data_, other.data_);
}

Position Interval::End() const { return data_.upper(); }

Interval& Interval::End(const Position& end)
{
    data_ = interval_type{data_.lower(), end};
    return *this;
}

bool Interval::Intersects(const Interval& other) const
{
    return boost::icl::intersects(data_, other.data_);
}

bool Interval::Overlaps(const Interval& other) const
{
    // if the left of one is in the range of the other
    return (other.Start() <= Start() && Start() <= other.End()) ||
           (Start() <= other.End() && other.Start() <= End());
}

bool Interval::IsValid() const { return !boost::icl::is_empty(data_); }

Data::Position Interval::Length() const { return boost::icl::length(data_); }

Position Interval::Start() const { return data_.lower(); }

Interval& Interval::Start(const Position& start)
{
    data_ = interval_type{start, data_.upper()};
    return *this;
}

Interval Interval::Union(const Interval& other) const
{
    if (Overlaps(other)) {
        return {std::min(Start(), other.Start()), std::max(End(), other.End())};
    } else {
        std::ostringstream msg;
        msg << "[pbcopper] interval ERROR: cannot merge intervals " << *this << " and " << other
            << ", because they do not overlap.";
        throw std::invalid_argument{msg.str()};
    }
}

Interval Interval::Intersect(const Interval& other) const
{
    if (Overlaps(other)) {
        return {std::max(Start(), other.Start()), std::min(End(), other.End())};
    } else {
        std::ostringstream msg;
        msg << "[pbcopper] interval ERROR: cannot create the intersection of intervals " << *this
            << " and " << other << ", because they do not overlap.";
        throw std::invalid_argument{msg.str()};
    }
}

Interval Interval::FromString(const std::string& str)
{
    try {
        std::vector<std::string> components;
        boost::split(components, str, boost::is_any_of("-"));
        if (components.size() == 1) {
            Position left = boost::lexical_cast<Position>(components[0]);
            return Interval(left, left + 1);
        } else if (components.size() == 2) {
            Position left = boost::lexical_cast<Position>(components[0]);
            Position right = boost::lexical_cast<Position>(components[1]);
            // if right < left, we have an invalid interval, fall through
            if (left <= right) {
                return Interval(left, right + 1);
            }
        }
    } catch (...) {
    }

    throw std::invalid_argument("[pbcopper] interval ERROR: cannot create an interval from '" +
                                str + "'");
}

Interval Interval::begin() const { return Interval(Start(), End()); }

Interval Interval::end() const { return Interval(End(), End()); }

Interval& Interval::operator++()
{
    Position newStart = Start();
    ++newStart;
    return Start(newStart);
}

Interval Interval::operator++(int)
{
    Position newStart = Start();
    return Interval{newStart++, End()};
}

Position Interval::operator*() const { return Start(); }

std::ostream& operator<<(std::ostream& os, const Interval& interval)
{
    return os << '[' << interval.Start() << ", " << interval.End() << ')';
}

std::istream& operator>>(std::istream& is, Interval& interval)
{
    char delim;
    Position start{}, end{};
    is >> std::ws >> delim >> start >> delim >> std::ws >> end >> delim;
    assert(is.good() && (start <= end));

    interval.Start(start);
    interval.End(end);

    return is;
}

}  // namespace Data
}  // namespace PacBio
