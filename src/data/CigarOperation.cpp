#include <pbcopper/data/CigarOperation.h>

#include <stdexcept>
#include <tuple>
#include <type_traits>

namespace PacBio {
namespace Data {
namespace {

// Since we can't rely on having htslib, define its CIGAR helpers here.
// See <htslib/sam.h> . Names are changed to ensure we don't clash if htslib is
// present.

// clang-format off
#define HTSLIB_BAM_CMATCH      0
#define HTSLIB_BAM_CINS        1
#define HTSLIB_BAM_CDEL        2
#define HTSLIB_BAM_CREF_SKIP   3
#define HTSLIB_BAM_CSOFT_CLIP  4
#define HTSLIB_BAM_CHARD_CLIP  5
#define HTSLIB_BAM_CPAD        6
#define HTSLIB_BAM_CEQUAL      7
#define HTSLIB_BAM_CDIFF       8
#define HTSLIB_BAM_CBACK       9

#define HTSLIB_BAM_CIGAR_STR    "MIDNSHP=XB"
#define HTSLIB_BAM_CIGAR_SHIFT  4
#define HTSLIB_BAM_CIGAR_MASK   0xf
#define HTSLIB_BAM_CIGAR_TYPE   0x3C1A7

#define htslib_bam_cigar_op(c)     ((c) & HTSLIB_BAM_CIGAR_MASK)
#define htslib_bam_cigar_oplen(c)  ((c) >> HTSLIB_BAM_CIGAR_SHIFT)
#define htslib_bam_cigar_opchr(c)  (HTSLIB_BAM_CIGAR_STR "??????"[htslib_bam_cigar_op(c)])
#define htslib_bam_cigar_gen(l, o) ((l) << HTSLIB_BAM_CIGAR_SHIFT | (o))
#define htslib_bam_cigar_type(o)   (HTSLIB_BAM_CIGAR_TYPE >> ((o) << 1) & 3)
// clang-format on

}  // namespace

bool CigarOperation::AutoValidateCigar = true;

bool CigarOperation::operator==(const CigarOperation& other) const noexcept
{
    return std::tie(type_, length_) == std::tie(other.type_, other.length_);
}

bool CigarOperation::operator!=(const CigarOperation& other) const noexcept
{
    return !(*this == other);
}

char CigarOperation::Char() const { return CigarOperation::TypeToChar(type_); }

CigarOperation& CigarOperation::Char(const char opChar)
{
    type_ = CigarOperation::CharToType(opChar);
    return *this;
}

void CigarOperation::DisableAutoValidation() { AutoValidateCigar = false; }

CigarOperation& CigarOperation::Length(const uint32_t length)
{
    length_ = length;
    return *this;
}

CigarOperation& CigarOperation::Type(const CigarOperationType opType)
{
    type_ = opType;
    return *this;
}

char CigarOperation::TypeToChar(const CigarOperationType type)
{
    return htslib_bam_cigar_opchr(static_cast<int>(type));
}

bool ConsumesQuery(const CigarOperationType type)
{
    return (htslib_bam_cigar_type(static_cast<int>(type)) & 0x1) != 0;
}

bool ConsumesReference(const CigarOperationType type)
{
    return (htslib_bam_cigar_type(static_cast<int>(type)) & 0x2) != 0;
}

}  // namespace Data
}  // namespace PacBio
