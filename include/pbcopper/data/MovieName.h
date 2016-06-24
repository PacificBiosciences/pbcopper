#ifndef PBCOPPER_DATA_MOVIENAME_H
#define PBCOPPER_DATA_MOVIENAME_H

#include "pbcopper/Config.h"
#include <boost/utility/string_ref.hpp>
#include <iosfwd>
#include <memory>
#include <string>

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
    explicit MovieName(const std::string& name);
    explicit MovieName(std::string&& name);

    // create MovieName from name parts
    MovieName(const std::string& instrumentSerialNumber,
              const std::string& runStartTime);

    MovieName(void);
    MovieName(const MovieName& other);
    MovieName(MovieName&& other) noexcept;
    MovieName& operator=(const MovieName& other);
    MovieName& operator=(MovieName&& other) noexcept;
    ~MovieName(void);

    /// \}

public:
    /// \name Name Parts
    /// \{

    boost::string_ref InstrumentName(void) const;
    boost::string_ref RunStartTime(void) const;

    /// \}

public:
    /// \name Additional Methods & Operators
    /// \{

    std::string ToStdString(void) const;

    /// \}

private:
    std::string movieName_;

    struct PartsCache;
    mutable std::unique_ptr<PartsCache> partsCache_;

private:
    void UpdatePartsCache(void) const;
};

/// \name Related Non-members
/// \{


// comparison operators
inline bool operator==(const MovieName& lhs, const MovieName& rhs);
inline bool operator!=(const MovieName& lhs, const MovieName& rhs);
inline bool operator<(const MovieName& lhs, const MovieName& rhs);

// I/O
inline std::ostream& operator<<(std::ostream& os, const MovieName& movieName);
inline std::istream& operator>>(std::istream& is, MovieName& movieName);

/// \}

} // namespace Data
} // namespace PacBio

#include "internal/MovieName-inl.h"

#endif // PBCOPPER_DATA_MOVIENAME_H
