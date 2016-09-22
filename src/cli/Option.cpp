
#include "pbcopper/cli/Option.h"
using namespace PacBio;
using namespace PacBio::CLI;
using namespace std;

namespace PacBio {
namespace CLI {
namespace internal {

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

} // namespace internal
} // namespace CLI
} // namespace PacBio

const Option& Option::DefaultHelpOption(void)
{ return internal::defaultHelpOption; }

const Option& Option::DefaultLogLevelOption(void)
{ return internal::defaultLogLevelOption; }

const Option& Option::DefaultVerboseOption(void)
{ return internal::defaultVerboseOption; }

const Option& Option::DefaultVersionOption(void)
{ return internal::defaultVersionOption; }
