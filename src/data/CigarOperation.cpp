// Author: Derek Barnett

#include <pbcopper/data/CigarOperation.h>

#include <cassert>
#include <stdexcept>
#include <tuple>
#include <type_traits>

namespace PacBio {
namespace Data {
namespace {

static bool AutoValidateCigar = true;

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

static_assert(std::is_copy_constructible<CigarOperation>::value,
              "CigarOperation(const CigarOperation&) is not = default");
static_assert(std::is_copy_assignable<CigarOperation>::value,
              "CigarOperation& operator=(const CigarOperation&) is not = default");

static_assert(std::is_nothrow_move_constructible<CigarOperation>::value,
              "CigarOperation(CigarOperation&&) is not = noexcept");
static_assert(std::is_nothrow_move_assignable<CigarOperation>::value,
              "CigarOperation& operator=(CigarOperation&&) is not = noexcept");

CigarOperation::CigarOperation(char c, uint32_t length)
    : CigarOperation{CigarOperation::CharToType(c), length}
{
}

CigarOperation::CigarOperation(CigarOperationType op, uint32_t length) : type_{op}, length_{length}
{
    if (AutoValidateCigar && (type_ == CigarOperationType::ALIGNMENT_MATCH))
        throw std::runtime_error{
            "CIGAR operation 'M' is not allowed in PacBio BAM files. Use 'X/=' instead."};
}

bool CigarOperation::operator==(const CigarOperation& other) const
{
    return std::tie(type_, length_) == std::tie(other.type_, other.length_);
}

bool CigarOperation::operator!=(const CigarOperation& other) const { return !(*this == other); }

char CigarOperation::Char() const { return CigarOperation::TypeToChar(type_); }

CigarOperation& CigarOperation::Char(const char opChar)
{
    type_ = CigarOperation::CharToType(opChar);
    return *this;
}

CigarOperationType CigarOperation::CharToType(const char c)
{
    switch (c) {
        case 'S':
            return CigarOperationType::SOFT_CLIP;
        case '=':
            return CigarOperationType::SEQUENCE_MATCH;
        case 'X':
            return CigarOperationType::SEQUENCE_MISMATCH;
        case 'I':
            return CigarOperationType::INSERTION;
        case 'D':
            return CigarOperationType::DELETION;
        case 'N':
            return CigarOperationType::REFERENCE_SKIP;
        case 'H':
            return CigarOperationType::HARD_CLIP;
        case 'P':
            return CigarOperationType::PADDING;
        case 'M':
            return CigarOperationType::ALIGNMENT_MATCH;
        default:
            return CigarOperationType::UNKNOWN_OP;
    }
}

void CigarOperation::DisableAutoValidation() { AutoValidateCigar = false; }

uint32_t CigarOperation::Length() const { return length_; }

CigarOperation& CigarOperation::Length(const uint32_t length)
{
    length_ = length;
    return *this;
}

CigarOperationType CigarOperation::Type() const { return type_; }

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
