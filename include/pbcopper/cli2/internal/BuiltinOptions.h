// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_BUILTINOPTIONS_H
#define PBCOPPER_CLI_v2_BUILTINOPTIONS_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/cli2/Option.h>

// clang-format off

namespace PacBio {
namespace CLI_v2 {
namespace Builtin {

const Option Help{
R"({
    "names" : ["h", "help"],
    "description" : "Show this help and exit."
})"};

const Option LogLevel{
R"({
    "names" : ["log-level"],
    "names.hidden" : ["logLevel"],
    "description" : "Set log level.",
    "type" : "string",
    "default" : "WARN",
    "choices" : ["TRACE", "DEBUG", "INFO", "WARN", "FATAL"]
})"};

const Option LogFile{
R"({
    "names" : ["log-file"],
    "names.hidden" : ["logFile"],
    "description" : "Log to a file, instead of stderr.",
    "type" : "file",
    "default" : ""
})"};

const Option NumThreads{
R"({
    "names" : ["j", "num-threads"],
    "description" : "Number of threads to use, 0 means autodetection.",
    "type" : "unsigned int",
    "default" : 0
})"};

const Option Version{
R"({
    "names" : ["version"],
    "description" : "Show application version and exit."
})"};

const Option Verbose{
R"({
    "names" : ["v", "verbose"],
    "description" : "Use verbose output."
})"};

} // namespace Builtin
} // namespace CLI_v2
} // namespace PacBio

// clang-format on

#endif  // PBCOPPER_CLI_v2_BUILTINOPTIONS_H
