#ifndef PBCOPPER_DATA_CIGAROPERATION_INL_H
#define PBCOPPER_DATA_CIGAROPERATION_INL_H

#include <stdexcept>

#include "pbcopper/data/CigarOperation.h"

namespace PacBio {
namespace Data {
namespace Internal {

inline void validate(const CigarOperationType type)
{
    // Define this symbol on the occasion where we need to use data from
    // external software that may emit the 'M' CIGAR operation. In most cases,
    // it should NOT be defined (thus leaving this validation enabled) to ensure
    // compliance with PacBio BAM specs.
    //
#if !defined(PBCOPPER_PERMISSIVE_CIGAR)
    if (type == CigarOperationType::ALIGNMENT_MATCH)
    {
        throw std::runtime_error("CIGAR operation 'M' is not allowed in PacBio "
                                 "BAM files. Use 'X/=' instead.");
    }
#endif
}

} // namespace Internal

inline CigarOperation::CigarOperation(void)
    : type_(CigarOperationType::UNKNOWN_OP)
    , length_(0)
{ }

inline CigarOperation::CigarOperation(char c, uint32_t length)
    : type_(CigarOperation::CharToType(c))
    , length_(length)
{
    Internal::validate(type_);
}

inline CigarOperation::CigarOperation(CigarOperationType op, uint32_t length)
    : type_(op)
    , length_(length)
{
    Internal::validate(type_);
}

inline char CigarOperation::Char(void) const
{
    return CigarOperation::TypeToChar(type_);
}

inline CigarOperation& CigarOperation::Char(const char opChar)
{
    return Type(CigarOperation::CharToType(opChar));
}

inline uint32_t CigarOperation::Length(void) const
{
    return length_;
}

inline CigarOperation& CigarOperation::Length(const uint32_t length)
{
    length_ = length;
    return *this;
}

inline CigarOperationType CigarOperation::Type(void) const
{
    return type_;
}

inline CigarOperation& CigarOperation::Type(const CigarOperationType opType)
{
    type_ = opType;
    Internal::validate(type_);
    return *this;
}

inline bool CigarOperation::operator==(const CigarOperation& other) const
{
    return type_ == other.type_ &&
           length_ == other.length_;
}

inline bool CigarOperation::operator!=(const CigarOperation& other) const
{
    return !(*this == other);
}

} // namespace Data
} // namespace PacBio

#endif // PBCOPPER_DATA_CIGAROPERATION_INL_H
