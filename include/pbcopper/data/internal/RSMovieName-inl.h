#ifndef PBCOPPER_DATA_RSMOVIENAME_INL_H
#define PBCOPPER_DATA_RSMOVIENAME_INL_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/data/RSMovieName.h>
#include <pbcopper/utility/StringUtils.h>

#include <istream>
#include <ostream>
#include <stdexcept>

#include <cassert>

namespace PacBio {
namespace Data {

struct RSMovieName::PartsCache
{
    boost::string_ref runStartTime_;
    boost::string_ref serialNumber_;
    boost::string_ref smrtCellBarcode_;
    boost::string_ref setNumber_;
    boost::string_ref partNumber_;
};

// NOTE: We're not going to re-calculate cache in copies until actually needed.
//       We can't completely re-use the original cache, since the string_refs
//       point to other string::c_str(). So we won't pay for the calculation
//       until the components are again requested. That should happen less often
//       than moving these guys around.

inline RSMovieName::RSMovieName() = default;

inline RSMovieName::RSMovieName(std::string name) : movieName_{std::move(name)} {}

inline RSMovieName::RSMovieName(const RSMovieName& other) : movieName_{other.movieName_} {}

inline RSMovieName::RSMovieName(RSMovieName&&) noexcept = default;

inline RSMovieName& RSMovieName::operator=(const RSMovieName& other)
{
    movieName_ = other.movieName_;
    partsCache_.reset(nullptr);
    return *this;
}

inline RSMovieName& RSMovieName::operator=(RSMovieName&& other) noexcept(
    std::is_nothrow_move_assignable<std::string>::value) = default;

inline RSMovieName::~RSMovieName() = default;

inline boost::string_ref RSMovieName::InstrumentSerialNumber() const
{
    if (!partsCache_) {
        UpdatePartsCache();
    }
    assert(partsCache_);
    return partsCache_->serialNumber_;
}

inline bool RSMovieName::IsReagentExpired() const
{
    const auto expiredStatus = PartNumber().at(0);
    return expiredStatus == 'X' || expiredStatus == 'x';
}

inline boost::string_ref RSMovieName::PartNumber() const
{
    if (!partsCache_) {
        UpdatePartsCache();
    }
    assert(partsCache_);
    return partsCache_->partNumber_;
}

inline boost::string_ref RSMovieName::RunStartTime() const
{
    if (!partsCache_) {
        UpdatePartsCache();
    }
    assert(partsCache_);
    return partsCache_->runStartTime_;
}

inline boost::string_ref RSMovieName::SetNumber() const
{
    if (!partsCache_) {
        UpdatePartsCache();
    }
    assert(partsCache_);
    return partsCache_->setNumber_;
}

inline boost::string_ref RSMovieName::SMRTCellBarcode() const
{
    if (!partsCache_) {
        UpdatePartsCache();
    }
    assert(partsCache_);
    return partsCache_->smrtCellBarcode_;
}

inline std::string RSMovieName::ToStdString() const { return movieName_; }

inline bool operator==(const RSMovieName& lhs, const RSMovieName& rhs) noexcept
{
    return lhs.ToStdString() == rhs.ToStdString();
}

inline bool operator!=(const RSMovieName& lhs, const RSMovieName& rhs) noexcept
{
    return !(lhs == rhs);
}

inline bool operator<(const RSMovieName& lhs, const RSMovieName& rhs) noexcept
{
    return lhs.ToStdString() < rhs.ToStdString();
}

inline std::ostream& operator<<(std::ostream& os, const RSMovieName& movieName)
{
    os << movieName.ToStdString();
    return os;
}

inline std::istream& operator>>(std::istream& is, RSMovieName& movieName)
{
    std::string s;
    is >> s;
    movieName = RSMovieName{std::move(s)};
    return is;
}

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_RSMOVIENAME_INL_H
