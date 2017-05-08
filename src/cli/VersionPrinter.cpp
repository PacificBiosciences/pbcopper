#include "pbcopper/cli/VersionPrinter.h"
#include "pbcopper/cli/Interface.h"
#include <ostream>

namespace PacBio {
namespace CLI {

void VersionPrinter::Print(const Interface& interface, std::ostream& out)
{
    out << interface.ApplicationName() << " "
        << interface.ApplicationVersion() << std::endl;
}

} // namespace CLI
} // namespace PacBio
