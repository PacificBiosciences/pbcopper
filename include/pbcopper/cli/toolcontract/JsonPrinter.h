#ifndef PBCOPPER_CLI_TOOLCONTRACT_JSONPRINTER_H
#define PBCOPPER_CLI_TOOLCONTRACT_JSONPRINTER_H

#include <iosfwd>

namespace PacBio {
namespace CLI {

class Interface;

namespace ToolContract {

class JsonPrinter
{
public:

    /// \brief Prints a JSON-formatted tool contract.
    ///
    /// \param interface    Interface object to generate tool contract from
    /// \param out          destination output stream
    /// \param indent       If indent is non-negative, then array elements and
    ///                     object members will be pretty-printed with that
    ///                     indent level. An indent level of 0 will only insert
    ///                     newlines. Indent level of -1 selects the most
    ///                     compact representation.
    ///
    /// \throws std::runtime_error if failed to print
    ///
    static void Print(const Interface& interface,
                      std::ostream& out,
                      const int indent = 4);
};

} // namespace ToolContract
} // namespace CLI
} // namespace PacBio

#endif // PBCOPPER_TOOLCONTRACT_JSONPRINTER_H
