#ifndef PBCOPPER_DATA_READNAMEBASE_H
#define PBCOPPER_DATA_READNAMEBASE_H

#include "pbcopper/Config.h"
#include "pbcopper/data/CCSTag.h"
#include "pbcopper/data/Interval.h"
#include "pbcopper/data/MovieName.h"
#include "pbcopper/data/Position.h"
#include "pbcopper/data/Zmw.h"

namespace PacBio {
namespace Data {
namespace internal {

template<typename MovieNameType>
class ReadNameBase
{
public:
    /// \name Constructors & Related Methods
    /// \{

    // create ReadNameBase from string
    explicit ReadNameBase(const std::string& name);
    explicit ReadNameBase(std::string&& name);

    // create ReadName from name parts
    ReadNameBase(const MovieNameType& movieName,
                 const PacBio::Data::Zmw& zmw,
                 const Interval<Position>& queryInterval);

    ReadNameBase(const MovieNameType& movieName,
                 const PacBio::Data::Zmw& zmw,
                 const Position& queryStart,
                 const Position& queryEnd);

    ReadNameBase(const MovieNameType& movieName,
                 const PacBio::Data::Zmw& zmw,
                 const CCSTag ccs);

    ReadNameBase(void);
    ReadNameBase(const ReadNameBase<MovieNameType>& other) = default;
    ReadNameBase(ReadNameBase<MovieNameType>&& other) = default;
    ReadNameBase<MovieNameType>& operator=(const ReadNameBase<MovieNameType>& other) = default;
    ReadNameBase<MovieNameType>& operator=(ReadNameBase<MovieNameType>&& other) = default;
    ~ReadNameBase(void) = default;

    /// \}

public:
    /// \name Name Parts
    /// \{

    MovieNameType MovieName(void) const;
    PacBio::Data::Zmw Zmw(void) const;
    Interval<Position> QueryInterval(void) const;
    Position QueryStart(void) const;
    Position QueryEnd(void) const;

    /// \}

public:
    /// \name Additional Methods & Operators
    /// \{

    bool IsCCS(void) const;
    std::string ToString(void) const;

    bool operator==(const ReadNameBase<MovieNameType>& other) const;
    bool operator<(const ReadNameBase<MovieNameType>& other) const;

    /// \}

private:
    MovieNameType movieName_;
    PacBio::Data::Zmw zmw_;
    std::unique_ptr<Interval<Position>> queryInterval_;

private:
    void Check(void) const;
    void FromString(std::string&& name);
};

// add'l comparison operators
template<typename MovieNameType>
inline bool operator!=(const ReadNameBase<MovieNameType>& lhs,
                       const ReadNameBase<MovieNameType>& rhs);

// I/O
template<typename MovieNameType>
inline std::ostream& operator<<(std::ostream& os,
                                const ReadNameBase<MovieNameType>& readName);

template<typename MovieNameType>
inline std::istream& operator>>(std::istream& is,
                                ReadNameBase<MovieNameType>& readName);

} // namespace internal
} // namespace Data
} // namespace PacBio

#include "ReadNameBase-inl.h"

#endif // PBCOPPER_DATA_READNAMEBASE_H
