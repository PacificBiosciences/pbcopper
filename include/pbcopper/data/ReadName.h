#ifndef PBCOPPER_DATA_READNAME_H
#define PBCOPPER_DATA_READNAME_H

#include "pbcopper/Config.h"
#include "pbcopper/data/CCSTag.h"
#include "pbcopper/data/Interval.h"
#include "pbcopper/data/MovieName.h"
#include "pbcopper/data/Position.h"
#include "pbcopper/data/Zmw.h"
#include <iosfwd>
#include <memory>
#include <string>

namespace PacBio {
namespace Data {

/// \brief The ReadName class provides methods for working with PacBio record
///        names.
class ReadName
{
public:
    /// \name Constructors & Related Methods
    /// \{

    // create ReadName from string
    explicit ReadName(const std::string& name);
    explicit ReadName(std::string&& name);

    // create ReadName from name parts
    ReadName(const PacBio::Data::MovieName& movieName,
             const PacBio::Data::Zmw& zmw,
             const Interval<Position>& queryInterval);

    ReadName(const PacBio::Data::MovieName& movieName,
             const PacBio::Data::Zmw& zmw,
             const Position& queryStart,
             const Position& queryEnd);

    ReadName(const PacBio::Data::MovieName& movieName,
             const PacBio::Data::Zmw& zmw,
             const CCSTag ccs);

    ReadName(void);
    ReadName(const ReadName& other) = default;
    ReadName(ReadName&& other) = default;
    ReadName& operator=(const ReadName& other) = default;
    ReadName& operator=(ReadName&& other) = default;
    ~ReadName(void) = default;

    /// \}

public:
    /// \name Name Parts
    /// \{

    PacBio::Data::MovieName MovieName(void) const;
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

    bool operator==(const ReadName& other) const;
    bool operator<(const ReadName& other) const;

    /// \}

private:
    PacBio::Data::MovieName movieName_;
    PacBio::Data::Zmw       zmw_;
    std::unique_ptr<Interval<Position>> queryInterval_;

private:
    void Check(void) const;
    void FromString(std::string&& name);
};

/// \name Related Non-members
/// \{

// add'l comparison operators
inline bool operator!=(const ReadName& lhs, const ReadName& rhs);

// I/O
inline std::ostream& operator<<(std::ostream& os, const ReadName& readName);
inline std::istream& operator>>(std::istream& is, ReadName& readName);

/// \}

} // namespace Data
} // namespace PacBio

#include "internal/ReadName-inl.h"

#endif // PBCOPPER_DATA_READNAME_H
