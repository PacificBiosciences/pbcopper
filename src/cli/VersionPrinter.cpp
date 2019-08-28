// Author: Derek Barnett

#include <pbcopper/cli/VersionPrinter.h>

#include <ostream>

#include <pbcopper/cli/Interface.h>

namespace PacBio {
namespace CLI {

void VersionPrinter::Print(const Interface& interface, std::ostream& out)
{
    out << interface.ApplicationName() << " " << interface.ApplicationVersion() << '\n';
}

}  // namespace CLI
}  // namespace PacBio
