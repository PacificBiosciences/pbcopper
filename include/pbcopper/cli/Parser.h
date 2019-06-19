// Author: Derek Barnett

#ifndef PBCOPPER_CLI_PARSER_H
#define PBCOPPER_CLI_PARSER_H

#include <pbcopper/PbcopperConfig.h>

#include <memory>
#include <string>
#include <vector>

#include <pbcopper/cli/Results.h>

namespace PacBio {
namespace CLI {

class Interface;

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

    Parser(Parser&&) noexcept;
    Parser& operator=(Parser&&) noexcept;
    ~Parser();

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
    class ParserPrivate;
    std::unique_ptr<ParserPrivate> d_;
};

}  // namespace CLI
}  // namespace PacBio

#endif  // PBCOPPER_CLI_PARSER_H
