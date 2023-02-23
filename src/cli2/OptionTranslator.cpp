#include <pbcopper/cli2/internal/OptionTranslator.h>

#include <pbcopper/cli2/OptionValue.h>
#include <pbcopper/json/JSON.h>

#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

using Json = PacBio::JSON::Json;
using Option = PacBio::CLI_v2::Option;
using OptionData = PacBio::CLI_v2::internal::OptionData;
using OptionValue = PacBio::CLI_v2::OptionValue;
using OptionValueType = PacBio::CLI_v2::OptionValueType;

namespace {

struct MissingOptionFieldException : public std::runtime_error
{
    MissingOptionFieldException(const std::string& field, const std::string& optionText)
        : std::runtime_error("")
    {
        std::ostringstream msg;
        msg << "[pbcopper] command line option ERROR: required field: \"" << field
            << "\" is missing or empty in option definition:\n"
            << optionText << '\n';
        msg_ = msg.str();
    }

    const char* what() const noexcept override { return msg_.c_str(); }

    std::string msg_;
};

void Validate(const Option& option, const OptionData& result)
{
    // option data fields
    if (result.Names.empty()) {
        throw MissingOptionFieldException{"names", option.text};
    }
    if (result.Description.empty()) {
        throw MissingOptionFieldException{"description", option.text};
    }

    // ensure defaults match choices
    if (!result.Choices.empty() && result.DefaultValue) {

        const auto match =
            std::find(result.Choices.cbegin(), result.Choices.cend(), *result.DefaultValue);
        if (match == result.Choices.cend()) {
            std::ostringstream msg;
            msg << "[pbcopper] command line option ERROR: default value: \"";
            std::visit([&msg](const auto& v) { msg << v; }, *result.DefaultValue);
            msg << "\" is not among valid choices for option: " << result.Names.at(0);
            throw std::runtime_error{msg.str()};
        }
    }
}

}  // namespace

namespace PacBio {
namespace CLI_v2 {
namespace internal {

OptionValue MakeOptionValue(const Json& value, const OptionValueType type)
{
    switch (type) {
        case OptionValueType::INT: {
            const int64_t i = value;
            return OptionValue{i};
        }
        case OptionValueType::UINT: {
            const uint64_t u = value;
            return OptionValue{u};
        }
        case OptionValueType::FLOAT: {
            const double d = value;
            return OptionValue{d};
        }
        case OptionValueType::BOOL: {
            const bool b = value;
            return OptionValue{b};
        }
        case OptionValueType::STRING:
        case OptionValueType::FILE:
        case OptionValueType::DIR: {
            std::string s = value;
            return OptionValue{std::move(s)};
        }
        default: {
            assert(false);  // unreachable
            throw std::invalid_argument{
                "[pbcopper] command line option ERROR: unknown or unreachable option type"};
        }
    }
}

std::vector<std::string> OptionTranslator::OptionNames(const Option& option)
{
    std::vector<std::string> result;
    const Json root = Json::parse(option.text);
    for (const auto& name : root["names"]) {
        result.push_back(name);
    }
    return result;
}

OptionData OptionTranslator::Translate(const Option& option)
{

    OptionData result;

    try {
        const Json root = Json::parse(option.text);

        //
        // NOTE: Keep this first. Some other fields depend on it.
        //
        // type
        const auto type = root.find("type");
        if (type != root.cend()) {
            result.Type = ValueType(*type);
        }

        // names
        const auto names = root.find("names");
        if (names != root.cend()) {
            for (const auto& name : *names) {
                result.Names.push_back(name.get<std::string>());
            }
        }

        // names.hidden
        const auto hiddenNames = root.find("names.hidden");
        if (hiddenNames != root.cend()) {
            for (const auto& hiddenName : *hiddenNames) {
                result.HiddenNames.push_back(hiddenName.get<std::string>());
            }
        }

        // description - from single string, or joining array of strings
        const auto description = root.find("description");
        if (description != root.cend()) {
            if (description->is_array()) {
                for (const auto& line : *description) {
                    result.Description += line.get<std::string>();
                }
            } else {
                result.Description = description->get<std::string>();
            }
        }

        // required
        const auto required = root.find("required");
        if (required != root.cend()) {
            result.IsRequired = *required;
        }

        // hidden
        const auto hidden = root.find("hidden");
        if (hidden != root.cend()) {
            result.IsHidden = *hidden;
        }

        // choices
        const auto choices = root.find("choices");
        if (choices != root.cend()) {
            for (const auto& choice : *choices) {
                result.Choices.emplace_back(MakeOptionValue(choice, result.Type));
            }
        }

        // choices.hidden
        const auto hideChoices = root.find("choices.hidden");
        if (hideChoices != root.cend()) {
            result.IsChoicesHidden = *hideChoices;
        }

        // default
        //  - direct value supplied to Option ctor
        //  - supplied in Option definition text
        //  - ok if absent for bool switches, default is OFF
        //  - ok if absent for string options, default is ""
        //
        const auto defaultValue = root.find("default");
        if (option.defaultValue) {
            result.DefaultValue = option.defaultValue;
        } else if (defaultValue != root.cend()) {
            result.DefaultValue = MakeOptionValue(*defaultValue, result.Type);
        } else if (result.Type == OptionValueType::BOOL) {
            result.DefaultValue = false;
        } else if (IsStringLike(result.Type)) {
            result.DefaultValue = std::string{};
        }

        // default.hidden
        const auto hideDefault = root.find("default.hidden");
        if (hideDefault != root.cend()) {
            result.IsDefaultValueHidden = *hideDefault;
        }

    } catch (const std::exception& e) {
        std::ostringstream msg;
        msg << "[pbcopper] command line option ERROR: cannot parse option definition:\n"
            << option.text << '\n'
            << "because of JSON error: " << e.what() << '\n';
        throw std::runtime_error{msg.str()};
    }

    Validate(option, result);
    return result;
}

std::vector<OptionData> OptionTranslator::Translate(const std::vector<Option>& options)
{
    std::vector<OptionData> result;
    for (const auto& opt : options) {
        result.emplace_back(Translate(opt));
    }
    return result;
}

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio
