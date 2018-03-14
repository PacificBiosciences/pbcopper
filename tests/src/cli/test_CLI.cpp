
#include "OStreamRedirector.h"

#include <gtest/gtest.h>
#include <pbcopper/cli/CLI.h>
#include <pbcopper/json/JSON.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>

using namespace PacBio;
using namespace PacBio::CLI;
using namespace PacBio::JSON;
using namespace std;

namespace CLITests {

static string appName(void)
{
    static const string appName = "frobber";
    return appName;
}

static PacBio::CLI::Interface makeInterface(void)
{
    // clang-format off
    Interface i {
        "frobber",
        "Frobb your files in a most delightful, nobbly way",
        "3.14"
    };
    i.AddHelpOption();     // use built-in help option
    i.AddLogLevelOption(); // use built-in logLevel option
    i.AddVersionOption();  // use built-in version option

    i.AddOptions({
        {"target_dir", {"t", "target-dir"}, "Copy all source files into <DIR>.", Option::StringType("my/default/dir")},
        {"force",      {"f", "force"},      "Overwrite things."},
        {"progress",   {"p"},               "Show progress during copy."},
        {"dry_run",    {"n", "no-op"},      "Dry run. Report actions that would be taken but do not perform them"},
        {"timeout",    {"timeout"},         "Abort execution after <INT> milliseconds.", Option::IntType(5000)},
        {"delta",      "delta",             "Some delta for things", Option::IntType(1), { 1, 2, 3 }},
        {"ploidy",     "ploidy",            "Genome ploidy", Option::StringType("haploid"), {"haploid", "diploid"}},
        {"element",    {"e", "element"},    "Choice of element indicates mood. Science.", Option::StringType("fire"), {"earth", "wind", "fire", "water"}},
        {"tc_only",    {"tc-only"},         "Hidden from cmdline help, but still available from TC.", Option::BoolType(false), {}, OptionFlags::HIDE_FROM_HELP}
    });

    i.AddPositionalArguments({
        {"source", "Source file to copy.",   "FILE" },
        {"dest",   "Destination directory.", "FILE" }
    });
    // clang-format on

    return i;
}

static PacBio::CLI::Interface makeToolContractEnabledInterface(void)
{
    using namespace ToolContract;

    // yields <PREFIX>.pbsmrtpipe_task_options.<ID>
    // if not set, PREFIX=ApplicationName from Interface
    //
    ToolContract::Task tcTask("frobber_task");        // if empty, pulss ApplicationName
    tcTask.Description("task-specific description");  // if empty, pulls ApplicationDescription
    tcTask.NumProcessors(4);                          // max_nproc if not provided
    tcTask.SetDistributed(true);
    tcTask.Type(TaskType::STANDARD);

    // clang-format off
    tcTask.Options({

        //
        // ID          display name
        //
        {"progress", "Show Progress"},
        {"force",    "Force Overwrite"},
        {"timeout",  "Timeout"},
        {"delta",    "Frobbing delta" },
        {"ploidy",   "Ploidy" },
        {"element", "Element"},
        {"tc_only",  "Tool contract-only option"}
    });
    tcTask.InputFileTypes({
        { "ID", "title", "description", "type" },
        { "ID", "title", "description", "type" }
    });
    tcTask.OutputFileTypes({
        { "ID", "title", "description", "type", "defaultName" },
        { "ID", "title", "description", "type", "defaultName" }
    });
    tcTask.ResourceTypes({
        ResourceType::TMP_DIR,
        ResourceType::TMP_FILE,
        ResourceType::TMP_FILE
    });
    // clang-format on

    ToolContract::Config config{tcTask};
    config.Version("0.1");  // if empty, pulls ApplicationVersion

    auto interface = makeInterface();
    interface.EnableToolContract(config);
    return interface;
}

struct MyAppSettings
{
    bool force_;
    bool showProgress_;
    string targetDir_;
    string ploidy_;
    string element_;
    int timeout_;
    int delta_;
    string source_;
    string dest_;
    vector<string> extras_;
};

static int mainAppEntry(const MyAppSettings& settings)
{
    // doing testing checks here, but this is where application would get
    // rocking with its (probably already-existing) 'settings' object
    // filled out with desired parameters
    //
    EXPECT_TRUE(settings.force_);
    EXPECT_FALSE(settings.showProgress_);
    EXPECT_EQ(string("/path/to/dir/"), settings.targetDir_);
    EXPECT_EQ(string("diploid"), settings.ploidy_);
    EXPECT_EQ(string("water"), settings.element_);
    EXPECT_EQ(42, settings.timeout_);
    EXPECT_EQ(2, settings.delta_);
    EXPECT_EQ(string("requiredIn"), settings.source_);
    EXPECT_EQ(string("requiredOut"), settings.dest_);
    return EXIT_SUCCESS;
}

static int appRunner(const PacBio::CLI::Results& args)
{
    // pull CLI results (whether user-supplied OR resolved tool contract) &
    // convert to the 'settings' object that most applications use for this
    // kind of thing
    //
    MyAppSettings s;
    s.force_ = args["force"];
    s.showProgress_ = args["progress"];
    s.targetDir_ = args["target_dir"];
    s.ploidy_ = args["ploidy"];
    s.element_ = args["element"];
    s.timeout_ = args["timeout"];
    s.delta_ = args["delta"];

    EXPECT_EQ(PacBio::Logging::LogLevel::DEBUG, args.LogLevel());

    const vector<string> positionalArgs = args.PositionalArguments();
    EXPECT_EQ(2, positionalArgs.size());
    s.source_ = positionalArgs.at(0);
    s.dest_ = positionalArgs.at(1);
    return mainAppEntry(s);
}

static int inputCommandLineChecker(const PacBio::CLI::Results& results)
{
    const string expectedCommandLine = {
        "frobber -f --timeout=42 --delta=2 --ploidy=diploid --element=water --target-dir "
        "/path/to/dir/ --logLevel=DEBUG requiredIn requiredOut"};
    EXPECT_EQ(expectedCommandLine, results.InputCommandLine());
    return EXIT_SUCCESS;
}

struct RtcGenerator
{
public:
    RtcGenerator(const std::string& fn);
    ~RtcGenerator(void);

private:
    std::string fn_;
};

}  // namespace CLITests

TEST(CLI_Runner, emits_tool_contract_when_requested_from_command_line)
{
    // check cout output
    const auto expectedText = R"({
    "driver": {
        "env": {},
        "exe": "frobber --resolved-tool-contract",
        "serialization": "json"
    },
    "schema_version": "2.0.0",
    "tool_contract": {
        "_comment": "Created by v0.2.0",
        "description": "Frobb your files in a most delightful, nobbly way",
        "input_types": [
            {
                "description": "description",
                "file_type_id": "type",
                "id": "ID",
                "title": "title"
            },
            {
                "description": "description",
                "file_type_id": "type",
                "id": "ID",
                "title": "title"
            }
        ],
        "is_distributed": true,
        "name": "frobber",
        "nproc": 4,
        "output_types": [
            {
                "default_name": "defaultName",
                "description": "description",
                "file_type_id": "type",
                "id": "ID",
                "title": "title"
            },
            {
                "default_name": "defaultName",
                "description": "description",
                "file_type_id": "type",
                "id": "ID",
                "title": "title"
            }
        ],
        "resource_types": [
            "$tmpdir",
            "$tmpfile",
            "$tmpfile"
        ],
        "schema_options": [
            {
                "choices": [
                    1,
                    2,
                    3
                ],
                "default": 1,
                "description": "Some delta for things",
                "id": "frobber.task_options.delta",
                "name": "Frobbing delta",
                "optionTypeId": "choice_integer"
            },
            {
                "choices": [
                    "earth",
                    "wind",
                    "fire",
                    "water"
                ],
                "default": "fire",
                "description": "Choice of element indicates mood. Science.",
                "id": "frobber.task_options.element",
                "name": "Element",
                "optionTypeId": "choice_string"
            },
            {
                "default": false,
                "description": "Overwrite things.",
                "id": "frobber.task_options.force",
                "name": "Force Overwrite",
                "optionTypeId": "boolean"
            },
            {
                "choices": [
                    "haploid",
                    "diploid"
                ],
                "default": "haploid",
                "description": "Genome ploidy",
                "id": "frobber.task_options.ploidy",
                "name": "Ploidy",
                "optionTypeId": "choice_string"
            },
            {
                "default": false,
                "description": "Show progress during copy.",
                "id": "frobber.task_options.progress",
                "name": "Show Progress",
                "optionTypeId": "boolean"
            },
            {
                "default": false,
                "description": "Hidden from cmdline help, but still available from TC.",
                "id": "frobber.task_options.tc_only",
                "name": "Tool contract-only option",
                "optionTypeId": "boolean"
            },
            {
                "default": 5000,
                "description": "Abort execution after <INT> milliseconds.",
                "id": "frobber.task_options.timeout",
                "name": "Timeout",
                "optionTypeId": "integer"
            }
        ],
        "task_type": "pbsmrtpipe.task_types.standard",
        "tool_contract_id": "frobber_task"
    },
    "tool_contract_id": "frobber_task",
    "version": "3.14"
})";

    // redirect cout to our catcher
    std::stringstream s;
    ::tests::CoutRedirect redirect(s.rdbuf());
    (void)redirect;

    // set up tool contract emitted from main CLI entry
    const vector<string> args = {CLITests::appName(), "--emit-tool-contract"};
    PacBio::CLI::Run(args, CLITests::makeToolContractEnabledInterface(), &CLITests::appRunner);
    EXPECT_EQ(expectedText, s.str());
}

TEST(CLI_Runner, runs_application_from_vector_of_args)
{
    const vector<string> args = {CLITests::appName(),
                                 "-f",
                                 "--timeout=42",
                                 "--delta=2",
                                 "--ploidy=diploid",
                                 "-e",
                                 "water",
                                 "--target-dir",
                                 "/path/to/dir/",
                                 "--logLevel=DEBUG",
                                 "requiredIn",
                                 "requiredOut"};
    PacBio::CLI::Run(args, CLITests::makeInterface(), &CLITests::appRunner);
}

TEST(CLI_Runner, runs_application_from_C_style_args)
{
    SCOPED_TRACE("run from normal args - raw C char*, a la application main()");

    static const int argc = 10;
    char* argv[argc + 1];
    argv[0] = const_cast<char*>("frobber");
    argv[1] = const_cast<char*>("-f");
    argv[2] = const_cast<char*>("--timeout=42");
    argv[3] = const_cast<char*>("--delta=2");
    argv[4] = const_cast<char*>("--ploidy=diploid");
    argv[5] = const_cast<char*>("--element=water");
    argv[6] = const_cast<char*>("--target-dir=/path/to/dir/");
    argv[7] = const_cast<char*>("--logLevel=DEBUG");
    argv[8] = const_cast<char*>("requiredIn");
    argv[9] = const_cast<char*>("requiredOut");
    argv[10] = nullptr;

    PacBio::CLI::Run(argc, argv, CLITests::makeInterface(), &CLITests::appRunner);
}

TEST(CLI_Runner, can_retrieve_input_commandline)
{
    SCOPED_TRACE("checking input command line");
    const vector<string> args = {CLITests::appName(), "-f",
                                 "--timeout=42",      "--delta=2",
                                 "--ploidy=diploid",  "--element=water",
                                 "--target-dir",      "/path/to/dir/",
                                 "--logLevel=DEBUG",  "requiredIn",
                                 "requiredOut"};
    PacBio::CLI::Run(args, CLITests::makeInterface(), &CLITests::inputCommandLineChecker);
}

TEST(CLI_Runner, runs_application_from_resolved_tool_contract)
{
    SCOPED_TRACE("run from RTC args");

    const string& rtcFn = "/tmp/pbcopper.cli.runner.resolved-tool-contract.json";
    CLITests::RtcGenerator rtc(rtcFn);
    (void)rtc;

    const vector<string> args = {CLITests::appName(), "--resolved-tool-contract", rtcFn};
    PacBio::CLI::Run(args, CLITests::makeToolContractEnabledInterface(), &CLITests::appRunner);
}

TEST(CLI_Runner, throws_on_invalid_choices)
{
    {
        SCOPED_TRACE("use invalid int choice on command line");
        const vector<string> args = {CLITests::appName(),
                                     "--delta=4",  // <-- invalid input (must be one of [1,2,3])
                                     "requiredIn", "requiredOut"};

        EXPECT_THROW(PacBio::CLI::Run(args, CLITests::makeInterface(), &CLITests::appRunner),
                     std::runtime_error);
    }
    {
        SCOPED_TRACE("use invalid string choice on command line");
        const vector<string> args = {
            CLITests::appName(),
            "--ploidy=triploid",  // <-- invalid input (must be one of["haploid","diploid"])
            "requiredIn", "requiredOut"};

        EXPECT_THROW(PacBio::CLI::Run(args, CLITests::makeInterface(), &CLITests::appRunner),
                     std::runtime_error);
    }
}

namespace CLITests {

RtcGenerator::RtcGenerator(const std::string& fn) : fn_(fn)
{
    auto text = R"(
{
    "driver": {
        "env": {},
        "exe": "frobber --resolved-tool-contract",
        "serialization": "json"
    },
    "resolved_tool_contract": {
        "input_files": [
            "requiredIn"
        ],
        "nproc": 1,
        "options": {
            "frobber.task_options.delta": 2,
            "frobber.task_options.force": true,
            "frobber.task_options.ploidy": "diploid",
            "frobber.task_options.element": "water",
            "frobber.task_options.timeout": 42,
            "frobber.task_options.target_dir": "/path/to/dir/",
            "frobber.task_options.tc_only": false
        },
        "output_files": [
            "requiredOut"
        ],
        "resources": [],
        "is_distributed": false,
        "task_type": "pbsmrtpipe.task_types.standard",
        "tool_contract_id": "frobber_task",
        "log_level": "DEBUG"
    }
})";

    std::ofstream out(fn);
    EXPECT_TRUE(static_cast<bool>(out));
    out << text;
}

RtcGenerator::~RtcGenerator(void) { ::remove(fn_.c_str()); }

}  // namespace CLITests
