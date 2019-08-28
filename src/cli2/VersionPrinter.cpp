#include <pbcopper/cli2/internal/VersionPrinter.h>

#include <iostream>

#include <pbcopper/cli2/Interface.h>
#include <pbcopper/cli2/MultiToolInterface.h>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

VersionPrinter::VersionPrinter(const Interface& i)
    : appName_{i.ApplicationName()}, appVersion_{i.ApplicationVersion()}
{
}

VersionPrinter::VersionPrinter(const MultiToolInterface& i)
    : appName_{i.ApplicationName()}, appVersion_{i.ApplicationVersion()}
{
}

void VersionPrinter::Print(std::ostream& out) const { out << appName_ << " " << appVersion_; }

std::ostream& operator<<(std::ostream& out, const VersionPrinter& version)
{
    version.Print(out);
    return out;
}

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio
