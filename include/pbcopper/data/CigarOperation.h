// Author: Derek Barnett

#ifndef PBCOPPER_DATA_CIGAROPERATION_H
#define PBCOPPER_DATA_CIGAROPERATION_H

#include <pbcopper/PbcopperConfig.h>

#include <cassert>
#include <cstdint>
#include <stdexcept>

namespace PacBio {
namespace Data {

/// \brief Describes a CIGAR operation.
///
/// Bracketed character is the corresponding SAM/BAM character code.
///
/// \warning ALIGNMENT_MATCH ('M') is included in this enum to maintain
///          consistency with htslib. However, as of PacBio BAM spec version
///          3.0b7, this CIGAR operation \b forbidden. Any attempt to read or
///          write a record containing this operation will trigger a
///          std::runtime_error. SEQUENCE_MATCH('=) or SEQUENCE_MISMATCH('X')
///          should be used instead.
///
enum class CigarOperationType
{
    UNKNOWN_OP = -1,      ///< unknown/invalid CIGAR operator
    ALIGNMENT_MATCH = 0,  ///< alignment match (can be a sequence match or mismatch) [M]
    INSERTION,            ///< insertion to the reference [I]
    DELETION,             ///< deletion from the reference [D]
    REFERENCE_SKIP,       ///< skipped region from the reference [N]
    SOFT_CLIP,            ///< soft clipping (clipped sequences present in SEQ) [S]
    HARD_CLIP = 5,        ///< hard clipping (clipped sequences NOT present in SEQ) [H]
    PADDING,              ///< padding (silent deletion from padded reference) [P]
    SEQUENCE_MATCH,       ///< sequence match [=]
    SEQUENCE_MISMATCH     ///< sequence mismatch [X]
};

/// \brief The CigarOperation class represents a single CIGAR operation
///        (consisting of a type & length).
///
class CigarOperation
{
public:
    static void DisableAutoValidation();

public:
    /// \name Operation Type Conversion Methods
    /// \{

    /// Convert between CigarOperationType enum & SAM/BAM character code.
    ///
    /// \param[in] type CigarOperationType value
    /// \returns SAM/BAM character code
    static char TypeToChar(const CigarOperationType type);

    /// Convert between CigarOperationType enum & SAM/BAM character code.
    ///
    /// \param[in] c SAM/BAM character code
    /// \returns CigarOperationType value
    PB_CUDA_HOST PB_CUDA_DEVICE static CigarOperationType CharToType(const char c)
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

    /// \}

public:
    /// \name Constructors & Related Methods
    /// \{

    CigarOperation() = default;

    PB_CUDA_HOST PB_CUDA_DEVICE CigarOperation(char c, uint32_t length)
        : CigarOperation{CigarOperation::CharToType(c), length}
    {
    }

    PB_CUDA_HOST PB_CUDA_DEVICE CigarOperation(CigarOperationType op, uint32_t length)
        : type_{op}, length_{length}
    {
#ifndef __CUDA_ARCH__  // host
        if (AutoValidateCigar && (type_ == CigarOperationType::ALIGNMENT_MATCH))
            throw std::runtime_error{
                "[pbcopper] CIGAR operation ERROR: 'M' is not allowed in PacBio BAM files. Use "
                "'X/=' "
                "instead."};
#endif
    }

    /// \}

public:
    /// \returns operation type as SAM/BAM char code
    char Char() const;

    /// \returns operation length
    PB_CUDA_HOST PB_CUDA_DEVICE uint32_t Length() const { return length_; }

    /// \returns operation type as CigarOperationType enum value
    PB_CUDA_HOST PB_CUDA_DEVICE CigarOperationType Type() const { return type_; }

    /// \}

public:
    /// \name Attributes
    /// \{

    /// Sets this operation type.
    ///
    /// \param[in] opChar SAM/BAM character code
    /// \returns reference to this operation
    CigarOperation& Char(const char opChar);

    /// Sets this operation length.
    ///
    /// \param[in] length
    /// \returns reference to this operation
    CigarOperation& Length(const uint32_t length);

    /// Sets this operation type.
    ///
    /// \param[in] opType CigarOperationType value
    /// \returns reference to this operation
    CigarOperation& Type(const CigarOperationType opType);

    /// \}

public:
    /// \name Comparison Operators
    /// \{

    /// \returns true if both CIGAR operation type & length match
    bool operator==(const CigarOperation& other) const noexcept;

    /// \returns true if either CIGAR operation type or length differ
    bool operator!=(const CigarOperation& other) const noexcept;

    /// \}

private:
    static bool AutoValidateCigar;

private:
    CigarOperationType type_ = CigarOperationType::UNKNOWN_OP;
    uint32_t length_ = 0;
};

bool ConsumesQuery(const CigarOperationType type);

bool ConsumesReference(const CigarOperationType type);

}  // namespace Data
}  // namespace PacBio

#endif  // CIGAROPERATION_H
