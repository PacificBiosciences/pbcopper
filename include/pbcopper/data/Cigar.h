// Author: Derek Barnett

#ifndef PBCOPPER_DATA_CIGAR_H
#define PBCOPPER_DATA_CIGAR_H

#include <string>
#include <vector>

#include <pbcopper/PbcopperConfig.h>
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

    /// \brief Creates an empty Cigar.
    Cigar(void);

    /// \brief Creates a Cigar object from SAM/BAM string input
    ///
    /// \param [in] cigarString   SAM/BAM-formatted CIGAR data
    ///
    explicit Cigar(const std::string& cigarString);

    Cigar(const Cigar&) = default;
    Cigar(Cigar&&) = default;
    Cigar& operator=(const Cigar&) = default;
    Cigar& operator=(Cigar&&) = default;
    ~Cigar(void) = default;

    /// \}

public:
    /// \name Conversion Methods
    /// \{

    /// Converts Cigar object data to SAM/BAM-formatted string
    ///
    /// \returns SAM/BAM formatted std::string
    ///
    std::string ToStdString(void) const;

    /// \}
};

///
/// \brief Input operator
/// \param in
/// \param op
/// \return
///
std::istream& operator>>(std::istream& in, Cigar& cigar);

///
/// \brief Output operator
/// \param out
/// \param op
///
std::ostream& operator<<(std::ostream& out, const Cigar& cigar);

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_CIGAR_H
