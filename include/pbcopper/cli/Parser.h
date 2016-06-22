#ifndef PBCOPPER_CLI_PARSER_H
#define PBCOPPER_CLI_PARSER_H

#include "pbcopper/Config.h"
#include "pbcopper/cli/Results.h"
#include <memory>
#include <string>
#include <vector>

namespace PacBio {
namespace CLI {

class Interface;

namespace internal { class ParserPrivate; }

///
/// \brief The Parser class
///
class Parser
{
public:
    /// \name Constructors & Related Methods
    /// \{

    ///
    /// \brief Parser
    /// \param interface
    ///
    Parser(const Interface& interface);

    Parser(const Parser& other);
    ~Parser(void);

    /// \}

public:
    /// \name Argument Parsing
    /// \{

    ///
    /// \brief Parse
    /// \param argc
    /// \param argv
    /// \return
    ///
    Results Parse(int argc, char* argv[]);

    ///
    /// \brief Parse
    /// \param args
    /// \return
    ///
    Results Parse(const std::vector<std::string>& args);

    /// \}

private:
    std::unique_ptr<internal::ParserPrivate> d_;
};

} // namespace CLI
} // namespace PacBio

#endif // PBCOPPER_CLI_PARSER_H
