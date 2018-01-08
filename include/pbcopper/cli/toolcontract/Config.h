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

#ifndef PBCOPPER_CLI_TOOLCONTRACT_CONFIG_H
#define PBCOPPER_CLI_TOOLCONTRACT_CONFIG_H

#include <string>

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/cli/toolcontract/Task.h>
#include <pbcopper/cli/toolcontract/Driver.h>

namespace PacBio {
namespace CLI {
namespace ToolContract {

///
/// \brief The ToolContractConfig class
///
class Config
{
public:
    /// \name Constructors & Related Methods
    /// \{

    /// \brief Contructs a tool contract configuration with the provided task
    ///        settings and default driver settings.
    ///
    /// \param task
    ///
    Config(const ToolContract::Task& task);

    /// \brief Contructs a tool contract configuration with the provided task &
    ///        driver settings.
    ///
    /// \param task
    /// \param driver
    ///
    Config(const ToolContract::Task& task,
           const ToolContract::Driver& driver);

    Config(const Config& other) = default;
    Config(Config&& other) = default;
    Config& operator=(const Config& other) = default;
    Config& operator=(Config&& other) = default;
    ~Config(void) = default;

    /// \}

public:
    /// \name Main Components
    /// \{

    ///
    /// \brief Driver
    /// \return
    ///
    const ToolContract::Driver& Driver(void) const;

    ///
    /// \brief Task
    /// \return
    ///
    const ToolContract::Task& Task(void) const;

    /// \}

public:
    /// \name Other Attributes
    /// \{

    std::string Version(void) const;

    /// \}

public:
    Config& Version(const std::string& version);

private:
    ToolContract::Task task_;
    ToolContract::Driver driver_;
    std::string version_;
};

} // namespace ToolContract
} // namespace CLI
} // namespace PacBio

#include <pbcopper/cli/toolcontract/internal/Config-inl.h>

#endif // PBCOPPER_CLI_TOOLCONTRACT_CONFIG_H
