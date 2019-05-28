// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_VERSIONPRINTER_H
#define PBCOPPER_CLI_v2_VERSIONPRINTER_H

#include <pbcopper/PbcopperConfig.h>

#include <iosfwd>
#include <string>

namespace PacBio {
namespace CLI_v2 {

class Interface;
class MultiToolInterface;

namespace internal {

///
/// Generates version display.
///
class VersionPrinter
{
public:
    explicit VersionPrinter(const Interface& i);
    explicit VersionPrinter(const MultiToolInterface& i);

    ///
    /// Prints version text to output stream
    ///
    void Print(std::ostream& out) const;

private:
    std::string appName_;
    std::string appVersion_;
};

std::ostream& operator<<(std::ostream& out, const VersionPrinter& version);

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_VERSIONPRINTER_H
