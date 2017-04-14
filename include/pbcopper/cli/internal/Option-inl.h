#ifndef PBCOPPER_CLI_OPTION_INL_H
#define PBCOPPER_CLI_OPTION_INL_H

#include "pbcopper/cli/Option.h"

namespace PacBio {
namespace CLI {
namespace internal {

class OptionPrivate
{
public:
    PacBio::JSON::Json option_;

public:
    explicit OptionPrivate(const std::string& id,
                           const std::vector<std::string>& names,
                           const std::string& description,
                           const PacBio::JSON::Json& defaultValue,
                           const PacBio::JSON::Json& choices)
        : option_(PacBio::JSON::Json::object_t())
    {
        // validate ID & name(s)
        if (id.empty())
            throw std::runtime_error("CLI::Option: options must have at non-empty ID");
        if (names.empty())
            throw std::runtime_error("CLI::Option: options must have at least one name");
        for (const std::string& name : names) {
            if (name.empty())
                throw std::runtime_error("CLI::Option: options cannot be empty");
            if (name.at(0) == '-')
                throw std::runtime_error("CLI::Option: options cannot start with '-'");
            if (name.at(0) == '/')
                throw std::runtime_error("CLI::Option: options cannot start with '/'");
            if (name.find('=') != std::string::npos)
                throw std::runtime_error("CLI::Option: options cannot contain an '='");
        }

        // store data
        option_["id"]           = id;
        option_["names"]        = names;
        option_["description"]  = description;
        option_["hidden"]       = false;

        // if none provided, treat as a switch-type option (init w/ false)
        if (defaultValue.is_null())
            option_["defaultValue"] = PacBio::JSON::Json::boolean_t(false);
        else
            option_["defaultValue"] = defaultValue;

        // choices
        if (choices.is_array() && !choices.empty())
            option_["choices"] = choices;
        else
            option_["choices"] = JSON::Json(nullptr);
    }

    OptionPrivate(const OptionPrivate& other)
        : option_(other.option_)
    { }
};

} // namespace internal

// ------------------------
// PacBio::CLI::Option
// ------------------------

inline Option::Option(const std::string& id,
               const std::string& name,
               const std::string& description,
               const PacBio::JSON::Json& defaultValue,
               const JSON::Json& choices)
    : d_(new internal::OptionPrivate{
            id,
            std::vector<std::string>{1, name},
            description,
            defaultValue,
            choices
         })
{ }

inline Option::Option(const std::string& id,
               const std::initializer_list<std::string>& init,
               const std::string& description,
               const PacBio::JSON::Json& defaultValue,
               const PacBio::JSON::Json& choices)
    : Option(id,
             std::vector<std::string>{init},
             description,
             defaultValue,
             choices)
{ }

inline Option::Option(const std::string& id,
                      const std::vector<std::string>& names,
                      const std::string& description,
                      const PacBio::JSON::Json& defaultValue,
                      const PacBio::JSON::Json& choices)
    : d_(new internal::OptionPrivate{
            id,
            names,
            description,
            defaultValue,
            choices
         })
{ }

inline Option::Option(const Option& other)
    : d_(new internal::OptionPrivate(*other.d_.get()))
{ }

inline Option::Option(Option&& other)
    : d_(std::move(other.d_))
{ }

inline Option& Option::operator=(const Option& other)
{
    d_->option_ = other.d_->option_;
    return *this;
}

inline Option& Option::operator=(Option&& other)
{
    std::swap(d_, other.d_);
    return *this;
}

inline Option::~Option(void) { }

inline PacBio::JSON::Json Option::Choices(void) const
{ return d_->option_["choices"]; }

inline PacBio::JSON::Json Option::DefaultValue(void) const
{ return d_->option_["defaultValue"]; }

inline std::string Option::Description(void) const
{ return d_->option_["description"]; }

inline bool Option::HasChoices(void) const
{
    const auto& choices = Choices();
    return choices.is_array() && !choices.empty();
}

inline std::string Option::Id(void) const
{ return d_->option_["id"]; }

inline bool Option::IsHidden(void) const
{ return d_->option_["hidden"]; }

inline std::vector<std::string> Option::Names(void) const
{ return d_->option_["names"]; }

inline std::string Option::ValueName(void) const
{ return std::string(); }

} // namespace CLI
} // namespace PacBio

#endif // PBCOPPER_CLI_OPTION_INL_H
