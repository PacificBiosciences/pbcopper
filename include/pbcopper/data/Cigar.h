// Author: Derek Barnett

#ifndef PBCOPPER_DATA_CIGAR_H
#define PBCOPPER_DATA_CIGAR_H

#include <pbcopper/PbcopperConfig.h>

#include <ostream>
#include <string>
#include <vector>

#include <pbcopper/data/CigarOperation.h>

namespace PacBio {
namespace Data {

/// \brief The Cigar class represents the CIGAR string used to report alignment
///        charateristics in SAM/BAM.
///
/// \note Use of the 'M' operator is forbidden in PacBio BAMs. See
///       CigarOperationType description for more information.
///
/// \sa https://samtools.github.io/hts-specs/SAMv1.pdf for more information on CIGAR in general.
///
class Cigar : public std::vector<CigarOperation>
{
public:
    /// \name Constructors & Related Methods
    /// \{

    /// \brief Creates a Cigar object from SAM/BAM string input
    ///
    /// \param [in] stdString   SAM/BAM formatted CIGAR data
    /// \returns a Cigar object representing the input data
    ///
    /// \note This class may be removed from the public API in the future,
    ///       as the constructor taking a std::string accomplishes the same end.
    ///
    static Cigar FromStdString(const std::string& stdString);

    static Cigar FromCStr(const char* str);

    /// \brief Creates an empty Cigar.
    Cigar() = default;

    /// \brief Creates a Cigar object from SAM/BAM string input
    ///
    /// \param [in] cigarString   SAM/BAM formatted CIGAR data
    ///
    Cigar(const char* str);

    Cigar(const std::string& cigarString);

    Cigar(std::vector<CigarOperation> cigar);

    /// \}

public:
    /// \name Conversion Methods
    /// \{

    /// Converts Cigar object data to SAM/BAM formatted string
    ///
    /// \returns SAM/BAM formatted std::string
    ///
    std::string ToStdString() const;

    /// \}
};

///
/// \brief
///
/// \param cigar
/// \return size_t
///
size_t ReferenceLength(const Cigar& cigar);

std::ostream& operator<<(std::ostream& os, const Cigar& cigar);

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_CIGAR_H
