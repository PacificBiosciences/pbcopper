
#include <pbcopper/cli/CLI.h>
#include <iostream>
#include <string>
using namespace PacBio;
using namespace PacBio::CLI;

struct MyAppSettings {
    bool progress;
    bool force;
    bool verbose;
    std::string tempDir;
    int timeout;
    std::string source;
    std::string dest;
    std::vector<std::string> extras;
};

static inline
int RunMain(const MyAppSettings& s)
{
    // this is where your normal app would take its settings and start rocking
    // here, we just print out what we received from the CL

    std::cout << "--------------------\n"
         << "App Settings:       \n"
         << "--------------------\n"
         << "progress?  " << (s.progress ? "on" : "off") << '\n'
         << "force?     " << (s.force ? "on" : "off") << '\n'
         << "verbose?   " << (s.verbose ? "on" : "off") << '\n'
         << "tempDir:   " << s.tempDir << '\n'
         << "timeout:   " << s.timeout << '\n'
         << "source:    " << s.source << '\n'
         << "dest:      " << s.dest << '\n'
         << "extras:    ";
    for (const auto& extra : s.extras )
        std::cout << extra << ", ";
    std::cout << '\n';

    return EXIT_SUCCESS;
}

static
PacBio::CLI::Interface createParser()
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
        {"progress",   {"p"},              "Show progress while doing the things."},
        {"force",      {"f", "force"},     "Overwrite the things ruthlessly." },
        {"temp_dir",   {"t", "temp-dir"},  "Do the temp things in this directory.",       Option::StringType("my/default/tmp/dir")},
        {"timeout",    {"timeout"},        "Abort execution after so many milliseconds.", Option::IntType(5000)},
        {"modelPath",  {"M", "modelPath"}, "Path to a model file.",                       Option::StringType("")}
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
        std::cerr << "ERROR: source & dest args are required\n";
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
