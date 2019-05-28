// Author: Derek Barnett

#ifndef PBCOPPER_DATA_READNAMEBASE_H
#define PBCOPPER_DATA_READNAMEBASE_H

#include <pbcopper/PbcopperConfig.h>

#include <type_traits>

#include <pbcopper/data/CCSTag.h>
#include <pbcopper/data/Interval.h>
#include <pbcopper/data/MovieName.h>
#include <pbcopper/data/Position.h>
#include <pbcopper/data/Zmw.h>

namespace PacBio {
namespace Data {
namespace internal {

template <typename MovieNameType>
class ReadNameBase
{
public:
    /// \name Constructors & Related Methods
    /// \{

    // create ReadNameBase from string
    explicit ReadNameBase(const std::string& name);
    explicit ReadNameBase(std::string&& name);

    // create ReadName from name parts
    ReadNameBase(const MovieNameType& movieName, const PacBio::Data::Zmw& zmw,
                 const Interval<Position>& queryInterval);

    ReadNameBase(const MovieNameType& movieName, const PacBio::Data::Zmw& zmw,
                 const Position& queryStart, const Position& queryEnd);

    ReadNameBase(const MovieNameType& movieName, const PacBio::Data::Zmw& zmw, const CCSTag ccs);

    ReadNameBase();
    ReadNameBase(const ReadNameBase<MovieNameType>&) = default;
    ReadNameBase(ReadNameBase<MovieNameType>&&) noexcept = default;
    ReadNameBase<MovieNameType>& operator=(const ReadNameBase<MovieNameType>&) = default;
    ReadNameBase<MovieNameType>& operator=(ReadNameBase<MovieNameType>&&) noexcept(
        std::is_nothrow_move_assignable<MovieNameType>::value) = default;
    ~ReadNameBase() = default;

    /// \}

public:
    /// \name Name Parts
    /// \{

    MovieNameType MovieName() const;
    PacBio::Data::Zmw Zmw() const;
    Interval<Position> QueryInterval() const;
    Position QueryStart() const;
    Position QueryEnd() const;

    /// \}

public:
    /// \name Additional Methods & Operators
    /// \{

    bool IsCCS() const;
    std::string ToString() const;

    bool operator==(const ReadNameBase<MovieNameType>& other) const;
    bool operator<(const ReadNameBase<MovieNameType>& other) const;

    /// \}

private:
    MovieNameType movieName_;
    PacBio::Data::Zmw zmw_;
    std::unique_ptr<Interval<Position>> queryInterval_;

private:
    void Check() const;
    void FromString(std::string&& name);
};

// add'l comparison operators
template <typename MovieNameType>
inline bool operator!=(const ReadNameBase<MovieNameType>& lhs,
                       const ReadNameBase<MovieNameType>& rhs);

// I/O
template <typename MovieNameType>
inline std::ostream& operator<<(std::ostream& os, const ReadNameBase<MovieNameType>& readName);

template <typename MovieNameType>
inline std::istream& operator>>(std::istream& is, ReadNameBase<MovieNameType>& readName);

}  // namespace internal
}  // namespace Data
}  // namespace PacBio

#include <pbcopper/data/internal/ReadNameBase-inl.h>

#endif  // PBCOPPER_DATA_READNAMEBASE_H
