// Author: Derek Barnett

#ifndef PBCOPPER_CLI_TOOLCONTRACT_TASK_INL_H
#define PBCOPPER_CLI_TOOLCONTRACT_TASK_INL_H

#include <pbcopper/cli/toolcontract/Task.h>

namespace PacBio {
namespace CLI {
namespace ToolContract {

inline Task::Task(std::string taskId)
    : id_{std::move(taskId)}, nProc_{1}, isDistributed_{true}, type_{TaskType::STANDARD}
{
}

inline const std::string& Task::Description() const { return description_; }

inline Task& Task::Description(std::string description)
{
    description_ = std::move(description);
    return *this;
}

inline const std::vector<InputFileType>& Task::InputFileTypes() const { return inputFileTypes_; }

inline Task& Task::InputFileTypes(std::vector<InputFileType> inputFileTypes)
{
    inputFileTypes_ = std::move(inputFileTypes);
    return *this;
}

inline const std::map<size_t, std::string>& Task::InputFilesToOptions() const
{
    return inputFilesToOptions_;
}

inline Task& Task::InputFilesToOptions(std::map<size_t, std::string> mapping)
{
    inputFilesToOptions_ = std::move(mapping);
    return *this;
}

inline bool Task::IsDistributed() const { return isDistributed_; }

inline uint16_t Task::NumProcessors() const { return nProc_; }

inline Task& Task::NumProcessors(const uint16_t nProc)
{
    nProc_ = nProc;
    return *this;
}

inline std::map<std::string, std::string> Task::Options() const { return optionDisplayNames_; }

inline Task& Task::Options(std::map<std::string, std::string> optionDisplayNames)
{
    optionDisplayNames_ = std::move(optionDisplayNames);
    return *this;
}

inline Task& Task::AddOption(std::pair<std::string, std::string> optionConfig)
{
    optionDisplayNames_.insert(std::move(optionConfig));
    return *this;
}

inline const std::vector<OutputFileType>& Task::OutputFileTypes() const { return outputFileTypes_; }

inline const std::map<size_t, std::string>& Task::OutputFilesToOptions() const
{
    return outputFilesToOptions_;
}

inline Task& Task::OutputFilesToOptions(std::map<size_t, std::string> mapping)
{
    outputFilesToOptions_ = std::move(mapping);
    return *this;
}

inline Task& Task::OutputFileTypes(std::vector<OutputFileType> outputFileTypes)
{
    outputFileTypes_ = std::move(outputFileTypes);
    return *this;
}

inline const std::vector<ResourceType>& Task::ResourceTypes() const { return resourceTypes_; }

inline Task& Task::ResourceTypes(std::vector<ResourceType> resourceTypes)
{
    resourceTypes_ = std::move(resourceTypes);
    return *this;
}

inline Task& Task::SetDistributed(const bool ok)
{
    isDistributed_ = ok;
    return *this;
}

inline const std::string& Task::TaskId() const { return id_; }

inline TaskType Task::Type() const { return type_; }

inline Task& Task::Type(const TaskType& type)
{
    type_ = type;
    return *this;
}

}  // namespace ToolContract
}  // namespace CLI
}  // namespace PacBio

#endif  // PBCOPPER_CLI_TOOLCONTRACT_TASK_INL_H
