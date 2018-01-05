#ifndef PBCOPPER_DATA_RSMOVIENAME_H
#define PBCOPPER_DATA_RSMOVIENAME_H

#include <iosfwd>
#include <memory>
#include <string>

#include <boost/utility/string_ref.hpp>

namespace PacBio {
namespace Data {

/// \brief The RSMovieName class provides methods for working with PacBio RSII
///        movie names.
///
class RSMovieName
{
public:
    /// \name Constructors & Related Methods
    /// \{

    // create RSMovieName from string
    explicit RSMovieName(const std::string& name);
    explicit RSMovieName(std::string&& name);

    // create MovieName from name parts
    RSMovieName(const std::string& runStartTime,
                const std::string& instrumentSerialNumber,
                const std::string& smrtCellBarcode,
                const std::string& setNumber,
                const std::string& partNumber);

    RSMovieName(void);
    RSMovieName(const RSMovieName& other);
    RSMovieName(RSMovieName&& other) noexcept;
    RSMovieName& operator=(const RSMovieName& other);
    RSMovieName& operator=(RSMovieName&& other) noexcept;
    ~RSMovieName(void);

    /// \}

public:
    /// \name Name Parts
    /// \{

    boost::string_ref InstrumentSerialNumber(void) const;
    boost::string_ref PartNumber(void) const;
    boost::string_ref RunStartTime(void) const;
    boost::string_ref SetNumber(void) const;
    boost::string_ref SMRTCellBarcode(void) const;

    /// \}

public:
    /// \name Additional Methods & Operators
    /// \{

    bool IsReagentExpired(void) const;
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
inline bool operator==(const RSMovieName& lhs, const RSMovieName& rhs);
inline bool operator!=(const RSMovieName& lhs, const RSMovieName& rhs);
inline bool operator<(const RSMovieName& lhs, const RSMovieName& rhs);

// I/O
inline std::ostream& operator<<(std::ostream& os, const RSMovieName& movieName);
inline std::istream& operator>>(std::istream& is, RSMovieName& movieName);

/// \}

} // namespace Data
} // namespace PacBio

#include "internal/RSMovieName-inl.h"

#endif // PBCOPPER_DATA_RSMOVIENAME_H
