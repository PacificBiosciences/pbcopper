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

#ifndef PBCOPPER_CLI_TOOLCONTRACT_JSONPRINTER_H
#define PBCOPPER_CLI_TOOLCONTRACT_JSONPRINTER_H

#include <iosfwd>

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace CLI {

class Interface;

namespace ToolContract {

class JsonPrinter
{
public:

    /// \brief Prints a JSON-formatted tool contract.
    ///
    /// \param interface    Interface object to generate tool contract from
    /// \param out          destination output stream
    /// \param indent       If indent is non-negative, then array elements and
    ///                     object members will be pretty-printed with that
    ///                     indent level. An indent level of 0 will only insert
    ///                     newlines. Indent level of -1 selects the most
    ///                     compact representation.
    ///
    /// \throws std::runtime_error if failed to print
    ///
    static void Print(const Interface& interface,
                      std::ostream& out,
                      const int indent = 4);
};

} // namespace ToolContract
} // namespace CLI
} // namespace PacBio

#endif // PBCOPPER_TOOLCONTRACT_JSONPRINTER_H
