# How do use the MultiToolInterface

```cpp
#include <pbcopper/cli2/CLI.h>

#include <iostream>

using namespace PacBio;

CLI_v2::Interface MakeDatasetsplitInterface()
{
    CLI_v2::PositionalArgument const InputSubreadFile {
         R"({
          "name"        : "dataset",
          "description" : "PacBio XML of multiple BAMs, possibly with filters",
          "type"        : "file"
         })"
    };
    CLI_v2::Interface tui("myprog dataset-split", "Split a dataset into 1 per BAM, with original filters.", "1.2.3");
    tui.AddPositionalArguments({InputSubreadFile});
    // ...
    return tui;
}

int DatasetsplitRunner(const PacBio::CLI_v2::Results& args)
{
    std::cerr << "In DatasetsplitRunner()\n";
    std::cerr << " cl: '" << args.InputCommandLine() << "'\n";
    for (auto a : args.PositionalArguments()) {
        std::cerr << "  arg: '" << a << "'\n";
    }
    // Use operator[] to look-up both PositionalArguments and Options.
    return 0;
}

int main(int argc, char** argv)
{
    CLI_v2::MultiToolInterface ui{"pbcpp", "Utilities for PB (mostly assembly)", PbcppVersion()};
    ui.AddTools({
            {"dataset-split",   MakeDatasetsplitInterface(),   &DatasetsplitRunner},
            // ...
    });
    ui.HelpFooter(R"(...Footer...)");

    return CLI_v2::Run(argc, argv, ui);
}
```

Notes:

* When you call `AddTools()`, the tool-name should be the subcommand name.
* When you define the `Interface` for the subcommand, the first string should be
  "progname subcommandname", space-separated.

If you follow that pattern, then your help messages will look like:
```
% myprog --help

myprog - Utilities for PB (mostly assembly)

Usage:
  myprog <tool>

Tools:
  dataset-split Split a dataset into 1 per BAM, with original filters.

...Footer...


% myprog dataset-split --help

myprog dataset-split - Split a dataset into 1 per BAM, with original filters.

Usage:
  myprog dataset-split [options] <dataset>

  dataset           FILE  PacBio XML of multiple BAMs, possibly with filters

Options:
  -h,--help               Show this help and exit.
  --log-level       STR   Set log level. Valid choices: (TRACE, DEBUG, INFO,
                          WARN, FATAL). [WARN]
  --log-file        FILE  Log to a file, instead of stderr.
  -j,--num-threads  INT   Number of threads to use, 0 means autodetection. [0]
  --version               Show application version and exit.
```
