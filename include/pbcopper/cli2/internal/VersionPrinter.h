#ifndef PBCOPPER_CLI_v2_VERSIONPRINTER_H
#define PBCOPPER_CLI_v2_VERSIONPRINTER_H

#include <pbcopper/PbcopperConfig.h>

#include <iosfwd>

namespace PacBio {
namespace CLI_v2 {

class Interface;
class MultiToolInterface;

namespace internal {

struct VersionPrinter
{
    static void Print(const Interface& interface);
    static void Print(const Interface& interface, std::ostream& out);

    static void Print(const MultiToolInterface& interface);
    static void Print(const MultiToolInterface& interface, std::ostream& out);
};

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_VERSIONPRINTER_H
