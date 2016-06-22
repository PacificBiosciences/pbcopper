#include <pbcopper/cli/toolcontract/JsonPrinter.h>
#include <pbcopper/cli/Interface.h>
#include <gtest/gtest.h>

using namespace PacBio;
using namespace PacBio::CLI;
using namespace PacBio::CLI::ToolContract;
using namespace std;

TEST(CLI_ToolContractJsonPrinter, prints_expected_output)
{
    // we're testing in the compact form of JSON output - there's already enough
    // quote-escaping white noise, without having newlines in the mix as well
    //

//    auto expectedText = R"(
//{
//    "driver":{"
//        "env":{},



//}
//    )";

    const string expectedText =
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
                "\"nproc\":1,"
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

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

/*
 *
    "{"
        version": "0.1",
        "driver": {
            "serialization": "json",
            "exe": "task_pbccs_ccs --resolved-tool-contract ",
            "env": {}
        },
        "tool_contract_id": "pbccs.tasks.ccs",
        "tool_contract": {
            "task_type": "pbsmrtpipe.task_types.standard",
            "resource_types": [],
            "description": "\nWrapper for 'ccs' executable to provide tool contract interface support (and\nincidentally, using DataSet XML as input).  Note that this will *not* currently\nsupport chunking, since it access the individual .bam files en masse.\n",
            "schema_options": [
                {
                    "pb_option": {
                        "default": 3.75,
                        "type": "number",
                        "option_id": "pbccs.task_options.min_snr",
                        "name": "Minimum SNR",
                        "description": "Minimum SNR of input subreads"
                    },
                    "title": "JSON Schema for pbccs.task_options.min_snr",
                    "required": [
                        "pbccs.task_options.min_snr"
                    ],
                    "$schema": "http://json-schema.org/draft-04/schema#",
                    "type": "object",
                    "properties": {
                        "pbccs.task_options.min_snr": {
                            "default": 3.75,
                            "type": "number",
                            "description": "Minimum SNR of input subreads",
                            "title": "Minimum SNR"
                        }
                    }
                },
                {
                    "pb_option": {
                        "default": 0.65,
                        "type": "number",
                        "option_id": "pbccs.task_options.min_read_score",
                        "name": "Minimum Read Score",
                        "description": "Minimum read score of input subreads"
                    },
                    "title": "JSON Schema for pbccs.task_options.min_read_score",
                    "required": [
                        "pbccs.task_options.min_read_score"
                    ],
                    "$schema": "http://json-schema.org/draft-04/schema#",
                    "type": "object",
                    "properties": {
                        "pbccs.task_options.min_read_score": {
                            "default": 0.65,
                            "type": "number",
                            "description": "Minimum read score of input subreads",
                            "title": "Minimum Read Score"
                        }
                    }
                },
                {
                    "pb_option": {
                        "default": 7000,
                        "type": "integer",
                        "option_id": "pbccs.task_options.max_length",
                        "name": "Maximum Subread Length",
                        "description": "Maximum length of subreads to use for generating CCS"
                    },
                    "title": "JSON Schema for pbccs.task_options.max_length",
                    "required": [
                        "pbccs.task_options.max_length"
                    ],
                    "$schema": "http://json-schema.org/draft-04/schema#",
                    "type": "object",
                    "properties": {
                        "pbccs.task_options.max_length": {
                            "default": 7000,
                            "type": "integer",
                            "description": "Maximum length of subreads to use for generating CCS",
                            "title": "Maximum Subread Length"
                        }
                    }
                },
                {
                    "pb_option": {
                        "default": 10,
                        "type": "integer",
                        "option_id": "pbccs.task_options.min_length",
                        "name": "Minimum Subread Length",
                        "description": "Minimum length of subreads to use for generating CCS"
                    },
                    "title": "JSON Schema for pbccs.task_options.min_length",
                    "required": [
                        "pbccs.task_options.min_length"
                    ],
                    "$schema": "http://json-schema.org/draft-04/schema#",
                    "type": "object",
                    "properties": {
                        "pbccs.task_options.min_length": {
                            "default": 10,
                            "type": "integer",
                            "description": "Minimum length of subreads to use for generating CCS",
                            "title": "Minimum Subread Length"
                        }
                    }
                },
                {
                    "pb_option": {
                        "default": 3,
                        "type": "integer",
                        "option_id": "pbccs.task_options.min_passes",
                        "name": "Minimum Number of Passes",
                        "description": "Minimum number of subreads required to generate CCS"
                    },
                    "title": "JSON Schema for pbccs.task_options.min_passes",
                    "required": [
                        "pbccs.task_options.min_passes"
                    ],
                    "$schema": "http://json-schema.org/draft-04/schema#",
                    "type": "object",
                    "properties": {
                        "pbccs.task_options.min_passes": {
                            "default": 3,
                            "type": "integer",
                            "description": "Minimum number of subreads required to generate CCS",
                            "title": "Minimum Number of Passes"
                        }
                    }
                },
                {
                    "pb_option": {
                        "default": 0.9,
                        "type": "number",
                        "option_id": "pbccs.task_options.min_predicted_accuracy",
                        "name": "Minimum Predicted Accuracy",
                        "description": "Minimum predicted accuracy in [0, 1]"
                    },
                    "title": "JSON Schema for pbccs.task_options.min_predicted_accuracy",
                    "required": [
                        "pbccs.task_options.min_predicted_accuracy"
                    ],
                    "$schema": "http://json-schema.org/draft-04/schema#",
                    "type": "object",
                    "properties": {
                        "pbccs.task_options.min_predicted_accuracy": {
                            "default": 0.9,
                            "type": "number",
                            "description": "Minimum predicted accuracy in [0, 1]",
                            "title": "Minimum Predicted Accuracy"
                        }
                    }
                },
                {
                    "pb_option": {
                        "default": -5.0,
                        "type": "number",
                        "option_id": "pbccs.task_options.min_zscore",
                        "name": "Minimum Z Score",
                        "description": "Minimum Z score to use a subread"
                    },
                    "title": "JSON Schema for pbccs.task_options.min_zscore",
                    "required": [
                        "pbccs.task_options.min_zscore"
                    ],
                    "$schema": "http://json-schema.org/draft-04/schema#",
                    "type": "object",
                    "properties": {
                        "pbccs.task_options.min_zscore": {
                            "default": -5.0,
                            "type": "number",
                            "description": "Minimum Z score to use a subread",
                            "title": "Minimum Z Score"
                        }
                    }
                },
                {
                    "pb_option": {
                        "default": 0.34,
                        "type": "number",
                        "option_id": "pbccs.task_options.max_drop_fraction",
                        "name": "Maximum Dropped Fraction",
                        "description": "Maximum fraction of subreads that can be dropped before giving up"
                    },
                    "title": "JSON Schema for pbccs.task_options.max_drop_fraction",
                    "required": [
                        "pbccs.task_options.max_drop_fraction"
                    ],
                    "$schema": "http://json-schema.org/draft-04/schema#",
                    "type": "object",
                    "properties": {
                        "pbccs.task_options.max_drop_fraction": {
                            "default": 0.34,
                            "type": "number",
                            "description": "Maximum fraction of subreads that can be dropped before giving up",
                            "title": "Maximum Dropped Fraction"
                        }
                    }
                },
                {
                    "pb_option": {
                        "default": false,
                        "type": "boolean",
                        "option_id": "pbccs.task_options.no_polish",
                        "name": "No Polish CCS",
                        "description": "Only output the initial template derived from the POA"
                    },
                    "title": "JSON Schema for pbccs.task_options.no_polish",
                    "required": [
                        "pbccs.task_options.no_polish"
                    ],
                    "$schema": "http://json-schema.org/draft-04/schema#",
                    "type": "object",
                    "properties": {
                        "pbccs.task_options.no_polish": {
                            "default": false,
                            "type": "boolean",
                            "description": "Only output the initial template derived from the POA",
                            "title": "No Polish CCS"
                        }
                    }
                },
                {
                    "pb_option": {
                        "default": false,
                        "type": "boolean",
                        "option_id": "pbccs.task_options.by_strand",
                        "name": "By Strand CCS",
                        "description": "For each ZMW, generate two CCS sequences, one for each strand"
                    },
                    "title": "JSON Schema for pbccs.task_options.by_strand",
                    "required": [
                        "pbccs.task_options.by_strand"
                    ],
                    "$schema": "http://json-schema.org/draft-04/schema#",
                    "type": "object",
                    "properties": {
                        "pbccs.task_options.by_strand": {
                            "default": false,
                            "type": "boolean",
                            "description": "For each ZMW, generate two CCS sequences, one for each strand",
                            "title": "By Strand CCS"
                        }
                    }
                }
            ],
            "output_types": [
                {
                    "title": "ConsensusReadSet",
                    "description": "Output DataSet XML file",
                    "default_name": "ccs",
                    "id": "bam_output",
                    "file_type_id": "PacBio.DataSet.ConsensusReadSet"
                },
                {
                    "title": "CSV report",
                    "description": "Text report summarizing run statistics",
                    "default_name": "ccs_report",
                    "id": "report_csv",
                    "file_type_id": "PacBio.FileTypes.txt"
                }
            ],
            "_comment": "Created by v0.3.25",
            "name": "ccs",
            "input_types": [
                {
                    "description": "Subread DataSet or .bam file",
                    "title": "SubreadSet",
                    "id": "subread_set",
                    "file_type_id": "PacBio.DataSet.SubreadSet"
                }
            ],
            "nproc": "$max_nproc",
            "is_distributed": true,
            "tool_contract_id": "pbccs.tasks.ccs"
        }
    "}"
*
*
*/

