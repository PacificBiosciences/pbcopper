// Author: Derek Barnett

#include <pbcopper/cli/Option.h>

#include <stdexcept>

namespace PacBio {
namespace CLI {
namespace {

// clang-format off
static const Option defaultHelpOption {
    "help",
    {"h", "help"},
    "Output this help."
};

static const Option defaultLogLevelOption {
    "log_level",
    {"log-level", "logLevel"},
    "Set log level.",
    Option::StringType("INFO")
};

static const Option defaultVerboseOption {
    "verbose",
    {"v", "verbose"},
    "Use verbose output."
};

static const Option defaultVersionOption {
    "version",
    "version",
    "Output version info."
};
// clang-format on

}  // anonymous

const Option& Option::DefaultHelpOption() { return defaultHelpOption; }

const Option& Option::DefaultLogLevelOption() { return defaultLogLevelOption; }

const Option& Option::DefaultVerboseOption() { return defaultVerboseOption; }

const Option& Option::DefaultVersionOption() { return defaultVersionOption; }

std::string Option::TypeId() const
{
    using Json = PacBio::JSON::Json;

    const bool hasChoices = HasChoices();
    const auto type = DefaultValue().type();
    switch (type) {
        case Json::value_t::number_integer:  // fall through
        case Json::value_t::number_unsigned:
            return (hasChoices ? "choice_integer" : "integer");

        case Json::value_t::number_float:
            return (hasChoices ? "choice_float" : "float");
        case Json::value_t::string:
            return (hasChoices ? "choice_string" : "string");
        case Json::value_t::boolean:
            return "boolean";
        default:
            throw std::runtime_error{
                "PacBio::CLI::ToolContract::JsonPrinter - unsupported type for option: " + Id()};
    }
}

}  // namespace CLI
}  // namespace PacBio
