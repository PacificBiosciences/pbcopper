// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_OPTION_H
#define PBCOPPER_CLI_v2_OPTION_H

#include <pbcopper/PbcopperConfig.h>

#include <string>

#include <boost/optional.hpp>

#include <pbcopper/cli2/OptionValue.h>

namespace PacBio {
namespace CLI_v2 {

///
/// Defines properties for an interface option, via JSON-formatted text.
///
/// Required properties:
///
///   - names       : (list)   Command-line names (e.g. ["o","output"])
///   - description : (string) Option's purpose/effect
///   - default     : (varied) Default value, required for all non-switch options
///
/// Additional properties:
///
///   - type           : (string) Value type, one of {"integer", "string", "float",
///                               "boolean", "file", "dir"}. If none is provided,
///                               the option will be considered a boolean switch [false]
///   - hidden         : (bool)   If true, hide from help list [false]
///   - choices        : (list)   Valid input values
///   - choices.hidden : (bool)   If true, choice list will be hidden from help. [false]
///
/// Example:
///
///     // --------------------- //
///     //      definitions      //
///     // --------------------- //
///
///     const Option Algorithm {
///     R"({
///         "names"       : [ "a", "algorithm" ],
///         "description" : "Frobbing algorithm to do all the things.",
///         "type"        : "string",
///         "choices"     : [ "frob", "frob-experimental" ],
///         "default"     : "frob"
///     })"
///     };
///
///     const Option Threshold {
///     R"({
///         "names"       : [ "t", "threshold" ],
///         "description" : "Minimum threshold for filtering the things.",
///         "type"        : "float",
///         "default"     : 10.0
///     })"
///     };
///
///     const Option InputReferenceFile {
///     R"({
///         "names"       : [ "r", "reference" ],
///         "description" : "Input reference file (FASTA or XML).",
///         "type"        : "file",
///         "required"    : true
///     })"
///     };
///
///     // -------------------- //
///     //     help display     //
///     // -------------------- //
///
///     $ myApp --help
///     Usage: myApp [options] <subreads>
///
///       subreads                Input file description
///
///     Algorithm Options:
///       -a,--algorithm    Frobbing algorithm to do all the things.
///                         Valid choices: (frob, frob-experimental). [frob]
///       -t,--threshold    Minimum threshold for filtering the things. [10.0]
///
///     I/O Options:
///       -r,--reference    Input reference file (FASTA or XML).
///
///       -h,--help         Show this help.
///       --log-level       Set log level. [INFO]
///       --version         Show application version.
///
///     // ------------------------------ //
///     //      command line example      //
///     // ------------------------------ //
///
///     $ myApp -t 5.2 --algorithm frob-experimental -r ref.fasta subreads,bam
///
///     // ----------------------- //
///     //      fetch results      //
///     // ----------------------- //
///
///     settings.algorithm = results[Algorithm];
///     settings.threshold = results[Threshold];
///     settings.referenceFile = results[InputReferenceFile];
///
struct Option
{
    ///
    /// Creates option definition, with default included in the JSON text.
    ///
    explicit Option(std::string def);

    ///
    /// Construct option definitions, with a default explicitly provided.
    /// This allows actual typed values to be reused within client code.
    ///
    /// Option opt{ "...JSON...", MyDefaults::MinValue};
    ///
    Option(std::string def, bool on);
    Option(std::string def, int8_t val);
    Option(std::string def, int16_t val);
    Option(std::string def, int32_t val);
    Option(std::string def, int64_t val);
    Option(std::string def, uint8_t val);
    Option(std::string def, uint16_t val);
    Option(std::string def, uint32_t val);
    Option(std::string def, uint64_t val);
    Option(std::string def, float val);
    Option(std::string def, double val);
    Option(std::string def, std::string val);

    std::string text;
    boost::optional<OptionValue> defaultValue;

    // TODO: multi-value ("stringlist") support ???
};

}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_OPTION_H
