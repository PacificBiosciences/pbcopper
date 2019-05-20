// Author: Derek Barnett

#ifndef PBCOPPER_CLI_OPTION_INL_H
#define PBCOPPER_CLI_OPTION_INL_H

#include <pbcopper/cli/Option.h>

namespace PacBio {
namespace CLI {
namespace internal {

class OptionPrivate
{
public:
    PacBio::JSON::Json option_;

public:
    explicit OptionPrivate(std::string id, std::vector<std::string> names, std::string description,
                           PacBio::JSON::Json defaultValue, PacBio::JSON::Json choices,
                           OptionFlags flags)
        : option_(PacBio::JSON::Json::object_t())
    // NOTE: ^^ not "uniform initialization." Charing parens to curly brackets gives this error:
    // "libc++abi.dylib: terminating with uncaught exception of type std::domain_error: cannot use operator[] with array"
    {
        // validate ID & name(s)
        if (id.empty()) throw std::runtime_error("CLI::Option: options must have at non-empty ID");
        if (names.empty())
            throw std::runtime_error("CLI::Option: options must have at least one name");
        for (const std::string& name : names) {
            if (name.empty()) throw std::runtime_error("CLI::Option: options cannot be empty");
            if (name.at(0) == '-')
                throw std::runtime_error("CLI::Option: options cannot start with '-'");
            if (name.at(0) == '/')
                throw std::runtime_error("CLI::Option: options cannot start with '/'");
            if (name.find('=') != std::string::npos)
                throw std::runtime_error("CLI::Option: options cannot contain an '='");
        }

        // store data
        option_["id"] = std::move(id);
        option_["names"] = std::move(names);
        option_["description"] = std::move(description);

        // if none provided, treat as a switch-type option (init w/ false)
        if (defaultValue.is_null())
            option_["defaultValue"] = PacBio::JSON::Json::boolean_t{false};
        else
            option_["defaultValue"] = std::move(defaultValue);

        // choices
        if (choices.is_array() && !choices.empty())
            option_["choices"] = std::move(choices);
        else
            option_["choices"] = JSON::Json(nullptr);

        // flags
        const bool isHidden = (flags & OptionFlags::HIDE_FROM_HELP) != 0;
        option_["hidden"] = isHidden;
    }

    OptionPrivate(const OptionPrivate& other) : option_(other.option_) {}
};

}  // namespace internal

// ------------------------
// PacBio::CLI::Option
// ------------------------

inline Option::Option(std::string id, std::string name, std::string description,
                      PacBio::JSON::Json defaultValue, JSON::Json choices, OptionFlags flags)
    : d_{std::make_unique<internal::OptionPrivate>(std::move(id), std::vector<std::string>{1, name},
                                                   std::move(description), std::move(defaultValue),
                                                   std::move(choices), flags)}
{
}

inline Option::Option(std::string id, std::initializer_list<std::string> init,
                      std::string description, PacBio::JSON::Json defaultValue,
                      PacBio::JSON::Json choices, OptionFlags flags)
    : Option{std::move(id),          std::vector<std::string>{init},
             std::move(description), std::move(defaultValue),
             std::move(choices),     flags}
{
}

inline Option::Option(std::string id, std::vector<std::string> names, std::string description,
                      PacBio::JSON::Json defaultValue, PacBio::JSON::Json choices,
                      OptionFlags flags)
    : d_{std::make_unique<internal::OptionPrivate>(std::move(id), std::move(names),
                                                   std::move(description), std::move(defaultValue),
                                                   std::move(choices), flags)}
{
}

inline Option::Option(const Option& other) : d_(new internal::OptionPrivate(*other.d_.get())) {}

inline Option::Option(Option&& other) noexcept = default;

inline Option& Option::operator=(const Option& other)
{
    d_->option_ = other.d_->option_;
    return *this;
}

inline Option& Option::operator=(Option&& other) noexcept = default;

inline Option::~Option() = default;

inline PacBio::JSON::Json Option::Choices() const { return d_->option_["choices"]; }

inline PacBio::JSON::Json Option::DefaultValue() const { return d_->option_["defaultValue"]; }

inline std::string Option::Description() const { return d_->option_["description"]; }

inline bool Option::HasChoices() const
{
    const auto choices = Choices();
    return choices.is_array() && !choices.empty();
}

inline std::string Option::Id() const { return d_->option_["id"]; }

inline bool Option::IsHidden() const { return d_->option_["hidden"]; }

inline std::vector<std::string> Option::Names() const { return d_->option_["names"]; }

inline std::string Option::ValueName() const { return std::string(); }

}  // namespace CLI
}  // namespace PacBio

#endif  // PBCOPPER_CLI_OPTION_INL_H
