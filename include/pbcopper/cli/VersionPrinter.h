#ifndef PBCOPPER_CLI_VERSIONPRINTER_H
#define PBCOPPER_CLI_VERSIONPRINTER_H

#include <iosfwd>

namespace PacBio {
namespace CLI {

class Interface;

class VersionPrinter
{
public:
    ///
    /// \brief Print
    /// \param interface
    /// \param out
    ///
    static void Print(const Interface& interface, std::ostream& out);
};

} // namespace CLI
} // namespace PacBio

#endif // PBCOPPER_CLI_VERSIONPRINTER_H
