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

#ifndef PBCOPPER_CLI_TOOLCONTRACT_DRIVER_H
#define PBCOPPER_CLI_TOOLCONTRACT_DRIVER_H

#include <string>
#include <unordered_map>

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace CLI {
namespace ToolContract {

///
/// \brief The ToolDriver class
///
class Driver
{
public:
    typedef std::unordered_map<std::string, std::string> Environment;

public:
    /// \name Constructors & Related Methods
    /// \{

    ///
    /// \brief ToolDriver
    ///
    Driver(void);

    ///
    /// \brief ToolDriver
    /// \param exe
    ///
    Driver(const std::string& exe);

    ///
    /// \brief ToolDriver
    /// \param exe
    /// \param env
    /// \param serialization
    ///
    Driver(const std::string& exe,
               const Environment& env,
               const std::string& serialization);

    Driver(const Driver& other) = default;
    Driver(Driver&& other) = default;
    Driver& operator=(const Driver& other) = default;
    Driver& operator=(Driver&& other) = default;
    ~Driver(void) = default;

    /// \}

public:
    /// \name Attributes
    /// \{

    ///
    /// \brief Env
    /// \return
    ///
    const Environment& Env(void) const;

    ///
    /// \brief Exe
    /// \return
    ///
    const std::string& Exe(void) const;

    ///
    /// \brief Serialization
    /// \return
    ///
    const std::string& Serialization(void) const;

    /// \}

public:
    /// \name Attributes
    /// \{

    ///
    /// \brief Env
    /// \param env
    /// \return
    ///
    Driver& Env(const Environment& env);

    ///
    /// \brief Exe
    /// \param exe
    /// \return
    ///
    Driver& Exe(const std::string& exe);

    ///
    /// \brief Serialization
    /// \param serialization
    /// \return
    ///
    Driver& Serialization(const std::string& serialization);

    /// \}

private:
    std::string exe_;
    std::string serialization_;
    std::unordered_map<std::string, std::string> env_;
};

} // namespace ToolContract
} // namespace CLI
} // namespace PacBio

#include <pbcopper/cli/toolcontract/internal/Driver-inl.h>

#endif // PBCOPPER_CLI_TOOLCONTRACT_DRIVER_H
