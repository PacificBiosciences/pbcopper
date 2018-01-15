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

#include <pbcopper/data/CigarOperation.h>

#include <sstream>
#include <stdexcept>
#include <unordered_map>

#include <pbcopper/utility/EnumClassHash.h>

namespace PacBio {
namespace Data {

CigarOperationType CigarOperation::CharToType(const char c)
{
    // clang-format off
    static const auto lookup = std::unordered_map<char, CigarOperationType>
    {
        { 'S', CigarOperationType::SOFT_CLIP },
        { '=', CigarOperationType::SEQUENCE_MATCH },
        { 'X', CigarOperationType::SEQUENCE_MISMATCH },
        { 'I', CigarOperationType::INSERTION },
        { 'D', CigarOperationType::DELETION },
        { 'N', CigarOperationType::REFERENCE_SKIP },
        { 'H', CigarOperationType::HARD_CLIP },
        { 'P', CigarOperationType::PADDING },
        { 'M', CigarOperationType::ALIGNMENT_MATCH }
    };
    // clang-format on

    const auto found = lookup.find(c);
    if (found == lookup.cend()) {
        std::stringstream s;
        s << "pbcopper: unrecognized CIGAR char code " << c << " (int: " << static_cast<int>(c)
          << ")";
        throw std::runtime_error(s.str());
    } else
        return found->second;
}

char CigarOperation::TypeToChar(const CigarOperationType type)
{
    using Hash = PacBio::Utility::EnumClassHash;

    // clang-format off
    static const auto lookup = std::unordered_map<CigarOperationType, char, Hash>
    {
        { CigarOperationType::SOFT_CLIP,         'S' },
        { CigarOperationType::SEQUENCE_MATCH,    '=' },
        { CigarOperationType::SEQUENCE_MISMATCH, 'X' },
        { CigarOperationType::INSERTION,         'I' },
        { CigarOperationType::DELETION,          'D' },
        { CigarOperationType::REFERENCE_SKIP,    'N' },
        { CigarOperationType::HARD_CLIP,         'H' },
        { CigarOperationType::PADDING,           'P' },
        { CigarOperationType::ALIGNMENT_MATCH,   'M' }
    };
    // clang-format on

    const auto found = lookup.find(type);
    if (found == lookup.cend()) {
        std::stringstream s;
        s << "pbcopper: unrecognized CIGAR operation value " << static_cast<int>(type);
        throw std::runtime_error(s.str());
    } else
        return found->second;
}

std::istream& operator>>(std::istream& in, CigarOperation& op)
{
    uint32_t l;
    char c;

    in >> l >> c;

    op.Length(l);
    op.Char(c);
    return in;
}

std::ostream& operator<<(std::ostream& out, const CigarOperation& op)
{
    out << op.Length() << op.Char();
    return out;
}

}  // namespace Data
}  // namespace PacBio
