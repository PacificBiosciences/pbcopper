// Author: Derek Barnett

#ifndef PBCOPPER_CLI_OPTION_H
#define PBCOPPER_CLI_OPTION_H

#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/cli/OptionFlags.h>
#include <pbcopper/json/JSON.h>

namespace PacBio {
namespace CLI {

namespace internal {
class OptionPrivate;
}

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
    using BoolType = JSON::Json::boolean_t;
    using IntType = JSON::Json::number_integer_t;
    using UIntType = JSON::Json::number_unsigned_t;
    using FloatType = JSON::Json::number_float_t;
    using StringType = JSON::Json::string_t;

public:
    /// \name Default Options
    /// \{

    ///
    /// \brief DefaultHelpOption
    /// \return
    ///
    static const Option& DefaultHelpOption();

    ///
    /// \brief DefaultLogLevelOption
    /// \return
    ///
    static const Option& DefaultLogLevelOption();

    ///
    /// \brief DefaultVerboseOption
    /// \return
    ///
    static const Option& DefaultVerboseOption();

    ///
    /// \brief DefaultVersionOption
    /// \return
    ///
    static const Option& DefaultVersionOption();

    /// \}

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
    /// The description is set to \p description. The default value for the
    /// option is set to \p defaultValue.
    ///
    /// If \p defaultValue is not provided, the option is considered to be a
    /// switch.
    ///
    /// If a \p choices array is provided, the input from the command line must
    /// be a member of the array.
    ///
    /// This class can participate in C++11-style uniform initialization:
    ///
    /// \code
    /// PacBio::CLI::Interface cl;
    /// cl.addOption({"verbose_arg", "verbose", "Verbose mode."});
    /// \endcode
    ///
    /// \param[in]  id              unique option id
    /// \param[in]  name            unique option name
    /// \param[in]  description     option description
    /// \param[in]  defaultValue    default value, if not user-specified
    /// \param[in]  choices         list of accepted inputs
    /// \param[in]  flags           extra configuration
    ///
    Option(std::string id, std::string name, std::string description,
           JSON::Json defaultValue = JSON::Json(nullptr), JSON::Json choices = JSON::Json(nullptr),
           OptionFlags flags = OptionFlags::DEFAULT);

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
    /// If a \p choices array is provided, the input from the command line must
    /// be a member of the array.
    ///
    /// This class can participate in C++11-style uniform initialization:
    ///
    /// \code
    /// PacBio::CLI::Interface cl;
    /// cl.addOption({"output_file", {"o", "output"}, "Write results to <file>."});
    /// \endcode
    ///
    /// \param[in]  id              unique option id
    /// \param[in]  names           unique option aliases
    /// \param[in]  description     option description
    /// \param[in]  defaultValue    default value for option if not specified
    /// \param[in]  choices         list of accepted inputs
    /// \param[in]  flags           extra configuration
    ///
    Option(std::string id, std::vector<std::string> names, std::string description,
           JSON::Json defaultValue = JSON::Json(nullptr), JSON::Json choices = JSON::Json(nullptr),
           OptionFlags flags = OptionFlags::DEFAULT);

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
    /// If a \p choices array is provided, the input from the command line must
    /// be a member of the array.
    ///
    /// This class can participate in C++11-style uniform initialization:
    ///
    /// \code
    /// PacBio::CLI::Interface cl;
    /// cl.addOption({"output_file", {"o", "output"}, "Write results to <file>."});
    /// \endcode
    ///
    /// \param[in]  id              unique option id
    /// \param[in]  names           unique option aliases
    /// \param[in]  description     option description
    /// \param[in]  defaultValue    default value for option if not specified
    /// \param[in]  choices         list of accepted inputs
    /// \param[in]  flags           extra configuration
    ///
    Option(std::string id, std::initializer_list<std::string> names, std::string description,
           JSON::Json defaultValue = JSON::Json(nullptr), JSON::Json choices = JSON::Json(nullptr),
           OptionFlags flags = OptionFlags::DEFAULT);

    Option(const Option& other);
    Option(Option&& other) noexcept;
    Option& operator=(const Option& other);
    Option& operator=(Option&& other) noexcept;
    ~Option();

    /// \}

public:
    /// \name Attributes
    /// \{

    ///
    /// \brief Choices
    /// \return
    ///
    JSON::Json Choices() const;

    ///
    /// \brief DefaultValue
    /// \return
    ///
    JSON::Json DefaultValue() const;

    ///
    /// \brief Description
    /// \return
    ///
    std::string Description() const;

    ///
    /// \brief HasChoices
    /// \return
    ///
    bool HasChoices() const;

    ///
    /// \brief Id
    /// \return
    ///
    std::string Id() const;

    ///
    /// \brief IsHidden
    /// \return
    ///
    bool IsHidden() const;

    ///
    /// \brief Names
    /// \return
    ///
    std::vector<std::string> Names() const;

    ///
    /// \brief TypeId
    /// \return
    ///
    std::string TypeId() const;

    ///
    /// \brief ValueName
    /// \return
    ///
    std::string ValueName() const;

    /// \}

private:
    std::shared_ptr<internal::OptionPrivate> d_;
};

}  // namespace CLI
}  // namespace PacBio

#include <pbcopper/cli/internal/Option-inl.h>

#endif  // PBCOPPER_CLI_OPTION_H
