#include "pbcopper/cli/toolcontract/ResolvedToolContract.h"

#include "pbcopper/json/JSON.h"
#include "pbcopper/utility/StringUtils.h"

using namespace PacBio;
using namespace PacBio::CLI;
using namespace PacBio::CLI::ToolContract;
using namespace PacBio::JSON;
using namespace std;

namespace PacBio {
namespace CLI {
namespace ToolContract {
namespace internal {

class RtcPrivate
{

public:
    const Interface interface_;

public:
    RtcPrivate(const Interface& interface)
        : interface_(interface)
    { }
};


} // namespace internal
} // namespace ToolContract
} // namespace CLI
} // namespace PacBio

ResolvedToolContract::ResolvedToolContract(const Interface& interface)
    : d_(new internal::RtcPrivate{ interface })
{ }

ResolvedToolContract::~ResolvedToolContract(void) { }

Results ResolvedToolContract::Parse(istream& in)
{
    Results results{ d_->interface_ };

    Json root(in);
    Json rtc = root["resolved_tool_contract"];

    // options (object)
    const Json options = rtc["options"];
    for (auto iter = options.cbegin(); iter != options.cend(); ++iter) {

        const auto& fullId = iter.key();
        const Json& value = iter.value();

        const auto idFields = PacBio::Utility::Split(fullId, '.');
        if (idFields.size() != 3)
            throw std::runtime_error("PacBio::CLI - unexpected option ID format: "+fullId);
        const auto optionId = idFields.at(2);

        results.RegisterOptionValue(optionId, value);
    }

    // input file (array)
    const Json inputFiles = rtc["input_files"];
    for (const auto& inputFile : inputFiles)
        results.RegisterPositionalArg(inputFile);

    // output file (array)
    const Json outputFiles = rtc["output_files"];
    for (const auto& outputFile : outputFiles)
        results.RegisterPositionalArg(outputFile);

    return results;
}
