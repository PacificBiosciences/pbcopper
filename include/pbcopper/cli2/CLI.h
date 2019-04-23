// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_CLI_H
#define PBCOPPER_CLI_v2_CLI_H

#include <iosfwd>
#include <string>
#include <vector>

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/cli2/Interface.h>
#include <pbcopper/cli2/MultiToolInterface.h>
#include <pbcopper/cli2/ResultsHandler.h>

namespace PacBio {
namespace CLI_v2 {

/// \brief
///
/// \param args
/// \param interface
/// \param handler
///
/// \return int
///
int Run(const std::vector<std::string>& args, const Interface& interface,
        const ResultsHandler& handler);

///
/// \brief
///
/// \param argc
/// \param argv
/// \param interface
/// \param handler
///
/// \return int
///
int Run(int argc, char* argv[], const Interface& interface, const ResultsHandler& handler);

///
/// \brief
///
/// \param args
/// \param interface
/// \return int
///
int Run(const std::vector<std::string>& args, const MultiToolInterface& interface);

///
/// \brief
///
/// \param argc
/// \param argv
/// \param interface
/// \return int
///
int Run(int argc, char* argv[], const MultiToolInterface& interface);

}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_CLI_H
