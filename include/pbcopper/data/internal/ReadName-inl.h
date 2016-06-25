
#ifndef PBCOPPER_DATA_READNAME_INL_H
#define PBCOPPER_DATA_READNAME_INL_H

#include "pbcopper/data/ReadName.h"
#include "pbcopper/utility/StringUtils.h"
#include <stdexcept>

namespace PacBio {
namespace Data {

//inline ReadName::ReadName(void)
//    : movieName_()
//    , zmw_(-1)
//    , queryInterval_(nullptr)
//{ }

//inline ReadName::ReadName(const std::string& name)
//{
//    auto copy = std::string{ name };
//    FromString(std::move(copy));
//    Check();
//}

//inline ReadName::ReadName(std::string&& name)
//{
//    FromString(std::move(name));
//    Check();
//}

//inline ReadName::ReadName(const PacBio::Data::MovieName& movieName,
//                          const PacBio::Data::Zmw& zmw,
//                          const Interval<Position>& queryInterval)
//    : movieName_(movieName)
//    , zmw_(zmw)
//    , queryInterval_(new Interval<Position>{ queryInterval })
//{ }

//inline ReadName::ReadName(const PacBio::Data::MovieName& movieName,
//                          const PacBio::Data::Zmw& zmw,
//                          const Position& queryStart,
//                          const Position& queryEnd)
//    : ReadName(movieName, zmw, Interval<Position>(queryStart, queryEnd))
//{ }

//inline ReadName::ReadName(const PacBio::Data::MovieName& movieName,
//                          const PacBio::Data::Zmw& zmw,
//                          const CCSTag)
//    : movieName_(movieName)
//    , zmw_(zmw)
//    , queryInterval_(nullptr)
//{ }

//inline bool ReadName::IsCCS(void) const
//{ return !queryInterval_; }

//inline PacBio::Data::MovieName ReadName::MovieName(void) const
//{ return movieName_; }

//inline Interval<Position> ReadName::QueryInterval(void) const
//{
//    if (IsCCS())
//        throw std::runtime_error("ReadName: cannot get query interval from CCS read");
//    return *queryInterval_;
//}

//inline Position ReadName::QueryStart(void) const
//{
//    if (IsCCS())
//        throw std::runtime_error("ReadName: cannot get query start from CCS read");
//    return queryInterval_->Start();
//}

//inline Position ReadName::QueryEnd(void) const
//{
//    if (IsCCS())
//        throw std::runtime_error("ReadName: cannot get query end from CCS read");
//    return queryInterval_->End();
//}

//inline PacBio::Data::Zmw ReadName::Zmw(void) const
//{ return zmw_; }

//inline bool operator!=(const ReadName& lhs, const ReadName& rhs)
//{ return !(lhs == rhs); }

//inline std::ostream& operator<<(std::ostream& os, const ReadName& readName)
//{
//    os << readName.ToString();
//    return os;
//}

//inline std::istream& operator>>(std::istream& is, ReadName& readName)
//{
//    auto s = std::string{ };
//    is >> s;
//    readName = ReadName{ std::move(s) };
//    return is;
//}

} // namespace Data
} // namespace PacBio

#endif // PBCOPPER_DATA_READNAME_INL_H
