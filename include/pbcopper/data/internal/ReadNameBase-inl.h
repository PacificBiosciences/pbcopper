// Author: Derek Barnett

#ifndef PBCOPPER_DATA_READNAMEBASE_INL_H
#define PBCOPPER_DATA_READNAMEBASE_INL_H

#include <cassert>

#include <istream>
#include <ostream>
#include <stdexcept>
#include <type_traits>

#include <pbcopper/data/internal/ReadNameBase.h>
#include <pbcopper/utility/StringUtils.h>

namespace PacBio {
namespace Data {
namespace internal {

template <typename MovieNameType>
ReadNameBase<MovieNameType>::ReadNameBase(const std::string& name)
{
    auto copy = std::string{name};
    FromString(std::move(copy));
    Check();
}

template <typename MovieNameType>
ReadNameBase<MovieNameType>::ReadNameBase(std::string&& name)
{
    FromString(std::move(name));
    Check();
}

template <typename MovieNameType>
ReadNameBase<MovieNameType>::ReadNameBase(const MovieNameType& movieName,
                                          const PacBio::Data::Zmw& zmw,
                                          const Interval& queryInterval)
    : movieName_(movieName), zmw_(zmw), queryInterval_(new Interval{queryInterval})
{
}

template <typename MovieNameType>
ReadNameBase<MovieNameType>::ReadNameBase(const MovieNameType& movieName,
                                          const PacBio::Data::Zmw& zmw, const Position& queryStart,
                                          const Position& queryEnd)
    : ReadNameBase<MovieNameType>(movieName, zmw, Interval(queryStart, queryEnd))
{
}

template <typename MovieNameType>
ReadNameBase<MovieNameType>::ReadNameBase(const MovieNameType& movieName,
                                          const PacBio::Data::Zmw& zmw, const CCSTag)
    : movieName_(movieName), zmw_(zmw), queryInterval_(nullptr)
{
}

template <typename MovieNameType>
bool ReadNameBase<MovieNameType>::IsCCS() const
{
    return !queryInterval_;
}

template <typename MovieNameType>
MovieNameType ReadNameBase<MovieNameType>::MovieName() const
{
    return movieName_;
}

template <typename MovieNameType>
Interval ReadNameBase<MovieNameType>::QueryInterval() const
{
    if (IsCCS()) {
        throw std::runtime_error{
            "[pbcopper] read name ERROR: cannot get query interval from CCS read"};
    }
    return *queryInterval_;
}

template <typename MovieNameType>
Position ReadNameBase<MovieNameType>::QueryStart() const
{
    if (IsCCS()) {
        throw std::runtime_error{
            "[pbcopper] read name ERROR: cannot get query start from CCS read"};
    }
    return queryInterval_->Start();
}

template <typename MovieNameType>
Position ReadNameBase<MovieNameType>::QueryEnd() const
{
    if (IsCCS()) {
        throw std::runtime_error{"[pbcopper] read name ERROR: cannot get query end from CCS read"};
    }
    return queryInterval_->End();
}

template <typename MovieNameType>
PacBio::Data::Zmw ReadNameBase<MovieNameType>::Zmw() const
{
    return zmw_;
}

template <typename MovieNameType>
bool ReadNameBase<MovieNameType>::operator==(const ReadNameBase<MovieNameType>& other) const
    noexcept
{
    // simple int check first
    if (zmw_ != other.zmw_) {
        return false;
    }

    // not equal if one is CCS and one is not
    if (IsCCS() != other.IsCCS()) {
        return false;
    } else {
        // reads share same CCS status, check intervals if both non-CCS
        if (!IsCCS()) {
            assert(queryInterval_);
            assert(other.queryInterval_);
            if (*queryInterval_ != *other.queryInterval_) {
                return false;
            }
        }
    }

    // else compare on movie
    return movieName_ == other.movieName_;
}

template <typename MovieNameType>
bool ReadNameBase<MovieNameType>::operator<(const ReadNameBase<MovieNameType>& other) const noexcept
{
    // sort by:
    //   1 - movie name
    //   2 - zmw hole number
    //   3 - query_interval (CCS to end)

    if (movieName_ != other.movieName_) {
        return movieName_ < other.movieName_;
    }
    if (zmw_ != other.zmw_) {
        return zmw_ < other.zmw_;
    }

    // if this read name is CCS, then we're either never less-than a subread
    // with query interval... or the other read is the same read name, so not
    // "less than" either
    if (IsCCS()) {
        return false;

        // this read is non-CCS. so if other is CCS, then we are always "less-than"
    } else if (other.IsCCS()) {
        return true;
    }

    // if here, both non-CCS, compare on query interval
    assert(!IsCCS());
    assert(!other.IsCCS());
    const auto thisInterval = *queryInterval_;
    const auto otherInterval = *other.queryInterval_;
    if (thisInterval.Start() == otherInterval.Start()) {
        return thisInterval.End() < otherInterval.End();
    }
    return thisInterval.End() < otherInterval.End();
}

template <typename MovieNameType>
void ReadNameBase<MovieNameType>::Check() const
{
    if (movieName_.ToStdString().empty()) {
        throw std::runtime_error{"[pbcopper] read name ERROR: movie name must not be empty"};
    }

    if (zmw_ < 0) {
        throw std::runtime_error{
            "[pbcopper] read name ERROR: ZMW hole number must be a positive integer"};
    }

    if (!IsCCS()) {
        if (QueryStart() < 0) {
            throw std::runtime_error{
                "[pbcopper] read name ERROR: query start must be a positive integer"};
        }
        if (QueryEnd() < 0) {
            throw std::runtime_error{
                "[pbcopper] read name ERROR: query end must be a positive integer"};
        }
    }
}

template <typename MovieNameType>
void ReadNameBase<MovieNameType>::FromString(std::string&& name)
{
    // ensure clean slate
    movieName_ = MovieNameType{};
    zmw_ = -1;
    queryInterval_.reset(nullptr);

    // parse name parts
    if (name.empty()) {
        return;
    }
    auto parts = PacBio::Utility::Split(name, '/');

    // most PB record names have 3 components, with the exception
    // of by-strand CCS, which has an additional 'fwd/rev' component
    if ((parts.size() < 3) || (parts.size() > 4)) {
        return;
    }

    // rip out & store name parts
    movieName_ = MovieNameType{std::move(parts.at(0))};
    zmw_ = std::stoi(parts.at(1));
    if (parts.at(2) != "ccs") {
        auto queryParts = PacBio::Utility::Split(parts.at(2), '_');
        if (queryParts.size() != 2) {
            return;
        }
        const Position queryStart = std::stoi(queryParts.at(0));
        const Position queryEnd = std::stoi(queryParts.at(1));
        queryInterval_.reset(new Interval{queryStart, queryEnd});
    }
}

template <typename MovieNameType>
std::string ReadNameBase<MovieNameType>::ToString() const
{
    // ensure we're OK
    Check();

    // construct name from parts
    std::string result;
    result.reserve(128);
    result += movieName_.ToStdString();
    result += "/";
    result += std::to_string(zmw_);
    result += "/";

    if (IsCCS()) {
        result += "ccs";
    } else {
        result += std::to_string(queryInterval_->Start());
        result += "_";
        result += std::to_string(queryInterval_->End());
    }

    return result;
}

template <typename MovieNameType>
bool operator!=(const ReadNameBase<MovieNameType>& lhs,
                const ReadNameBase<MovieNameType>& rhs) noexcept
{
    return !(lhs == rhs);
}

template <typename MovieNameType>
std::ostream& operator<<(std::ostream& os, const ReadNameBase<MovieNameType>& readName)
{
    os << readName.ToString();
    return os;
}

template <typename MovieNameType>
std::istream& operator>>(std::istream& is, ReadNameBase<MovieNameType>& readName)
{
    std::string s;
    is >> s;
    readName = ReadNameBase<MovieNameType>{std::move(s)};
    return is;
}

}  // namespace internal
}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_READNAMEBASE_INL_H
