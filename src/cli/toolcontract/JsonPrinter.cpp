// Author: Derek Barnett

#include <pbcopper/cli/toolcontract/JsonPrinter.h>

#include <stdexcept>
#include <unordered_map>

#include <pbcopper/cli/Interface.h>
#include <pbcopper/json/JSON.h>
#include <pbcopper/utility/PbcopperVersion.h>

using Json = PacBio::JSON::Json;

namespace PacBio {
namespace CLI {
namespace ToolContract {
namespace {

Option RegisteredOption(const Interface& interface, const std::string& optionId)
{
    const auto registeredOptions = interface.RegisteredOptions();
    for (const Option& opt : registeredOptions) {
        if (opt.Id() == optionId) return opt;
    }
    throw std::runtime_error("PacBio::CLI - could not find option with id: " + optionId);
}

Json currentSchemaVersion() { return Json("2.0.0"); }

Json makeDriverJson(const Interface& interface)
{
    const auto& driver = interface.ToolContract().driver_;

    Json driverJson;

    Json envJson = Json::object();
    const auto env = driver.Env();
    auto envIter = env.cbegin();
    const auto envEnd = env.cend();
    for (; envIter != envEnd; ++envIter) {
        const auto& key = envIter->first;
        const auto& value = envIter->second;
        envJson[key] = value;
    }
    driverJson["env"] = envJson;

    auto exe = driver.Exe();
    if (exe.empty()) exe = interface.ApplicationName() + " --resolved-tool-contract";
    driverJson["exe"] = exe;

    auto serialization = driver.Serialization();
    if (serialization.empty()) serialization = "json";
    driverJson["serialization"] = serialization;

    return driverJson;
}

Json makeInputTypesJson(const Interface& interface)
{
    Json inputTypesJson = Json::array();

    const auto& task = interface.ToolContract().task_;
    const auto inputFileTypes = task.InputFileTypes();
    for (const InputFileType& file : inputFileTypes) {
        Json entry = Json::object();
        entry["title"] = file.Title();
        entry["id"] = file.Id();
        entry["file_type_id"] = file.Type();
        entry["description"] = file.Description();
        inputTypesJson.push_back(entry);
    }

    return inputTypesJson;
}

Json makeNproc(const ToolContract::Task& task)
{
    const auto numProcessors = task.NumProcessors();
    if (numProcessors == ToolContract::Task::MAX_NPROC)
        return Json("$max_nproc");
    else
        return Json(numProcessors);
}

Json makeOutputTypesJson(const Interface& interface)
{
    Json outputTypesJson = Json::array();

    const auto& task = interface.ToolContract().task_;
    const auto outputFileTypes = task.OutputFileTypes();
    for (const OutputFileType& file : outputFileTypes) {
        Json entry = Json::object();
        entry["title"] = file.Title();
        entry["id"] = file.Id();
        entry["file_type_id"] = file.Type();
        entry["default_name"] = file.DefaultName();
        entry["description"] = file.Description();
        outputTypesJson.push_back(entry);
    }

    return outputTypesJson;
}

Json makeResourceTypesJson(const ToolContract::Task& task)
{
    const std::unordered_map<ResourceType, std::string> lookup = {
        {ResourceType::LOG_FILE, "$logfile"},
        {ResourceType::TMP_FILE, "$tmpfile"},
        {ResourceType::TMP_DIR, "$tmpdir"},
        {ResourceType::OUTPUT_DIR, "$outputdir"}};

    Json resourceTypesJson = Json::array();

    const auto taskResourceTypes = task.ResourceTypes();
    for (const auto& t : taskResourceTypes) {
        const auto found = lookup.find(t);
        if (found == lookup.cend())
            throw std::runtime_error("CLI::ToolContract::JsonPrinter - unknown resource type");
        resourceTypesJson.push_back(found->second);
    }

    return resourceTypesJson;
}

Json makeSchemaOptionJson(const Interface& interface, const std::string& optionPrefix,
                          const std::pair<std::string, std::string>& taskOption)
{
    // format ID/display name
    const std::string& optionId = taskOption.first;
    const std::string& optionDisplayName = taskOption.second;
    const std::string fullOptionId = optionPrefix + ".task_options." + optionId;

    // get option info
    const Option registeredOption = RegisteredOption(interface, optionId);
    const JSON::Json defaultValue = registeredOption.DefaultValue();
    const std::string description = registeredOption.Description();
    const std::string optionType = registeredOption.TypeId();

    // populate JSON
    Json schemaOption = Json::object();
    schemaOption["default"] = defaultValue;
    schemaOption["description"] = description;
    schemaOption["id"] = fullOptionId;
    schemaOption["name"] = optionDisplayName;
    schemaOption["optionTypeId"] = optionType;

    if (registeredOption.HasChoices()) schemaOption["choices"] = registeredOption.Choices();

    return schemaOption;
}

Json makeSchemaOptionsJson(const Interface& interface)
{
    Json schemaOptions = Json::array();

    const auto optionPrefix = interface.AlternativeToolContractName().empty()
                                  ? interface.ApplicationName()
                                  : interface.AlternativeToolContractName();

    const auto& task = interface.ToolContract().task_;
    for (const auto& taskOption : task.Options())
        schemaOptions.push_back(makeSchemaOptionJson(interface, optionPrefix, taskOption));
    return schemaOptions;
}

std::string makeTaskType(const TaskType& type)
{
    switch (type) {
        case TaskType::STANDARD:
            return "pbsmrtpipe.task_types.standard";
        case TaskType::GATHERED:
            return "pbsmrtpipe.task_types.gathered";
        case TaskType::SCATTERED:
            return "pbsmrtpipe.task_types.scattered";
        default:
            throw std::runtime_error("CLI::ToolContract::JsonPrinter - unknown task type");
    }
}

Json makeTaskJson(const Interface& interface)
{
    Json tcJson;
    const auto& task = interface.ToolContract().task_;

    tcJson["_comment"] = "Created by v" + Utility::LibraryVersionString();
    tcJson["description"] = interface.ApplicationDescription();
    tcJson["name"] = interface.ApplicationName();
    tcJson["is_distributed"] = task.IsDistributed();
    tcJson["tool_contract_id"] = task.TaskId();

    tcJson["nproc"] = makeNproc(task);
    tcJson["task_type"] = makeTaskType(task.Type());
    tcJson["input_types"] = makeInputTypesJson(interface);
    tcJson["output_types"] = makeOutputTypesJson(interface);
    tcJson["resource_types"] = makeResourceTypesJson(task);
    tcJson["schema_options"] = makeSchemaOptionsJson(interface);

    return tcJson;
}

}  // namespace

void JsonPrinter::Print(const Interface& interface, std::ostream& out, const int indent)
{
    // build up JSON object from @interface
    Json result;
    result["driver"] = makeDriverJson(interface);
    result["schema_version"] = currentSchemaVersion();
    result["tool_contract"] = makeTaskJson(interface);
    result["tool_contract_id"] = interface.ToolContract().task_.TaskId();
    result["version"] = interface.ApplicationVersion();

    out << result.dump(indent);
}

}  // namespace ToolContract
}  // namespace CLI
}  // namespace PacBio
