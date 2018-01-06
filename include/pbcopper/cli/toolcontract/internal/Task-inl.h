#ifndef PBCOPPER_CLI_TOOLCONTRACT_TASK_INL_H
#define PBCOPPER_CLI_TOOLCONTRACT_TASK_INL_H

#include <pbcopper/cli/toolcontract/Task.h>

namespace PacBio {
namespace CLI {
namespace ToolContract {

inline Task::Task(const std::string& taskId)
    : id_(taskId)
    , nProc_(1)
    , isDistributed_(true)
    , type_(TaskType::STANDARD)
{ }

inline std::string Task::Description(void) const
{ return description_; }

inline Task& Task::Description(const std::string& description)
{ description_ = description; return *this; }

inline std::vector<InputFileType> Task::InputFileTypes(void) const
{ return inputFileTypes_; }

inline Task& Task::InputFileTypes(const std::vector<InputFileType>& inputFileTypes)
{ inputFileTypes_ = inputFileTypes; return *this; }

inline std::map<size_t, std::string> Task::InputFilesToOptions(void) const
{ return inputFilesToOptions_; }

inline Task& Task::InputFilesToOptions(const std::map<size_t, std::string>& mapping)
{ inputFilesToOptions_ = mapping; return *this; }

inline bool Task::IsDistributed(void) const
{ return isDistributed_; }

inline uint16_t Task::NumProcessors(void) const
{ return nProc_; }

inline Task& Task::NumProcessors(const uint16_t nProc)
{ nProc_ = nProc; return *this; }

inline std::map<std::string, std::string> Task::Options(void) const
{ return optionDisplayNames_; }

inline Task& Task::Options(const std::map<std::string, std::string>& optionDisplayNames)
{ optionDisplayNames_ = optionDisplayNames; return *this; }

inline Task& Task::AddOption(const std::pair<std::string, std::string>& optionConfig)
{ optionDisplayNames_.insert(optionConfig); return *this; }

inline std::vector<OutputFileType> Task::OutputFileTypes(void) const
{ return outputFileTypes_; }

inline std::map<size_t, std::string> Task::OutputFilesToOptions(void) const
{ return outputFilesToOptions_; }

inline Task& Task::OutputFilesToOptions(const std::map<size_t, std::string>& mapping)
{ outputFilesToOptions_ = mapping; return *this; }

inline Task& Task::OutputFileTypes(const std::vector<OutputFileType>& outputFileTypes)
{ outputFileTypes_ = outputFileTypes; return *this; }

inline std::vector<ResourceType> Task::ResourceTypes(void) const
{ return resourceTypes_; }

inline Task& Task::ResourceTypes(const std::vector<ResourceType>& resourceTypes)
{ resourceTypes_ = resourceTypes; return *this; }

inline Task& Task::SetDistributed(const bool ok)
{ isDistributed_ = ok; return *this; }

inline const std::string& Task::TaskId(void) const
{ return id_; }

inline TaskType Task::Type(void) const
{ return type_; }

inline Task& Task::Type(const TaskType& type)
{ type_ = type; return *this; }

} // namespace ToolContract
} // namespace CLI
} // namespace PacBio

#endif // PBCOPPER_CLI_TOOLCONTRACT_TASK_INL_H
