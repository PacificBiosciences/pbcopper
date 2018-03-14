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

#ifndef PBCOPPER_CLI_TOOLCONTRACT_TASK_INL_H
#define PBCOPPER_CLI_TOOLCONTRACT_TASK_INL_H

#include <pbcopper/cli/toolcontract/Task.h>

namespace PacBio {
namespace CLI {
namespace ToolContract {

inline Task::Task(const std::string& taskId)
    : id_(taskId), nProc_(1), isDistributed_(true), type_(TaskType::STANDARD)
{
}

inline std::string Task::Description(void) const { return description_; }

inline Task& Task::Description(const std::string& description)
{
    description_ = description;
    return *this;
}

inline std::vector<InputFileType> Task::InputFileTypes(void) const { return inputFileTypes_; }

inline Task& Task::InputFileTypes(const std::vector<InputFileType>& inputFileTypes)
{
    inputFileTypes_ = inputFileTypes;
    return *this;
}

inline std::map<size_t, std::string> Task::InputFilesToOptions(void) const
{
    return inputFilesToOptions_;
}

inline Task& Task::InputFilesToOptions(const std::map<size_t, std::string>& mapping)
{
    inputFilesToOptions_ = mapping;
    return *this;
}

inline bool Task::IsDistributed(void) const { return isDistributed_; }

inline uint16_t Task::NumProcessors(void) const { return nProc_; }

inline Task& Task::NumProcessors(const uint16_t nProc)
{
    nProc_ = nProc;
    return *this;
}

inline std::map<std::string, std::string> Task::Options(void) const { return optionDisplayNames_; }

inline Task& Task::Options(const std::map<std::string, std::string>& optionDisplayNames)
{
    optionDisplayNames_ = optionDisplayNames;
    return *this;
}

inline Task& Task::AddOption(const std::pair<std::string, std::string>& optionConfig)
{
    optionDisplayNames_.insert(optionConfig);
    return *this;
}

inline std::vector<OutputFileType> Task::OutputFileTypes(void) const { return outputFileTypes_; }

inline std::map<size_t, std::string> Task::OutputFilesToOptions(void) const
{
    return outputFilesToOptions_;
}

inline Task& Task::OutputFilesToOptions(const std::map<size_t, std::string>& mapping)
{
    outputFilesToOptions_ = mapping;
    return *this;
}

inline Task& Task::OutputFileTypes(const std::vector<OutputFileType>& outputFileTypes)
{
    outputFileTypes_ = outputFileTypes;
    return *this;
}

inline std::vector<ResourceType> Task::ResourceTypes(void) const { return resourceTypes_; }

inline Task& Task::ResourceTypes(const std::vector<ResourceType>& resourceTypes)
{
    resourceTypes_ = resourceTypes;
    return *this;
}

inline Task& Task::SetDistributed(const bool ok)
{
    isDistributed_ = ok;
    return *this;
}

inline const std::string& Task::TaskId(void) const { return id_; }

inline TaskType Task::Type(void) const { return type_; }

inline Task& Task::Type(const TaskType& type)
{
    type_ = type;
    return *this;
}

}  // namespace ToolContract
}  // namespace CLI
}  // namespace PacBio

#endif  // PBCOPPER_CLI_TOOLCONTRACT_TASK_INL_H
