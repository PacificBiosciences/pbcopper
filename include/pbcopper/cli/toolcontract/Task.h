// Author: Derek Barnett

#ifndef PBCOPPER_CLI_TOOLCONTRACT_TASK_H
#define PBCOPPER_CLI_TOOLCONTRACT_TASK_H

#include <pbcopper/PbcopperConfig.h>

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include <pbcopper/cli/toolcontract/InputFileType.h>
#include <pbcopper/cli/toolcontract/OutputFileType.h>
#include <pbcopper/cli/toolcontract/ResourceType.h>
#include <pbcopper/cli/toolcontract/TaskType.h>

namespace PacBio {
namespace CLI {
namespace ToolContract {

///
/// \brief The Task class
///
class Task
{
public:
    /// Used to indicate that the maximum number of available processors, as
    /// determined by the tool contract resolving system, should be used for
    /// this task.
    ///
    /// The actual integer value (zero) is not used; instead this should be
    /// considered more of a "sentinel" value.
    ///
    static const uint16_t MAX_NPROC = 0;

public:
    /// \name Constructors & Related Methods
    /// \{

    ///
    /// \brief ToolContractTask
    /// \param taskId
    ///
    explicit Task(std::string taskId);

    Task() = default;
    Task(const Task&) = default;
    Task(Task&&) noexcept = default;
    Task& operator=(const Task&) = default;
    Task& operator=(Task&&) noexcept = default;
    ~Task() = default;

    /// \}

public:
    /// \name Attributes
    /// \{

    ///
    /// \brief Description
    /// \return
    ///
    const std::string& Description() const;

    ///
    /// \brief IsDistributed
    /// \return
    ///
    bool IsDistributed() const;

    ///
    /// \brief NumProcessors
    ///
    /// Default is 1.
    ///
    /// \return
    ///
    uint16_t NumProcessors() const;

    ///
    /// \brief TaskId
    /// \return
    ///
    const std::string& TaskId() const;

    ///
    /// \brief Type
    /// \return
    ///
    TaskType Type() const;

    /// \}

public:
    /// \name Options
    /// \{

    std::map<std::string, std::string> Options() const;

    /// \}

public:
    /// \name File & Resource Types
    /// \{

    ///
    /// \brief InputFileTypes
    /// \return
    ///
    const std::vector<InputFileType>& InputFileTypes() const;

    ///
    /// \brief InputFilesToOptions
    /// \return
    ///
    const std::map<size_t, std::string>& InputFilesToOptions() const;

    ///
    /// \brief OutputFileTypes
    /// \return
    ///
    const std::vector<OutputFileType>& OutputFileTypes() const;

    ///
    /// \brief OutputFilesToOptions
    /// \return
    ///
    const std::map<size_t, std::string>& OutputFilesToOptions() const;

    ///
    /// \brief ResourceTypes
    /// \return
    ///
    const std::vector<ResourceType>& ResourceTypes() const;

    /// \}

public:
    /// \name Attributes
    /// \{

    ///
    /// \brief Description
    /// \param description
    /// \return
    ///
    Task& Description(std::string description);

    ///
    /// \brief NumProcessors
    ///
    /// Set an explicit number of processors for this task (default is 1).
    ///
    /// Task::MAX_NPROC can be used here to let the tool contract resolving
    /// system determine the actual processor count. Using \p nProc of 0 yields
    /// the same effect.
    ///
    /// \param nProc
    /// \return
    ///
    Task& NumProcessors(const uint16_t nProc);

    ///
    /// \brief SetDistributed
    /// \param ok
    /// \return
    ///
    Task& SetDistributed(const bool ok = true);

    ///
    /// \brief Type
    /// \param type
    /// \return
    ///
    Task& Type(const TaskType& type);

    /// \}

public:
    /// \name Options
    /// \{

    ///
    /// \brief Options
    /// \param optionConfigs
    /// \return
    ///
    Task& Options(std::map<std::string, std::string> optionConfigs);

    ///
    /// \brief Add single option
    /// \param OptionConfig
    /// \return
    ///
    Task& AddOption(std::pair<std::string, std::string> optionConfig);

    /// \}

public:
    /// \name File & Resource Types
    /// \{

    /// \brief Sets the task's input files list to provided list.
    ///
    /// \param   inputFileTypes
    /// \returns reference to this task
    ///
    Task& InputFileTypes(std::vector<InputFileType> inputFileTypes);

    ///
    /// \brief OutputFilesToOptions
    ///
    /// Set a mapping for RTC's input_files[] JSON array to named command-line
    /// options. Key is the position in the array, and value is the option's ID.
    ///
    /// \param mapping
    /// \return
    ///
    Task& InputFilesToOptions(std::map<size_t, std::string> mapping);

    /// \brief Sets the task's output files list to provided list.
    ///
    /// \param   outputFileTypes
    /// \returns reference to this taskn
    ///
    Task& OutputFileTypes(std::vector<OutputFileType> outputFileTypes);

    ///
    /// \brief OutputFilesToOptions
    ///
    /// Set a mapping for RTC's output_files[] JSON array to named command-line
    /// options. Key is the position in the array, and value is the option's ID.
    ///
    /// \param mapping
    /// \return
    ///
    Task& OutputFilesToOptions(std::map<size_t, std::string> mapping);

    /// \brief Sets the task's resource list to provided list.
    ///
    /// \param   resourceTypes
    /// \returns reference to this task
    ///
    Task& ResourceTypes(std::vector<ResourceType> resourceTypes);

    /// \}

private:
    // attributes
    std::string id_;
    std::string description_;
    uint16_t nProc_;
    bool isDistributed_;
    TaskType type_;

    // registered task options
    std::map<std::string, std::string> optionDisplayNames_;

    // files & resources
    std::vector<InputFileType> inputFileTypes_;
    std::vector<OutputFileType> outputFileTypes_;
    std::vector<ResourceType> resourceTypes_;

    // file -> option mappings
    std::map<size_t, std::string> inputFilesToOptions_;
    std::map<size_t, std::string> outputFilesToOptions_;
};

}  // namespace ToolContract
}  // namespace CLI
}  // namespace PacBio

#include <pbcopper/cli/toolcontract/internal/Task-inl.h>

#endif  // PBCOPPER_CLI_TOOLCONTRACT_TASK_H
