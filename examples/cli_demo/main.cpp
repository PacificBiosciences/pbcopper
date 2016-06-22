
#include <pbcopper/cli/CLI.h>
#include <iostream>
#include <string>
using namespace PacBio;
using namespace PacBio::CLI;
using namespace std;

struct MyAppSettings {
    bool progress;
    bool force;
    bool verbose;
    string tempDir;
    int timeout;
    string source;
    string dest;
    vector<string> extras;
};

static inline
int RunMain(const MyAppSettings& s)
{
    // this is where your normal app would take its settings and start rocking
    // here, we just print out what we received from the CL

    cout << "--------------------" << endl
         << "App Settings:       " << endl
         << "--------------------" << endl
         << "progress?  " << (s.progress ? "on" : "off") << endl
         << "force?     " << (s.force ? "on" : "off") << endl
         << "verbose?   " << (s.verbose ? "on" : "off") << endl
         << "tempDir:   " << s.tempDir << endl
         << "timeout:   " << s.timeout << endl
         << "source:    " << s.source << endl
         << "dest:      " << s.dest << endl
         << "extras:    ";
    for (const auto& extra : s.extras )
        cout << extra << ", ";
    cout << endl;

    return EXIT_SUCCESS;
}

static
PacBio::CLI::Interface createParser(void)
{
    PacBio::CLI::Interface cl {
        "cli_demo",
        "Play around with pbcopper's command line interface. Find bugs, report them.",
        "1.0"
    };
    cl.AddHelpOption();
    cl.AddVerboseOption();
    cl.AddVersionOption();
    cl.AddOptions({
        {"progress", {"p"},              "Show progress while doing the things."},
        {"force",    {"f", "force"},     "Overwrite the things ruthlessly." },
        {"temp_dir", {"t", "temp-dir"},  "Do the temp things in this directory.",       Option::StringType("my/default/tmp/dir")},
        {"timeout",  {"timeout"},        "Abort execution after so many milliseconds.", Option::IntType(5000)}
    });
    cl.AddPositionalArguments({
        {"source", "Source file for getting the things.", "<source>"},
        {"dest",   "Destination file for the generated things.", "<dest>"},
        {"extras", "Extra stuff to pass in here, optionally.", "[extras...]"}
    });
    return cl;
}

static
int argRunner(const PacBio::CLI::Results& args)
{
    // here, we translate the CLI Results to MyAppSettings
    //
    // this indirection allows the Results to come from either user-supplied
    // CL args OR from a resolved tool contract in the pbsmrtpipe context
    //

    MyAppSettings s;
    s.progress  = args["progress"];
    s.force     = args["force"];
    s.verbose   = args["verbose"];
    s.tempDir = args["temp_dir"];
    s.timeout   = args["timeout"];

    const auto positionalArgs = args.PositionalArguments();
    if (positionalArgs.size() < 2) {
        cerr << "ERROR: source & dest args are required" << endl;
        PacBio::CLI::PrintHelp(args.ApplicationInterface(), std::cout);
        return EXIT_FAILURE;
    }

    s.source = positionalArgs.at(0);
    s.dest   = positionalArgs.at(1);
    s.extras = { positionalArgs.begin()+2, positionalArgs.end() };
    return RunMain(s);
}

int main(int argc, char* argv[])
{
    return PacBio::CLI::Run(argc, argv,
                            createParser(),
                            &argRunner
                            );
}
