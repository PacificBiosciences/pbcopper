#ifndef PBCOPPER_DATA_MOVIENAME_H
#define PBCOPPER_DATA_MOVIENAME_H

#include <pbcopper/PbcopperConfig.h>

#include <iosfwd>
#include <memory>
#include <string>
#include <type_traits>

#include <boost/utility/string_ref.hpp>

namespace PacBio {
namespace Data {

/// \brief The MovieName class provides methods for working with PacBio movie
///        names.
class MovieName
{
public:
    /// \name Constructors & Related Methods
    /// \{

    // create MovieName from string
    explicit MovieName(std::string name);

    // create MovieName from name parts
    MovieName(const std::string& instrumentName, const std::string& runStartTime);

    MovieName();
    MovieName(const MovieName& other);
    MovieName(MovieName&& other) noexcept;
    MovieName& operator=(const MovieName& other);
    MovieName& operator=(MovieName&& other) noexcept(
        std::is_nothrow_move_assignable<std::string>::value);
    ~MovieName();

    /// \}

public:
    /// \name Name Parts
    /// \{

    boost::string_ref InstrumentName() const;
    boost::string_ref RunStartTime() const;

    /// \}

public:
    /// \name Additional Methods & Operators
    /// \{

    std::string ToStdString() const;

    /// \}

private:
    std::string movieName_;

    struct PartsCache;
    mutable std::unique_ptr<PartsCache> partsCache_;

private:
    void UpdatePartsCache() const;
};

/// \name Related Non-members
/// \{

// comparison operators
bool operator==(const MovieName& lhs, const MovieName& rhs) noexcept;
bool operator!=(const MovieName& lhs, const MovieName& rhs) noexcept;
bool operator<(const MovieName& lhs, const MovieName& rhs) noexcept;

// I/O
std::ostream& operator<<(std::ostream& os, const MovieName& movieName);
std::istream& operator>>(std::istream& is, MovieName& movieName);

/// \}

}  // namespace Data
}  // namespace PacBio

#include <pbcopper/data/internal/MovieName-inl.h>

#endif  // PBCOPPER_DATA_MOVIENAME_H
