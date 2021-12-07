#include <pbcopper/cli2/internal/VersionPrinter.h>

#include <pbcopper/cli2/Interface.h>
#include <pbcopper/cli2/MultiToolInterface.h>

#include <iostream>
#include <ostream>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

void VersionPrinter::Print(const Interface& interface) { Print(interface, std::cout); }

void VersionPrinter::Print(const Interface& interface, std::ostream& out)
{
    out << interface.ApplicationName() << ' ' << interface.ApplicationVersion() << '\n';
}

void VersionPrinter::Print(const MultiToolInterface& interface) { Print(interface, std::cout); }

void VersionPrinter::Print(const MultiToolInterface& interface, std::ostream& out)
{
    out << interface.ApplicationName() << ' ' << interface.ApplicationVersion() << '\n';
}

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio
