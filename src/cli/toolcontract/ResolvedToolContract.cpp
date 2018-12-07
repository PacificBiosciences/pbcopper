// Author: Derek Barnett

#include <pbcopper/cli/toolcontract/ResolvedToolContract.h>

#include <pbcopper/json/JSON.h>
#include <pbcopper/utility/StringUtils.h>

namespace PacBio {
namespace CLI {
namespace ToolContract {

class ResolvedToolContract::ResolvedToolContractPrivate
{
public:
    Interface interface_;

    explicit ResolvedToolContractPrivate(Interface interface) : interface_{std::move(interface)} {}
};

ResolvedToolContract::ResolvedToolContract(Interface interface)
    : d_{std::make_unique<ResolvedToolContractPrivate>(std::move(interface))}
{
}

ResolvedToolContract::~ResolvedToolContract() {}

Results ResolvedToolContract::Parse(std::istream& in)
{
    using Json = PacBio::JSON::Json;

    const auto& interface = d_->interface_;
    const auto& task = interface.ToolContract().task_;

    Results results{d_->interface_};
    results.SetFromRTC(true);

    Json root = Json::parse(in);
    Json rtc = root["resolved_tool_contract"];

    // options (object)
    const Json options = rtc["options"];
    for (auto iter = options.cbegin(); iter != options.cend(); ++iter) {

        const auto& fullId = iter.key();
        const Json& value = iter.value();

        const auto idFields = PacBio::Utility::Split(fullId, '.');
        if (idFields.size() != 3)
            throw std::runtime_error("PacBio::CLI - unexpected option ID format: " + fullId);
        const auto optionId = idFields.at(2);

        results.RegisterOptionValue(optionId, value);
    }

    // log_level
    const Json log_level = rtc["log_level"];
    const std::string logLevelString = log_level;
    results.LogLevel(PacBio::Logging::LogLevel{logLevelString});

    // nproc
    const Json nproc = rtc["nproc"];
    results.NumProcessors(nproc);

    // input file (array)
    const Json inputFiles = rtc["input_files"];
    const auto inputFilesToOptions = task.InputFilesToOptions();
    const auto inputFilesToOptionsEnd = inputFilesToOptions.cend();
    for (size_t i = 0; i < inputFiles.size(); ++i) {
        const auto inputFile = inputFiles.at(i);
        const auto inputFilesToOptionsIter = inputFilesToOptions.find(i);
        if (inputFilesToOptionsIter == inputFilesToOptionsEnd)
            results.RegisterPositionalArg(inputFile);
        else {
            const auto optionId = inputFilesToOptionsIter->second;
            results.RegisterOptionValue(optionId, inputFile);
        }
    }

    // output file (array)
    const Json outputFiles = rtc["output_files"];
    const auto outputFilesToOptions = task.OutputFilesToOptions();
    const auto outputFilesToOptionsEnd = outputFilesToOptions.cend();
    for (size_t i = 0; i < outputFiles.size(); ++i) {
        const auto outputFile = outputFiles.at(i);
        const auto outputFilesToOptionsIter = outputFilesToOptions.find(i);
        if (outputFilesToOptionsIter == outputFilesToOptionsEnd)
            results.RegisterPositionalArg(outputFile);
        else {
            const auto optionId = outputFilesToOptionsIter->second;
            results.RegisterOptionValue(optionId, outputFile);
        }
    }

    return results;
}

}  // namespace ToolContract
}  // namespace CLI
}  // namespace PacBio
