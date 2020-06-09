#include <pbcopper/cli2/internal/PositionalArgumentTranslator.h>

#include <sstream>

#include <pbcopper/cli2/PositionalArgument.h>
#include <pbcopper/cli2/internal/PositionalArgumentData.h>
#include <pbcopper/json/JSON.h>

using Json = PacBio::JSON::Json;
using PositionalArgument = PacBio::CLI_v2::PositionalArgument;
using PositionalArgumentData = PacBio::CLI_v2::internal::PositionalArgumentData;

namespace {

struct MissingPosArgFieldException : public std::runtime_error
{
    MissingPosArgFieldException(const std::string& field, const std::string& posArgText)
        : std::runtime_error("")
    {
        std::ostringstream msg;
        msg << "[pbcopper] command line argument ERROR: required field: \"" << field
            << "\" is missing or empty in positional argument definition:\n"
            << posArgText << '\n';
        msg_ = msg.str();
    }

    const char* what() const noexcept override { return msg_.c_str(); }

    std::string msg_;
};

void Validate(const PositionalArgument& posArg, const PositionalArgumentData& result)
{
    // required argument data fields
    if (result.name.empty()) throw MissingPosArgFieldException("name", posArg.text);
    if (result.description.empty()) throw MissingPosArgFieldException("description", posArg.text);
}

}  // namespace

namespace PacBio {
namespace CLI_v2 {
namespace internal {

std::string PositionalArgumentTranslator::PositionalArgName(const PositionalArgument& posArg)
{
    const Json root = Json::parse(posArg.text);
    std::string result = root["name"];
    return result;
}

PositionalArgumentData PositionalArgumentTranslator::Translate(const PositionalArgument& posArg)
{
    PositionalArgumentData result;

    try {
        const Json root = Json::parse(posArg.text);

        // name
        auto name = root.find("name");
        if (name != root.cend()) result.name = name->get<std::string>();

        // description
        const auto description = root.find("description");
        if (description != root.cend()) result.description = description->get<std::string>();

        // type
        const auto type = root.find("type");
        if (type != root.cend()) result.type = ValueType(type->get<std::string>());

        // required
        const auto required = root.find("required");
        if (required != root.cend()) result.required = *required;

    } catch (std::exception& e) {
        std::ostringstream msg;
        msg << "[pbcopper] command line argument ERROR: cannot parse definition:\n"
            << posArg.text << '\n'
            << "with JSON error: " << e.what() << '\n';
        throw std::runtime_error{msg.str()};
    }

    Validate(posArg, result);
    return result;
}

std::vector<PositionalArgumentData> PositionalArgumentTranslator::Translate(
    const std::vector<PositionalArgument>& posArgs)
{
    std::vector<PositionalArgumentData> result;
    for (const auto& posArg : posArgs)
        result.emplace_back(Translate(posArg));
    return result;
}

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio
