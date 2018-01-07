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

#ifndef PBCOPPER_CLI_CLI_H
#define PBCOPPER_CLI_CLI_H

#include <functional>
#include <iosfwd>
#include <string>
#include <vector>

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/cli/Interface.h>
#include <pbcopper/cli/Results.h>

/// So applications's main.cpp contains something like:
///
/// #include "BlasrSettings.h" // for settings struct
/// #include "BlasrEntry.h"    // app entry point that takes the settings & starts working
///
/// static PacBio::CLI::Interface makeBlasrInterface(void)
/// {
///     // sets up interface object with options
/// }
///
/// static int blasrRunner(const PacBio::CLI::Results& args)
/// {
///     // extract options from args -> settings
///     //
///     // if interface was configured w/ tool contract support, args may be from
///     // cmd-line OR resolved tool contract, but the application shouldn't actually
///     // care or be concerned with that
///     //
///
///     BlasrSettings s;
///     s.foo = args["foo"];
///     // ...
///
///     // start the application's real work & return success/fail, a la main()
///     return blasrEntry(s);
/// }
///
/// int main(int argc, char* argv[])
/// {
///     return PacBio::CLI::Run(argc, argv,
///                             makeBlasrInterface(),
///                             &blasrRunner);
/// }
///

namespace PacBio {
namespace CLI {

///
/// \brief ResultsHandler
///
typedef std::function<int(const PacBio::CLI::Results&)> ResultsHandler;

///
/// \brief PrintHelp
/// \param interface
/// \param out
///
void PrintHelp(const Interface& interface, std::ostream& out);

///
/// \brief PrintVersion
/// \param interface
/// \param out
///
void PrintVersion(const Interface& interface, std::ostream& out);

// TODO: add logging input here

///
/// \brief Run
/// \param argc
/// \param argv
/// \param interface
/// \param handler
/// \return
///
int Run(int argc, char* argv[],
        const Interface& interface,
        const ResultsHandler& handler);

///
/// \brief Run
/// \param args
/// \param interface
/// \param handler
/// \return
///
int Run(const std::vector<std::string>& args,
        const Interface& interface,
        const ResultsHandler& handler);

} // namespace CLI
} // namespace PacBio

#endif // PBCOPPER_CLI_CLI_H
