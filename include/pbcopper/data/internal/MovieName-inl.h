// Author: Derek Barnett

#ifndef PBCOPPER_DATA_MOVIENAME_INL_H
#define PBCOPPER_DATA_MOVIENAME_INL_H

#include <cassert>
#include <iostream>
#include <stdexcept>

#include <pbcopper/data/MovieName.h>
#include <pbcopper/utility/StringUtils.h>

namespace PacBio {
namespace Data {

struct MovieName::PartsCache
{
    boost::string_ref instrumentName_;
    boost::string_ref runStartTime_;
};

// NOTE: We're not going to re-calculate cache in copies until actually needed.
//       We can't completely re-use the original cache, since the string_refs
//       point to other string::c_str(). So we won't pay for the calculation
//       until the components are again requested. That should happen less often
//       than moving these guys around.

inline MovieName::MovieName() = default;

inline MovieName::MovieName(std::string name) : movieName_{std::move(name)} {}

inline MovieName::MovieName(const MovieName& other) : movieName_{other.movieName_} {}

inline MovieName::MovieName(MovieName&&) noexcept = default;

inline MovieName& MovieName::operator=(const MovieName& other)
{
    movieName_ = other.movieName_;
    partsCache_.reset(nullptr);
    return *this;
}

inline MovieName& MovieName::operator=(MovieName&&) noexcept(
    std::is_nothrow_move_assignable<std::string>::value) = default;

inline MovieName::~MovieName() = default;

inline boost::string_ref MovieName::InstrumentName() const
{
    if (!partsCache_) UpdatePartsCache();
    assert(partsCache_);
    return partsCache_->instrumentName_;
}

inline boost::string_ref MovieName::RunStartTime() const
{
    if (!partsCache_) UpdatePartsCache();
    assert(partsCache_);
    return partsCache_->runStartTime_;
}

inline std::string MovieName::ToStdString() const { return movieName_; }

inline bool operator==(const MovieName& lhs, const MovieName& rhs)
{
    return lhs.ToStdString() == rhs.ToStdString();
}

inline bool operator!=(const MovieName& lhs, const MovieName& rhs) { return !(lhs == rhs); }

inline bool operator<(const MovieName& lhs, const MovieName& rhs)
{
    return lhs.ToStdString() < rhs.ToStdString();
}

inline std::ostream& operator<<(std::ostream& os, const MovieName& movieName)
{
    os << movieName.ToStdString();
    return os;
}

inline std::istream& operator>>(std::istream& is, MovieName& movieName)
{
    std::string s;
    is >> s;
    movieName = MovieName{std::move(s)};
    return is;
}

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_MOVIENAME_INL_H
