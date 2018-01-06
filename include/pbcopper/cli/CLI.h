#ifndef PBCOPPER_CLI_CLI_H
#define PBCOPPER_CLI_CLI_H

#include <functional>
#include <iosfwd>
#include <string>
#include <vector>

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/cli/Interface.h>
#include <pbcopper/cli/Results.h>

/// So applications's main.cpp contains something like:
///
/// #include "BlasrSettings.h" // for settings struct
/// #include "BlasrEntry.h"    // app entry point that takes the settings & starts working
///
/// static PacBio::CLI::Interface makeBlasrInterface(void)
/// {
///     // sets up interface object with options
/// }
///
/// static int blasrRunner(const PacBio::CLI::Results& args)
/// {
///     // extract options from args -> settings
///     //
///     // if interface was configured w/ tool contract support, args may be from
///     // cmd-line OR resolved tool contract, but the application shouldn't actually
///     // care or be concerned with that
///     //
///
///     BlasrSettings s;
///     s.foo = args["foo"];
///     // ...
///
///     // start the application's real work & return success/fail, a la main()
///     return blasrEntry(s);
/// }
///
/// int main(int argc, char* argv[])
/// {
///     return PacBio::CLI::Run(argc, argv,
///                             makeBlasrInterface(),
///                             &blasrRunner);
/// }
///

namespace PacBio {
namespace CLI {

///
/// \brief ResultsHandler
///
typedef std::function<int(const PacBio::CLI::Results&)> ResultsHandler;

///
/// \brief PrintHelp
/// \param interface
/// \param out
///
void PrintHelp(const Interface& interface, std::ostream& out);

///
/// \brief PrintVersion
/// \param interface
/// \param out
///
void PrintVersion(const Interface& interface, std::ostream& out);

// TODO: add logging input here

///
/// \brief Run
/// \param argc
/// \param argv
/// \param interface
/// \param handler
/// \return
///
int Run(int argc, char* argv[],
        const Interface& interface,
        const ResultsHandler& handler);

///
/// \brief Run
/// \param args
/// \param interface
/// \param handler
/// \return
///
int Run(const std::vector<std::string>& args,
        const Interface& interface,
        const ResultsHandler& handler);

} // namespace CLI
} // namespace PacBio

#endif // PBCOPPER_CLI_CLI_H
