#ifndef PBCOPPER_CLI_TOOLCONTRACT_TASK_H
#define PBCOPPER_CLI_TOOLCONTRACT_TASK_H

#include "pbcopper/Config.h"
#include "pbcopper/cli/toolcontract/InputFileType.h"
#include "pbcopper/cli/toolcontract/OutputFileType.h"
#include "pbcopper/cli/toolcontract/ResourceType.h"
#include "pbcopper/cli/toolcontract/TaskType.h"
#include <map>
#include <string>
#include <vector>

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
    Task(const std::string& taskId);

    Task(const Task&) = default;
    ~Task(void) = default;

    /// \}

public:
    /// \name Attributes
    /// \{

    ///
    /// \brief Description
    /// \return
    ///
    std::string Description(void) const;

    ///
    /// \brief IsDistributed
    /// \return
    ///
    bool IsDistributed(void) const;

    ///
    /// \brief NumProcessors
    ///
    /// Default is 1.
    ///
    /// \return
    ///
    uint16_t NumProcessors(void) const;


    ///
    /// \brief TaskId
    /// \return
    ///
    const std::string& TaskId(void) const;

    ///
    /// \brief Type
    /// \return
    ///
    TaskType Type(void) const;

    /// \}

public:
    /// \name Options
    /// \{

    std::map<std::string, std::string> Options(void) const;

    /// \}

public:
    /// \name File & Resource Types
    /// \{

    ///
    /// \brief InputFileTypes
    /// \return
    ///
    std::vector<InputFileType> InputFileTypes(void) const;

    ///
    /// \brief InputFilesToOptions
    /// \return
    ///
    std::map<size_t, std::string> InputFilesToOptions(void) const;

    ///
    /// \brief OutputFileTypes
    /// \return
    ///
    std::vector<OutputFileType> OutputFileTypes(void) const;

    ///
    /// \brief OutputFilesToOptions
    /// \return
    ///
    std::map<size_t, std::string> OutputFilesToOptions(void) const;

    ///
    /// \brief ResourceTypes
    /// \return
    ///
    std::vector<ResourceType> ResourceTypes(void) const;

    /// \}

public:
    /// \name Attributes
    /// \{

    ///
    /// \brief Description
    /// \param description
    /// \return
    ///
    Task& Description(const std::string& description);

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
    Task& Options(const std::map<std::string, std::string>& optionConfigs);

    ///
    /// \brief Add single option
    /// \param OptionConfig
    /// \return
    ///
    Task& AddOption(const std::pair<std::string, std::string>& optionConfig);

    /// \}

public:
    /// \name File & Resource Types
    /// \{

    /// \brief Sets the task's input files list to provided list.
    ///
    /// \param   inputFileTypes
    /// \returns reference to this task
    ///
    Task& InputFileTypes(const std::vector<InputFileType>& inputFileTypes);

    ///
    /// \brief OutputFilesToOptions
    ///
    /// Set a mapping for RTC's input_files[] JSON array to named command-line
    /// options. Key is the position in the array, and value is the option's ID.
    ///
    /// \param mapping
    /// \return
    ///
    Task& InputFilesToOptions(const std::map<size_t, std::string>& mapping);

    /// \brief Sets the task's output files list to provided list.
    ///
    /// \param   outputFileTypes
    /// \returns reference to this taskn
    ///
    Task& OutputFileTypes(const std::vector<OutputFileType>& outputFileTypes);

    ///
    /// \brief OutputFilesToOptions
    ///
    /// Set a mapping for RTC's output_files[] JSON array to named command-line
    /// options. Key is the position in the array, and value is the option's ID.
    ///
    /// \param mapping
    /// \return
    ///
    Task& OutputFilesToOptions(const std::map<size_t, std::string>& mapping);

    /// \brief Sets the task's resource list to provided list.
    ///
    /// \param   resourceTypes
    /// \returns reference to this task
    ///
    Task& ResourceTypes(const std::vector<ResourceType>& resourceTypes);

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
    std::vector<InputFileType>  inputFileTypes_;
    std::vector<OutputFileType> outputFileTypes_;
    std::vector<ResourceType>   resourceTypes_;

    // file -> option mappings
    std::map<size_t, std::string> inputFilesToOptions_;
    std::map<size_t, std::string> outputFilesToOptions_;
};

} // namespace ToolContract
} // namespace CLI
} // namespace PacBio

#include "internal/Task-inl.h"

#endif // PBCOPPER_CLI_TOOLCONTRACT_TASK_H
