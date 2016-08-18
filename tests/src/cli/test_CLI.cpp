
#include "OStreamRedirector.h"

#include <pbcopper/cli/CLI.h>
#include <pbcopper/json/JSON.h>
#include <gtest/gtest.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdio>

using namespace PacBio;
using namespace PacBio::CLI;
using namespace PacBio::JSON;
using namespace std;

namespace PacBio {
namespace CLI {
namespace tests {

static string appName(void)
{
    static const string appName = "frobber";
    return appName;
}

static PacBio::CLI::Interface makeInterface(void)
{
    Interface i {
        "frobber",
        "Frobb your files in a most delightful, nobbly way",
        "3.14"
    };
    i.AddHelpOption();    // use built-in help output
    i.AddVersionOption(); // use built-in version output

    i.AddOptions({
        {"target_dir", {"t", "target-dir"}, "Copy all source files into <DIR>.", Option::StringType("my/default/dir")},
        {"force",      {"f", "force"},      "Overwrite things."},
        {"progress",   {"p"},               "Show progress during copy."},
        {"dry_run",    {"n", "no-op"},      "Dry run. Report actions that would be taken but do not perform them"},
        {"timeout",    {"timeout"},         "Abort execution after <INT> milliseconds.", Option::IntType(5000)}
    });

    i.AddPositionalArguments({
        {"source", "Source file to copy.",   "FILE" },
        {"dest",   "Destination directory.", "FILE" }
    });

    return i;
}

static PacBio::CLI::Interface makeToolContractEnabledInterface(void)
{
    using namespace ToolContract;

    // yields <PREFIX>.pbsmrtpipe_task_options.<ID>
    // if not set, PREFIX=ApplicationName from Interface
    //
    ToolContract::Task tcTask("frobber_task");       // if empty, pulss ApplicationName
    tcTask.Description("task-specific description"); // if empty, pulls ApplicationDescription
    tcTask.NumProcessors(4); // max_nproc if not provided
    tcTask.SetDistributed(true);
    tcTask.Type(TaskType::STANDARD);

    tcTask.Options({

        //
        // ID          display name
        //
        {"progress", "Show Progress"},
        {"force",    "Force Overwrite"},
        {"timeout",  "Timeout"}
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

    ToolContract::Config config{tcTask};
    config.Version("0.1");                // if empty, pulls ApplicationVersion

    auto interface = makeInterface();
    interface.EnableToolContract(config);
    return interface;
}

struct MyAppSettings
{
    bool force_;
    bool showProgress_;
    string targetDir_;
    int timeout_;
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
    EXPECT_EQ(42, settings.timeout_);
    EXPECT_EQ(string("requiredIn"),  settings.source_);
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
    s.force_        = args["force"];
    s.showProgress_ = args["progress"];
    s.targetDir_    = args["target_dir"];
    s.timeout_      = args["timeout"];

    const vector<string> positionalArgs = args.PositionalArguments();
    EXPECT_EQ(2, positionalArgs.size());
    s.source_ = positionalArgs.at(0);
    s.dest_   = positionalArgs.at(1);
    return mainAppEntry(s);
}

static int inputCommandLineChecker(const PacBio::CLI::Results& results)
{
    const string expectedCommandLine =
    {
        "frobber -f --timeout=42 --target-dir /path/to/dir/ requiredIn requiredOut"
    };
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

} // namespace tests
} // namespace CLI
} // namespace PacBio

TEST(CLI_Runner, emits_tool_contract_when_requested_from_command_line)
{
    // check cout output
    const auto expectedText = R"({
    "driver": {
        "env": {},
        "exe": "frobber --resolved-tool-contract",
        "serialization": "json"
    },
    "tool_contract": {
        "_comment": "Created by v0.0.1",
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
                "$schema": "http://json-schema.org/draft-04/schema#",
                "pb_option": {
                    "default": false,
                    "description": "Overwrite things.",
                    "name": "Force Overwrite",
                    "option_id": "frobber.task_options.force",
                    "type": "boolean"
                },
                "properties": {
                    "frobber.task_options.force": {
                        "default": false,
                        "description": "Overwrite things.",
                        "title": "Force Overwrite",
                        "type": "boolean"
                    }
                },
                "required": [
                    "frobber.task_options.force"
                ],
                "title": "JSON Schema for frobber.task_options.force",
                "type": "object"
            },
            {
                "$schema": "http://json-schema.org/draft-04/schema#",
                "pb_option": {
                    "default": false,
                    "description": "Show progress during copy.",
                    "name": "Show Progress",
                    "option_id": "frobber.task_options.progress",
                    "type": "boolean"
                },
                "properties": {
                    "frobber.task_options.progress": {
                        "default": false,
                        "description": "Show progress during copy.",
                        "title": "Show Progress",
                        "type": "boolean"
                    }
                },
                "required": [
                    "frobber.task_options.progress"
                ],
                "title": "JSON Schema for frobber.task_options.progress",
                "type": "object"
            },
            {
                "$schema": "http://json-schema.org/draft-04/schema#",
                "pb_option": {
                    "default": 5000,
                    "description": "Abort execution after <INT> milliseconds.",
                    "name": "Timeout",
                    "option_id": "frobber.task_options.timeout",
                    "type": "integer"
                },
                "properties": {
                    "frobber.task_options.timeout": {
                        "default": 5000,
                        "description": "Abort execution after <INT> milliseconds.",
                        "title": "Timeout",
                        "type": "integer"
                    }
                },
                "required": [
                    "frobber.task_options.timeout"
                ],
                "title": "JSON Schema for frobber.task_options.timeout",
                "type": "object"
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
    const vector<string> args =
    {
        PacBio::CLI::tests::appName(),
        "--emit-tool-contract"
    };
    PacBio::CLI::Run(args,
                     PacBio::CLI::tests::makeToolContractEnabledInterface(),
                     &PacBio::CLI::tests::appRunner);
    EXPECT_EQ(expectedText, s.str());
}

TEST(CLI_Runner, runs_application_from_vector_of_args)
{
    const vector<string> args =
    {
        PacBio::CLI::tests::appName(),
        "-f",
        "--timeout=42",
        "--target-dir", "/path/to/dir/",
        "requiredIn",
        "requiredOut"
    };
    PacBio::CLI::Run(args,
                     PacBio::CLI::tests::makeInterface(),
                     &PacBio::CLI::tests::appRunner);
}

TEST(CLI_Runner, runs_application_from_C_style_args)
{
    SCOPED_TRACE("run from normal args - raw C char*, a la application main()");

    static const int argc = 6;
    char* argv[argc+1];
    argv[0] = const_cast<char*>("frobber");
    argv[1] = const_cast<char*>("-f");
    argv[2] = const_cast<char*>("--timeout=42");
    argv[3] = const_cast<char*>("--target-dir=/path/to/dir/");
    argv[4] = const_cast<char*>("requiredIn");
    argv[5] = const_cast<char*>("requiredOut");
    argv[6] = nullptr;

    PacBio::CLI::Run(argc, argv,
                     PacBio::CLI::tests::makeInterface(),
                     &PacBio::CLI::tests::appRunner);
}

TEST(CLI_Runner, can_retrieve_input_commandline)
{
    SCOPED_TRACE("checking input command line");
    const vector<string> args =
    {
        PacBio::CLI::tests::appName(),
        "-f",
        "--timeout=42",
        "--target-dir", "/path/to/dir/",
        "requiredIn",
        "requiredOut"
    };
    PacBio::CLI::Run(args,
                     PacBio::CLI::tests::makeInterface(),
                     &PacBio::CLI::tests::inputCommandLineChecker);
}

TEST(CLI_Runner, runs_application_from_resolved_tool_contract)
{
    SCOPED_TRACE("run from RTC args");

    const string& rtcFn = "/tmp/pbcopper.cli.runner.resolved-tool-contract.json";
    PacBio::CLI::tests::RtcGenerator rtc(rtcFn);
    (void)rtc;

    const vector<string> args =
    {
        PacBio::CLI::tests::appName(),
        "--resolved-tool-contract",
        rtcFn
    };
    PacBio::CLI::Run(args,
                     PacBio::CLI::tests::makeToolContractEnabledInterface(),
                     &PacBio::CLI::tests::appRunner);
}

namespace PacBio {
namespace CLI {
namespace tests {

RtcGenerator::RtcGenerator(const std::string& fn)
    : fn_(fn)
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
            "frobber.task_options.force": true,
            "frobber.task_options.timeout": 42,
            "frobber.task_options.target_dir": "/path/to/dir/"
        },
        "output_files": [
            "requiredOut"
        ],
        "resources": [],
        "is_distributed": false,
        "task_type": "pbsmrtpipe.task_types.standard",
        "tool_contract_id": "frobber_task",
        "log_level": "INFO"
    }
})";

    std::ofstream out(fn);
    EXPECT_TRUE(static_cast<bool>(out));
    out << text;
}

RtcGenerator::~RtcGenerator(void)
{ ::remove(fn_.c_str()); }

} // namespace tests
} // namespace CLI
} // namespace PacBio

