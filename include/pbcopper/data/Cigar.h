#ifndef PBCOPPER_DATA_CIGAR_H
#define PBCOPPER_DATA_CIGAR_H

#include <pbcopper/PbcopperConfig.h>

#include <iosfwd>
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

/// https://confluence.pacificbiosciences.com/pages/viewpage.action?spaceKey=SL&title=Concordance%2C+Identity%2C+and+SMRT+Link+Reports
struct CigarBaseCounts
{
    int DeletionBases;
    int DeletionEvents;
    int InsertionBases;
    int InsertionEvents;
    int MatchBases;
    int MatchEvents;
    int MismatchBases;
    int MismatchEvents;
    int NumAlignedBases;
    double GapCompressedIdentity;
    double Identity;
};

///
/// \brief
///
/// \param cigar
/// \return CigarBaseCounts, which includes identity calculations
///
CigarBaseCounts CigarOpsCalculator(const Cigar& cigar);

///
/// \brief
///
/// \param cigar
/// \return size_t
///
size_t ReferenceLength(const Cigar& cigar);

std::ostream& operator<<(std::ostream& os, const Cigar& cigar);

///
/// \brief Apply CIGAR to annotate indels in sequences.
///
/// M5 refers to the .m5 format used by e.g. dagcon, sparc, blasr
///
/// Example:
///   input reference:  AAAAAAAAAAA
///   input query:      AAAAAAAA
///   input CIGAR:      3=2D2=1I2D2=
///
///   result reference: AAAAAAA-AAAA
///   result query:     AAA--AAA--AA
///
/// \param ref              input reference sequence
/// \param query            input query sequence
/// \param qReversed        true if query is reverse-complemented
/// \param cigar            CIGAR alignment data
/// \param retRefAln        output annotated reference sequence
/// \param retQueryAln      output annotated query sequence
///
/// \return success/failure
///
bool ConvertCigarToM5(const std::string& ref, const std::string& query, bool qReversed,
                      const Data::Cigar& cigar, std::string& retRefAln, std::string& retQueryAln);

///
/// \brief Apply CIGAR to annotate indels in sequence substrings.
///
/// M5 refers to the .m5 format used by e.g. dagcon, sparc, blasr
///
/// \param ref              input reference sequence
/// \param query            input query sequence
/// \param rStart           start position on reference sequence to apply CIGAR
/// \param rEnd             end position on reference sequence to apply CIGAR
/// \param qStart           start position on query sequence to apply CIGAR
/// \param qEnd             end position on query sequence to apply CIGAR
/// \param qReversed        true if query is reverse-complemented
/// \param cigar            CIGAR alignment data
/// \param retRefAln        output annotated reference sequence
/// \param retQueryAln      output annotated query sequence
///
/// \return success/failure
///
bool ConvertCigarToM5(const std::string& ref, const std::string& query, int32_t rStart,
                      int32_t rEnd, int32_t qStart, int32_t qEnd, bool qReversed,
                      const Data::Cigar& cigar, std::string& retRefAln, std::string& retQueryAln);

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_CIGAR_H
