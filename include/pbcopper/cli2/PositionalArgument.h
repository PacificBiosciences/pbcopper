// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_POSITIONALARGUMENT_H
#define PBCOPPER_CLI_v2_POSITIONALARGUMENT_H

#include <pbcopper/PbcopperConfig.h>

#include <string>

namespace PacBio {
namespace CLI_v2 {

///
/// Defines properties for an interface positional argument, via JSON-formatted text.
///
/// Required properties are:
///
///   - name        : (string) display name for help, must be unique (used as lookup ID for results)
///   - description : (string) argument's purpose/effect
///
/// Additional properties:
///
///   - type : (string) one of {"integer", "string", "float", "boolean", "file", "dir"}
///
/// Example:
///
///     // --------------------- //
///     //      definitions      //
///     // --------------------- //
///
///     const PositionalArgument InputSubreadFile {
///     R"({
///          "name"        : "alignments",
///          "description" : "Input aligned subreads file (BAM or XML).",
///          "type"        : "file"
///     })";
///     };
///
///     const PositionalArgument InputReferenceFile {
///     R"({
///          "name"        : "reference",
///          "description" : "Input reference FASTA file.",
///          "type"        : "file"
///     })";
///     };
///
///     const PositionalArgument OutputCcsFile {
///     R"({
///          "name"        : "result",
///          "description" : "Aligned CCS results.",
///          "type"      : "file"
///     })";
///     };
///
///     interface.AddPositionalArguments({
///         InputSubreadFile,
///         InputReferenceFile,
///         OutputCcsFile
///     });
///
///     // -------------------- //
///     //     help display     //
///     // -------------------- //
///
///     $ myApp --help
///     Usage: myApp [options] <alignments> <reference> <result>
///
///       alignments  FILE  Input aligned subreads (BAM or XML).
///       reference   FILE  Input reference (FASTA or XML).
///       result      FILE  Aligned CCS results.
///
///     Options:
///         ...
///
///     // ------------------------------ //
///     //      command line example      //
///     // ------------------------------ //
///
///     $ myApp subreads.bam ref.fa out.fq
///
///     // ----------------------- //
///     //      fetch results      //
///     // ----------------------- //
///
///     settings.inBam  = results[InputSubreadFile];
///     settings.inRef  = results[InputReferenceFile];
///     settings.outCcs = results[OutputCcsFile];
///
struct PositionalArgument
{
    std::string text;
};

}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_POSITIONALARGUMENT_H
