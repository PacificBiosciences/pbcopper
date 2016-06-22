#include <pbcopper/cli/toolcontract/ResolvedToolContract.h>
#include <gtest/gtest.h>

#include <sstream>
#include <string>

using namespace PacBio;
using namespace PacBio::CLI;
using namespace PacBio::CLI::ToolContract;
using namespace std;

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
    "nproc": 1,
    "options": {
      "pbcommand.task_options.max_nlines": 27
    },
    "output_files": [],
    "resources": [],
    "is_distributed": false,
    "task_type": "pbsmrtpipe.task_types.standard",
    "tool_contract_id": "frobber.tools.dev_app",
    "log_level": "INFO"
  }
}
    )";

    stringstream input;
    input << text;

    Interface interface {
        "frobber",
        "Frobb your files in a most delightful, nobbly way",
        "3.14"
    };
    interface.AddOptions({
        {"max_nlines", {"n"}, "Max Number of lines to Copy", Option::IntType(10)}
    });

    const string id = "frobber.tasks.dev_txt_app";
    ToolContract::Task tcTask(id);
    tcTask.Options({
        { "max_nlines", "Max Lines" }
    });

    ToolContract::Config tcConfig(tcTask);
    interface.EnableToolContract(tcConfig);


    ToolContract::ResolvedToolContract rtc(interface);
    const Results results = rtc.Parse(input);

    const int maxNLines = results["max_nlines"];
    EXPECT_EQ(27, maxNLines);


}
