// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_RESULTSHANDLER_H
#define PBCOPPER_CLI_v2_RESULTSHANDLER_H

#include <functional>

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace CLI_v2 {

class Results;

///
/// A "ResultsHandler" is the callback associated with an interface. It will be
/// invoked after parsing values from the command line or resolved tool contract,
/// providing those results to the application.
///
/// The return value will be the application's exit value (e.g. EXIT_SUCCESS or
/// EXIT_FAILURE).
///
using ResultsHandler = std::function<int(const Results&)>;

}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_RESULTSHANDLER_H
