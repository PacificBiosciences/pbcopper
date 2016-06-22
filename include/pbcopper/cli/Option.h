#ifndef PBCOPPER_CLI_OPTION_H
#define PBCOPPER_CLI_OPTION_H

#include "pbcopper/Config.h"
#include "pbcopper/json/JSON.h"
#include <initializer_list>
#include <string>
#include <memory>
#include <vector>

namespace PacBio {
namespace CLI {

namespace internal { class OptionPrivate; }

/// \brief The Option class defines a possible command-line option.
///
/// This class is used to describe an option on the command line. It allows
/// different ways of defining the same option with multiple aliases possible.
///  It is also used to describe how the option is used - it may be a flag
/// (e.g. -v) or take a value (e.g. -o file).
///
class Option
{
public:
    using BoolType   = JSON::Json::boolean_t;
    using IntType    = JSON::Json::number_integer_t;
    using UIntType   = JSON::Json::number_unsigned_t;
    using FloatType  = JSON::Json::number_float_t;
    using StringType = JSON::Json::string_t;

public:
    /// \name Constructors & Related Methods
    /// \{

    /// \brief Constructs a command line option object with the given arguments.
    ///
    /// The name of the option are set to \p name. Option names can be either
    /// short or long. If a name is one character in length, it is considered a
    /// short name. %Option names must not be empty, must not start with a dash
    /// or a slash character, must not contain a '=' and cannot be repeated.
    ///
    /// The description is set to \p description. In addition, the \p valueName
    /// can be set if the option expects a value. The default value for the
    /// option is set to \p defaultValue.
    ///
    /// If \p defaultValue is not provided, the option is considered to be a
    /// switch.
    ///
    /// This class can participate in C++11-style uniform initialization:
    ///
    /// \code
    /// PacBio::CLI::Interface cl;
    /// cl.addOption({"verbose_arg", "verbose", "Verbose mode."});
    /// \endcode
    ///
    /// \param[in]  name            option name
    /// \param[in]  description     option description
    /// \param[in]  valueName       value name (used in help display)
    /// \param[in]  defaultValue    default value for option if not specified
    ///
    Option(const std::string& id,
           const std::string& name,
           const std::string& description,
           const JSON::Json& defaultValue = JSON::Json());

    /// \brief Constructs a command line option object with the given arguments.
    ///
    /// This overload allows to set multiple names for the option, for instance
    /// "o" and "output".
    ///
    /// The names of the option are set to \p names. Option names can be either
    /// short or long. If a name is one character in length, it is considered a
    /// short name. %Option names must not be empty, must not start with a dash
    /// or a slash character, must not contain a '=' and cannot be repeated.
    ///
    /// The description is set to \p description. In addition, the \p valueName
    /// can be set if the option expects a value. The default value for the
    /// option is set to \p defaultValue.
    ///
    /// If \p defaultValue is not provided, the option is considered to be a
    /// switch.
    ///
    /// This class can participate in C++11-style uniform initialization:
    ///
    /// \code
    /// PacBio::CLI::Interface cl;
    /// cl.addOption({"output_file", {"o", "output"}, "Write results to <file>."});
    /// \endcode
    ///
    /// \param[in]  name            option name
    /// \param[in]  description     option description
    /// \param[in]  valueName       value name (used in help display)
    /// \param[in]  defaultValue    default value for option if not specified
    ///
    Option(const std::string& id,
           const std::vector<std::string>& names,
           const std::string& description,
           const JSON::Json& defaultValue = JSON::Json());

    /// \brief Constructs a command line option object with the given arguments.
    ///
    /// This overload allows to set multiple names for the option, for instance
    /// "o" and "output".
    ///
    /// The names of the option are set to \p names. Option names can be either
    /// short or long. If a name is one character in length, it is considered a
    /// short name. %Option names must not be empty, must not start with a dash
    /// or a slash character, must not contain a '=' and cannot be repeated.
    ///
    /// The description is set to \p description. In addition, the \p valueName
    /// can be set if the option expects a value. The default value for the
    /// option is set to \p defaultValue.
    ///
    /// If \p defaultValue is not provided, the option is considered to be a
    /// switch.
    ///
    /// This class can participate in C++11-style uniform initialization:
    ///
    /// \code
    /// PacBio::CLI::Interface cl;
    /// cl.addOption({"output_file", {"o", "output"}, "Write results to <file>."});
    /// \endcode
    ///
    /// \param[in]  name            option name
    /// \param[in]  description     option description
    /// \param[in]  valueName       value name (used in help display)
    /// \param[in]  defaultValue    default value for option if not specified
    ///
    Option(const std::string& id,
           const std::initializer_list<std::string>& names,
           const std::string& description,
           const JSON::Json& defaultValue = JSON::Json());

    Option(const Option& other);
    Option(Option&& other);
    Option& operator=(const Option& other);
    Option& operator=(Option&& other);
    ~Option(void);

    /// \}

public:
    /// \name Attributes
    /// \{

    ///
    /// \brief DefaultValue
    /// \return
    ///
    JSON::Json DefaultValue(void) const;

    ///
    /// \brief Description
    /// \return
    ///
    std::string Description(void) const;

    ///
    /// \brief Id
    /// \return
    ///
    std::string Id(void) const;

    ///
    /// \brief IsHidden
    /// \return
    ///
    bool IsHidden(void) const;

    ///
    /// \brief Names
    /// \return
    ///
    std::vector<std::string> Names(void) const;

    ///
    /// \brief ValueName
    /// \return
    ///
    std::string ValueName(void) const;

    /// \}

private:
    std::shared_ptr<internal::OptionPrivate> d_;
};

} // namespace CLI
} // namespace PacBio

#include "internal/Option-inl.h"

#endif // PBCOPPER_CLI_OPTION_H
