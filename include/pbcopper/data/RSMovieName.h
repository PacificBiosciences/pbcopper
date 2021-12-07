#ifndef PBCOPPER_DATA_RSMOVIENAME_H
#define PBCOPPER_DATA_RSMOVIENAME_H

#include <pbcopper/PbcopperConfig.h>

#include <boost/utility/string_ref.hpp>

#include <iosfwd>
#include <memory>
#include <string>
#include <type_traits>

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
    explicit RSMovieName(std::string name);

    // create MovieName from name parts
    RSMovieName(const std::string& runStartTime, const std::string& instrumentSerialNumber,
                const std::string& smrtCellBarcode, const std::string& setNumber,
                const std::string& partNumber);

    RSMovieName();
    RSMovieName(const RSMovieName& other);
    RSMovieName(RSMovieName&& other) noexcept;
    RSMovieName& operator=(const RSMovieName& other);
    RSMovieName& operator=(RSMovieName&& other) noexcept(
        std::is_nothrow_move_assignable<std::string>::value);
    ~RSMovieName();

    /// \}

public:
    /// \name Name Parts
    /// \{

    boost::string_ref InstrumentSerialNumber() const;
    boost::string_ref PartNumber() const;
    boost::string_ref RunStartTime() const;
    boost::string_ref SetNumber() const;
    boost::string_ref SMRTCellBarcode() const;

    /// \}

public:
    /// \name Additional Methods & Operators
    /// \{

    bool IsReagentExpired() const;
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
bool operator==(const RSMovieName& lhs, const RSMovieName& rhs) noexcept;
bool operator!=(const RSMovieName& lhs, const RSMovieName& rhs) noexcept;
bool operator<(const RSMovieName& lhs, const RSMovieName& rhs) noexcept;

// I/O
std::ostream& operator<<(std::ostream& os, const RSMovieName& movieName);
std::istream& operator>>(std::istream& is, RSMovieName& movieName);

/// \}

}  // namespace Data
}  // namespace PacBio

#include <pbcopper/data/internal/RSMovieName-inl.h>

#endif  // PBCOPPER_DATA_RSMOVIENAME_H
