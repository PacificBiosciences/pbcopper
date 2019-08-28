// Author: Derek Barnett

#ifndef PBCOPPER_CLI_HELPPRINTER_H
#define PBCOPPER_CLI_HELPPRINTER_H

#include <pbcopper/PbcopperConfig.h>

#include <iosfwd>

namespace PacBio {
namespace CLI {

class Interface;

///
/// \brief The HelpPrinter class
///
class HelpPrinter
{
public:
    ///
    /// \brief Print
    /// \param interface
    /// \param out
    ///
    static void Print(const Interface& interface, std::ostream& out);
};

}  // namespace CLI
}  // namespace PacBio

#endif  // PBCOPPER_CLI_HELPPRINTER_H
