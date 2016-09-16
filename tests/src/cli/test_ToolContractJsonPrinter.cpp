#include <pbcopper/cli/toolcontract/JsonPrinter.h>
#include <pbcopper/cli/Interface.h>
#include <gtest/gtest.h>

using namespace PacBio;
using namespace PacBio::CLI;
using namespace PacBio::CLI::ToolContract;
using namespace std;

static string MakeToolContractText(const string nProc)
{
    // we're testing in the compact form of JSON output - there's already enough
    // quote-escaping white noise, without having newlines in the mix as well
    //

    const string header =
    {
        "{"
            "\"driver\":{"
                "\"env\":{},"
                "\"exe\":\"frobber --resolved-tool-contract\","
                "\"serialization\":\"json\""
            "},"
            "\"tool_contract\":{"
                "\"_comment\":\"Created by v0.0.1\","
                "\"description\":\"Frobb your files in a most delightful, nobbly way\","
                "\"input_types\":["
                    "{"
                        "\"description\":\"Generic Txt file\","
                        "\"file_type_id\":\"PacBio.FileTypes.txt\","
                        "\"id\":\"txt_in\","
                        "\"title\":\"Txt file\""
                    "}"
                "],"
                "\"is_distributed\":false,"
                "\"name\":\"frobber\","
                "\"nproc\":"
    };

    const string footer =
            {","
                "\"output_types\":["
                    "{"
                        "\"default_name\":\"output\","
                        "\"description\":\"Generic Output Txt file\","
                        "\"file_type_id\":\"PacBio.FileTypes.txt\","
                        "\"id\":\"txt_out\","
                        "\"title\":\"Txt outfile\""
                    "}"
                "],"
                "\"resource_types\":["
                    "\"$tmpfile\","
                    "\"$tmpfile\","
                    "\"$tmpdir\""
                "],"
                "\"schema_options\":["
                    "{"
                        "\"$schema\":\"http://json-schema.org/draft-04/schema#\","
                        "\"pb_option\":{"
                            "\"default\":10,"
                            "\"description\":\"Max Number of lines to Copy\","
                            "\"name\":\"Max lines\","
                            "\"option_id\":\"frobber.task_options.max_nlines\","
                            "\"type\":\"integer\""
                        "},"
                        "\"properties\":{"
                            "\"frobber.task_options.max_nlines\":{"
                                "\"default\":10,"
                                "\"description\":\"Max Number of lines to Copy\","
                                "\"title\":\"Max lines\","
                                "\"type\":\"integer\""
                            "}"
                        "},"
                        "\"required\":["
                            "\"frobber.task_options.max_nlines\""
                        "],"
                        "\"title\":\"JSON Schema for frobber.task_options.max_nlines\","
                        "\"type\":\"object\""
                    "}"
                "],"
                "\"task_type\":\"pbsmrtpipe.task_types.standard\","
                "\"tool_contract_id\":\"frobber.tasks.dev_txt_app\""
            "},"
            "\"tool_contract_id\":\"frobber.tasks.dev_txt_app\","
            "\"version\":\"3.14\""
        "}"
    };
    return header + nProc + footer;
}

TEST(CLI_ToolContractJsonPrinter, prints_expected_output)
{
    const string expectedText = MakeToolContractText("1");

    // --------------------------------------------
    // setup interface & tool contract config
    // --------------------------------------------

    Interface i {
        "frobber",
        "Frobb your files in a most delightful, nobbly way",
        "3.14"
    };
    i.AddOptions({
        {"max_nlines", "n", "Max Number of lines to Copy", Option::IntType(10)}
    });

    const string id = "frobber.tasks.dev_txt_app";
    ToolContract::Task tcTask(id);
    tcTask.InputFileTypes({
        { "txt_in", "Txt file", "Generic Txt file", "PacBio.FileTypes.txt" }
    });
    tcTask.OutputFileTypes({
        { "txt_out", "Txt outfile", "Generic Output Txt file", "PacBio.FileTypes.txt", "output" }
    });
    tcTask.ResourceTypes({
        ResourceType::TMP_FILE,
        ResourceType::TMP_FILE,
        ResourceType::TMP_DIR
    });
    tcTask.Options({
        { "max_nlines","Max lines" }
    });

    ToolContract::Config tcConfig(tcTask);
    i.EnableToolContract(tcConfig);

    // --------------------------------------------
    // test tool contract output
    // --------------------------------------------

    stringstream s;
    ToolContract::JsonPrinter::Print(i, s, -1);
    EXPECT_EQ(expectedText, s.str());
}

TEST(CLI_ToolContractJsonPrinter, prints_expected_output_with_explicit_nproc)
{
    const string expectedText = MakeToolContractText("4");

    // --------------------------------------------
    // setup interface & tool contract config
    // --------------------------------------------

    Interface i {
        "frobber",
        "Frobb your files in a most delightful, nobbly way",
        "3.14"
    };
    i.AddOptions({
        {"max_nlines", "n", "Max Number of lines to Copy", Option::IntType(10)}
    });

    const string id = "frobber.tasks.dev_txt_app";
    ToolContract::Task tcTask(id);
    tcTask.InputFileTypes({
        { "txt_in", "Txt file", "Generic Txt file", "PacBio.FileTypes.txt" }
    });
    tcTask.OutputFileTypes({
        { "txt_out", "Txt outfile", "Generic Output Txt file", "PacBio.FileTypes.txt", "output" }
    });
    tcTask.ResourceTypes({
        ResourceType::TMP_FILE,
        ResourceType::TMP_FILE,
        ResourceType::TMP_DIR
    });
    tcTask.Options({
        { "max_nlines","Max lines" }
    });
    tcTask.NumProcessors(4);

    ToolContract::Config tcConfig(tcTask);
    i.EnableToolContract(tcConfig);

    // --------------------------------------------
    // test tool contract output
    // --------------------------------------------

    stringstream s;
    ToolContract::JsonPrinter::Print(i, s, -1);
    EXPECT_EQ(expectedText, s.str());
}

TEST(CLI_ToolContractJsonPrinter, prints_expected_output_with_max_nproc)
{
    const string expectedText = MakeToolContractText("\"$max_nproc\"");

    // --------------------------------------------
    // setup interface & tool contract config
    // --------------------------------------------

    Interface i {
        "frobber",
        "Frobb your files in a most delightful, nobbly way",
        "3.14"
    };
    i.AddOptions({
        {"max_nlines", "n", "Max Number of lines to Copy", Option::IntType(10)}
    });

    const string id = "frobber.tasks.dev_txt_app";
    ToolContract::Task tcTask(id);
    tcTask.InputFileTypes({
        { "txt_in", "Txt file", "Generic Txt file", "PacBio.FileTypes.txt" }
    });
    tcTask.OutputFileTypes({
        { "txt_out", "Txt outfile", "Generic Output Txt file", "PacBio.FileTypes.txt", "output" }
    });
    tcTask.ResourceTypes({
        ResourceType::TMP_FILE,
        ResourceType::TMP_FILE,
        ResourceType::TMP_DIR
    });
    tcTask.Options({
        { "max_nlines","Max lines" }
    });
    tcTask.NumProcessors(Task::MAX_NPROC);

    ToolContract::Config tcConfig(tcTask);
    i.EnableToolContract(tcConfig);

    // --------------------------------------------
    // test tool contract output
    // --------------------------------------------

    stringstream s;
    ToolContract::JsonPrinter::Print(i, s, -1);
    EXPECT_EQ(expectedText, s.str());
}
