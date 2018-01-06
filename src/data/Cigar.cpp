// Copyright (c) 2016, Pacific Biosciences of California, Inc.
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
//
// File Description
/// \file Cigar.cpp
/// \brief Implements the Cigar class.
//
// Author: Derek Barnett

#include <pbcopper/data/Cigar.h>
#include <cctype>
#include <cstddef>
#include <sstream>

namespace PacBio {
namespace Data {

Cigar::Cigar(void) { }

Cigar::Cigar(const std::string& cigarString)
    : std::vector<CigarOperation>()
{
    size_t numberStart = 0;
    const auto numChars = cigarString.size();
    for (size_t i = 0; i < numChars; ++i)
    {
        const char c = cigarString.at(i);
        if (!isdigit(c))
        {
            const auto distance = i - numberStart;
            const auto length = std::stoul(cigarString.substr(numberStart, distance));
            emplace_back(c, length);
            numberStart = i+1;
        }
    }
}

std::string Cigar::ToStdString(void) const
{
    std::stringstream s;
    for (const auto& op : *this) {
        s << op.Length()
          << op.Char();
    }
    return s.str();
}

std::istream& operator>>(std::istream& in, Cigar& cigar)
{
    std::string s;
    in >> s;
    cigar = Cigar{ s };
    return in;
}

std::ostream& operator<<(std::ostream& out, const Cigar& cigar)
{
    out << cigar.ToStdString();
    return out;
}

} // namespace Data
} // namespace PacBio
