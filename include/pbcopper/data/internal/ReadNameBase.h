#ifndef PBCOPPER_DATA_READNAMEBASE_H
#define PBCOPPER_DATA_READNAMEBASE_H

#include <pbcopper/PbcopperConfig.h>

#include <iosfwd>
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
                 const Interval& queryInterval);

    ReadNameBase(const MovieNameType& movieName, const PacBio::Data::Zmw& zmw,
                 const Position& queryStart, const Position& queryEnd);

    ReadNameBase(const MovieNameType& movieName, const PacBio::Data::Zmw& zmw, const CCSTag ccs);

    ReadNameBase() = default;

    /// \}

public:
    /// \name Name Parts
    /// \{

    MovieNameType MovieName() const;
    PacBio::Data::Zmw Zmw() const;
    Interval QueryInterval() const;
    Position QueryStart() const;
    Position QueryEnd() const;

    /// \}

public:
    /// \name Additional Methods & Operators
    /// \{

    bool IsCCS() const;
    std::string ToString() const;

    bool operator==(const ReadNameBase<MovieNameType>& other) const noexcept;
    bool operator<(const ReadNameBase<MovieNameType>& other) const noexcept;

    /// \}

private:
    MovieNameType movieName_;
    PacBio::Data::Zmw zmw_ = -1;
    std::unique_ptr<Interval> queryInterval_;

private:
    void Check() const;
    void FromString(std::string&& name);
};

// add'l comparison operators
template <typename MovieNameType>
bool operator!=(const ReadNameBase<MovieNameType>& lhs,
                const ReadNameBase<MovieNameType>& rhs) noexcept;

// I/O
template <typename MovieNameType>
std::ostream& operator<<(std::ostream& os, const ReadNameBase<MovieNameType>& readName);

template <typename MovieNameType>
std::istream& operator>>(std::istream& is, ReadNameBase<MovieNameType>& readName);

}  // namespace internal
}  // namespace Data
}  // namespace PacBio

#include <pbcopper/data/internal/ReadNameBase-inl.h>

#endif  // PBCOPPER_DATA_READNAMEBASE_H
