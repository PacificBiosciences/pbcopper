
#include <pbcopper/cli2/OptionValue.h>

#include <sstream>
#include <stdexcept>
#include <unordered_map>

#include <boost/algorithm/string.hpp>

namespace PacBio {
namespace CLI_v2 {

OptionValueType ValueType(std::string typeString)
{
    static const std::unordered_map<std::string, OptionValueType> stringToType{
        {"int", OptionValueType::INT},           {"integer", OptionValueType::INT},
        {"uint", OptionValueType::UINT},         {"unsigned", OptionValueType::UINT},
        {"unsigned int", OptionValueType::UINT}, {"unsigned integer", OptionValueType::UINT},
        {"string", OptionValueType::STRING},     {"file", OptionValueType::FILE},
        {"dir", OptionValueType::DIR},           {"float", OptionValueType::FLOAT},
        {"bool", OptionValueType::BOOL},         {"boolean", OptionValueType::BOOL}};

    boost::to_lower(typeString);
    const auto found = stringToType.find(typeString);
    if (found != stringToType.cend())
        return found->second;
    else {
        std::ostringstream msg;
        msg << "[pbcopper] option ERROR: could not determine value type for \"" << typeString
            << "\"\n";
        throw std::runtime_error{msg.str()};
    }
}

}  // namespace CLI_v2
}  // namespace PacBio
