
#include <pbcopper/cli2/OptionValue.h>

#include <sstream>
#include <stdexcept>
#include <unordered_map>

#include <boost/algorithm/string.hpp>

namespace PacBio {
namespace CLI_v2 {
namespace {

class BoolConverter : public boost::static_visitor<bool>
{
public:
    bool operator()(bool x) const { return x; }

    // anything else always throws
    template <typename T>
    bool operator()(const T&) const
    {
        throw std::runtime_error{"OptionValue: cannot convert value to bool"};
        return 0;
    }
};

class DoubleConverter : public boost::static_visitor<double>
{
public:
    double operator()(double x) const { return x; }

    // anything else always throws
    template <typename T>
    double operator()(const T&) const
    {
        throw std::runtime_error{"OptionValue: cannot convert value to double"};
        return 0;
    }
};

class Int64Converter : public boost::static_visitor<int64_t>
{
public:
    int64_t operator()(int8_t x) const { return static_cast<int64_t>(x); }
    int64_t operator()(int16_t x) const { return static_cast<int64_t>(x); }
    int64_t operator()(int32_t x) const { return static_cast<int64_t>(x); }
    int64_t operator()(int64_t x) const { return x; }

    // anything else always throws
    template <typename T>
    int64_t operator()(const T&) const
    {
        throw std::runtime_error{"OptionValue: cannot convert value to integer"};
        return 0;
    }
};

class UInt64Converter : public boost::static_visitor<uint64_t>
{
public:
    uint64_t operator()(uint8_t x) const { return static_cast<uint64_t>(x); }
    uint64_t operator()(uint16_t x) const { return static_cast<uint64_t>(x); }
    uint64_t operator()(uint32_t x) const { return static_cast<uint64_t>(x); }
    uint64_t operator()(uint64_t x) const { return x; }

    // anything else always throws
    template <typename T>
    uint64_t operator()(const T&) const
    {
        throw std::runtime_error{"OptionValue: cannot convert value to unsigned integer"};
        return 0;
    }
};

class StringConverter : public boost::static_visitor<std::string>
{
public:
    std::string operator()(std::string x) const { return x; }

    // anything else always throws
    template <typename T>
    std::string operator()(const T&) const
    {
        throw std::runtime_error{"OptionValue: cannot convert value to string"};
        return 0;
    }
};

}  // namespace

OptionValueType ValueType(std::string typeString)
{
    static const std::unordered_map<std::string, OptionValueType> stringToType{
        {"int", OptionValueType::INT},           {"integer", OptionValueType::INT},
        {"uint", OptionValueType::UINT},         {"unsigned", OptionValueType::UINT},
        {"unsigned int", OptionValueType::UINT}, {"unsigned integer", OptionValueType::UINT},
        {"string", OptionValueType::STRING},     {"file", OptionValueType::FILE},
        {"dir", OptionValueType::DIR},           {"float", OptionValueType::FLOAT},
        {"double", OptionValueType::FLOAT},      {"bool", OptionValueType::BOOL},
        {"boolean", OptionValueType::BOOL}};

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

bool OptionValueToBool(const OptionValue& value)
{
    return boost::apply_visitor(BoolConverter(), value);
}

double OptionValueToDouble(const OptionValue& value)
{
    return boost::apply_visitor(DoubleConverter(), value);
}

int64_t OptionValueToInt(const OptionValue& value)
{
    return boost::apply_visitor(Int64Converter(), value);
}

std::string OptionValueToString(const OptionValue& value)
{
    return boost::apply_visitor(StringConverter(), value);
}

uint64_t OptionValueToUInt(const OptionValue& value)
{
    return boost::apply_visitor(UInt64Converter(), value);
}

}  // namespace CLI_v2
}  // namespace PacBio
