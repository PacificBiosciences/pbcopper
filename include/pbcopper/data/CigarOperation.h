// Copyright (c) 2016-2018, Pacific Biosciences of California, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//  * Neither the name of Pacific Biosciences nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY PACIFIC
// BIOSCIENCES AND ITS CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL PACIFIC BIOSCIENCES OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
// USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

// Author: Derek Barnett
#ifndef PBCOPPER_DATA_CIGAROPERATION_H
#define PBCOPPER_DATA_CIGAROPERATION_H

#include <cstdint>
#include <iostream>

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Data {

/// \brief Describes a CIGAR operation.
///
/// Bracketed character is the corresponding SAM/BAM character code.
///
/// \warning ALIGNMENT_MATCH ('M') is included in this enum to maintain
///          consistency with htslib. However, as of PacBio BAM spec version
///          3.0b7, this CIGAR operation \b forbidden in PacBio data. Any
///          attempt to read or write a record containing this operation will
///          trigger a std::runtime_error. SEQUENCE_MATCH('=) or
///          SEQUENCE_MISMATCH('X') should be used instead.
///
enum class CigarOperationType
{
    UNKNOWN_OP        = -1 ///< unknown/invalid CIGAR operator
  , ALIGNMENT_MATCH   = 0  ///< alignment match (can be a sequence match or mismatch) [M]
  , INSERTION              ///< insertion to the reference [I]
  , DELETION               ///< deletion from the reference [D]
  , REFERENCE_SKIP         ///< skipped region from the reference [N]
  , SOFT_CLIP              ///< soft clipping (clipped sequences present in SEQ) [S]
  , HARD_CLIP         = 5  ///< hard clipping (clipped sequences NOT present in SEQ) [H]
  , PADDING                ///< padding (silent deletion from padded reference) [P]
  , SEQUENCE_MATCH         ///< sequence match [=]
  , SEQUENCE_MISMATCH      ///< sequence mismatch [X]
};

/// \brief The CigarOperation class represents a single CIGAR operation
///        (consisting of a type & length).
///
class CigarOperation
{
public:

    /// \name Operation Type Conversion Methods
    /// \{

    /// \brief Converts enum value to SAM/BAM character code.
    ///
    /// \param[in] type CigarOperationType value
    /// \returns SAM/BAM character code
    ///
    static char TypeToChar(const CigarOperationType type);

    /// \brief Converts SAM/BAM character code to enum value.
    ///
    /// \param[in] c    SAM/BAM character code
    /// \returns CigarOperationType value
    ///
    static CigarOperationType CharToType(const char c);

    /// \}

public:
    /// \name Constructors & Related Methods
    /// \{

    ///
    /// \brief Creates a default (unknown) CigarOperation, with length = 0.
    ///
    CigarOperation(void);

    ///
    /// \brief Creates a CigarOperation from char code & leength.
    ///
    /// \param c        char code for op type
    /// \param length   op length
    ///
    CigarOperation(char c, uint32_t length);

    ///
    /// \brief Creates a CigarOperation from enum value & length
    ///
    /// \param op       enum value for op type
    /// \param length   op length
    ///
    CigarOperation(CigarOperationType op, uint32_t length);

    CigarOperation(const CigarOperation& other) = default;
    CigarOperation(CigarOperation&& other) = default;
    CigarOperation& operator=(const CigarOperation& other) = default;
    CigarOperation& operator=(CigarOperation&& other) = default;
    ~CigarOperation(void) = default;

    /// \}

public:
    /// \name Comparison Operators
    /// \{

    /// \returns true if both CIGAR operation type & length match
    bool operator==(const CigarOperation& other) const;

    /// \returns true if either CIGAR operation type or length differ
    bool operator!=(const CigarOperation& other) const;

    /// \}

public:
    /// \name Attributes
    /// \{

    /// \returns operation type as SAM/BAM char code
    char Char(void) const;

    /// \returns operation length
    uint32_t Length(void) const;

    /// \returns operation type as CigarOperationType enum value
    CigarOperationType Type(void) const;

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

private:
    CigarOperationType type_;
    uint32_t length_;
};

///
/// \brief Input operator
/// \param in
/// \param op
/// \return
///
std::istream& operator>>(std::istream& in, CigarOperation& op);

///
/// \brief Output operator
/// \param out
/// \param op
///
std::ostream& operator<<(std::ostream& out, const CigarOperation& op);

} // namespace Data
} // namespace PacBio

#include <pbcopper/data/internal/CigarOperation-inl.h>

#endif // PBCOPPER_DATA_CIGAROPERATION_H
