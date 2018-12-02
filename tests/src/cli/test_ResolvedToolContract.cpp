#include <pbcopper/cli/toolcontract/ResolvedToolContract.h>

#include <sstream>
#include <string>

#include <gtest/gtest.h>

TEST(CLI_ResolvedToolContract, read_basic_RTC)
{
    auto text = R"(
{
  "driver": {
    "env": {},
    "exe": "python -m pbcommand.cli.example.dev_app --resolved-tool-contract "
  },
  "resolved_tool_contract": {
    "input_files": [],
    "nproc": 4,
    "options": {
      "pbcommand.task_options.max_nlines": 27
    },
    "output_files": [],
    "resources": [],
    "is_distributed": false,
    "task_type": "pbsmrtpipe.task_types.standard",
    "tool_contract_id": "frobber.tools.dev_app",
    "log_level": "WARN"
  }
}
    )";

    std::stringstream input;
    input << text;

    // clang-format off

    PacBio::CLI::Interface interface {
        "frobber",
        "Frobb your files in a most delightful, nobbly way",
        "3.14"
    };
    interface.AddOptions({
        {"max_nlines", {"n"}, "Max Number of lines to Copy", PacBio::CLI::Option::IntType(10)}
    });

    const std::string id{"frobber.tasks.dev_txt_app"};
    PacBio::CLI::ToolContract::Task tcTask(id);
    tcTask.Options({
        { "max_nlines", "Max Lines" }
    });

    // clang-format off

    PacBio::CLI::ToolContract::Config tcConfig(tcTask);
    interface.EnableToolContract(tcConfig);

    PacBio::CLI::ToolContract::ResolvedToolContract rtc(interface);
    const PacBio::CLI::Results results = rtc.Parse(input);

    const int maxNLines = results["max_nlines"];
    EXPECT_EQ(27, maxNLines);
    EXPECT_EQ(PacBio::Logging::LogLevel::WARN, results.LogLevel());
    EXPECT_EQ(4, results.NumProcessors());
}

TEST(CLI_ResolvedToolContract, map_files_to_options)
{
    auto text = R"(
{
  "driver": {
    "env": {},
    "exe": "python -m pbcommand.cli.example.dev_app --resolved-tool-contract "
  },
  "resolved_tool_contract": {
    "input_files": [
      "/path/to/subreads_file.txt"
    ],
    "nproc": 2,
    "options": {
      "pbcommand.task_options.min_length": 25,
      "pbcommand.task_options.max_length": 500
    },
    "output_files": [
      "/path/to/output_file.txt",
      "/path/to/junk_file.txt",
      "/path/to/report_file.txt",
      "/path/to/json_file.txt"

     ],
    "resources": [],
    "is_distributed": false,
    "task_type": "pbsmrtpipe.task_types.standard",
    "tool_contract_id": "frobber.tools.dev_app",
    "log_level": "WARN"
  }
}
    )";

    std::stringstream input;
    input << text;

    // clang-format off
    PacBio::CLI::Interface interface {
        "frobber",
        "Frobb your files in a most delightful, nobbly way",
        "3.14"
    };
    interface.AddOptions({
        {"min_length",  {"minLength"},  "Minimum length.", PacBio::CLI::Option::IntType(10)},
        {"max_length",  {"maxLength"},  "Maximum length.", PacBio::CLI::Option::IntType(1000)},
        {"output_file", {"resultFile"}, "Output file.", PacBio::CLI::Option::StringType("")},
        {"junk_file",   {"junkFile"},   "Junk file.",   PacBio::CLI::Option::StringType("")},
        {"report_file", {"reportFile"}, "Report file.", PacBio::CLI::Option::StringType("")},
        {"json_file",   {"jsonFile"},   "JSON file.",   PacBio::CLI::Option::StringType("")},
    });

    const std::string id{"frobber.tasks.dev_txt_app"};
    PacBio::CLI::ToolContract::Task tcTask(id);
    tcTask.Options({
        { "min_length", "Minimum Length" },
        { "max_length", "Maximum Length" },
    });
    tcTask.InputFileTypes({
        { "input_file", "Input File", "Generic Txt file", "PacBio.FileTypes.txt" }
    });
    tcTask.OutputFileTypes({
        { "output_file", "Output File", "Output file.", "PacBio.FileTypes.txt", "output" },
        { "junk_file",   "Junk File",   "Junk file.",   "PacBio.FileTypes.txt", "junk" },
        { "report_file", "Report File", "Report file.", "PacBio.FileTypes.txt", "report" },
        { "json_file",   "JSON File",   "JSON file.",   "PacBio.FileTypes.txt", "json" }
    });
    tcTask.OutputFilesToOptions({
        { 0, "output_file" },
        { 1, "junk_file" },
        { 2, "report_file" },
        { 3, "json_file" },
    });
    // clang-format on

    PacBio::CLI::ToolContract::Config tcConfig(tcTask);
    interface.EnableToolContract(tcConfig);

    PacBio::CLI::ToolContract::ResolvedToolContract rtc(interface);
    const PacBio::CLI::Results results = rtc.Parse(input);

    const int minLength = results["min_length"];
    const int maxLength = results["max_length"];
    const std::string outputFile = results["output_file"];
    const std::string reportFile = results["report_file"];
    const std::string junkFile = results["junk_file"];
    const std::string jsonFile = results["json_file"];

    const auto positionalArgs = results.PositionalArguments();
    EXPECT_EQ(1, positionalArgs.size());
    const std::string inputFile = positionalArgs.at(0);

    EXPECT_EQ(25, minLength);
    EXPECT_EQ(500, maxLength);
    EXPECT_EQ("/path/to/output_file.txt", outputFile);
    EXPECT_EQ("/path/to/report_file.txt", reportFile);
    EXPECT_EQ("/path/to/junk_file.txt", junkFile);
    EXPECT_EQ("/path/to/json_file.txt", jsonFile);
    EXPECT_EQ("/path/to/subreads_file.txt", inputFile);

    EXPECT_EQ(2, results.NumProcessors());
}
