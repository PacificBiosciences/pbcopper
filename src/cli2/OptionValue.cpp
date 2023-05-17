#include <pbcopper/cli2/OptionValue.h>

#include <boost/algorithm/string.hpp>

#include <ostream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace PacBio {
namespace CLI_v2 {
namespace {

class BoolConverter
{
public:
    bool operator()(bool x) const noexcept { return x; }

    // anything else always throws
    template <typename T>
    bool operator()(const T&) const
    {
        throw std::runtime_error{
            "[pbcopper] command line option ERROR: cannot convert value to bool"};
        return 0;
    }
};

class DoubleConverter
{
public:
    double operator()(double x) const noexcept { return x; }

    // anything else always throws
    template <typename T>
    double operator()(const T&) const
    {
        throw std::runtime_error{
            "[pbcopper] command line option ERROR: cannot convert value to double"};
        return 0;
    }
};

class Int64Converter
{
public:
    std::int64_t operator()(std::int8_t x) const noexcept { return x; }
    std::int64_t operator()(std::int16_t x) const noexcept { return x; }
    std::int64_t operator()(std::int32_t x) const noexcept { return x; }
    std::int64_t operator()(std::int64_t x) const noexcept { return x; }

    // anything else always throws
    template <typename T>
    std::int64_t operator()(const T&) const
    {
        throw std::runtime_error{
            "[pbcopper] command line option ERROR: cannot convert value to integer"};
        return 0;
    }
};

class UInt64Converter
{
public:
    std::uint64_t operator()(std::uint8_t x) const noexcept { return x; }
    std::uint64_t operator()(std::uint16_t x) const noexcept { return x; }
    std::uint64_t operator()(std::uint32_t x) const noexcept { return x; }
    std::uint64_t operator()(std::uint64_t x) const noexcept { return x; }

    // anything else always throws
    template <typename T>
    std::uint64_t operator()(const T&) const
    {
        throw std::runtime_error{
            "[pbcopper] command line option ERROR: cannot convert value to unsigned integer"};
        return 0;
    }
};

class StringConverter
{
public:
    std::string operator()(std::string x) const { return x; }

    // anything else always throws
    template <typename T>
    std::string operator()(const T&) const
    {
        throw std::runtime_error{
            "[pbcopper] command line option ERROR: cannot convert value to string"};
        return 0;
    }
};

// clang-format off
const std::unordered_map<std::string, OptionValueType> stringToType{
    {"int", OptionValueType::INT},           {"integer", OptionValueType::INT},
    {"uint", OptionValueType::UINT},         {"unsigned", OptionValueType::UINT},
    {"unsigned int", OptionValueType::UINT}, {"unsigned integer", OptionValueType::UINT},
    {"string", OptionValueType::STRING},     {"file", OptionValueType::FILE},
    {"dir", OptionValueType::DIR},           {"float", OptionValueType::FLOAT},
    {"double", OptionValueType::FLOAT},      {"bool", OptionValueType::BOOL},
    {"boolean", OptionValueType::BOOL}
};

const std::vector<std::string> typeToString{
    // index follows our std::variant
    "std::int8_t", "std::int16_t", "std::int32_t", "std::int64_t",
    "std::uint8_t", "std::uint16_t", "std::uint32_t", "std::uint64_t",
    "double", "bool", "string"
};
// clang-format on

}  // namespace

OptionValueType ValueType(std::string typeString)
{
    boost::to_lower(typeString);
    const auto found = stringToType.find(typeString);
    if (found != stringToType.cend()) {
        return found->second;
    } else {
        std::ostringstream msg;
        msg << "[pbcopper] command line option ERROR: could not determine value type for \""
            << typeString << "\"\n";
        throw std::runtime_error{msg.str()};
    }
}

bool OptionValueToBool(const OptionValue& value) { return std::visit(BoolConverter{}, value); }

double OptionValueToDouble(const OptionValue& value)
{
    return std::visit(DoubleConverter{}, value);
}

int64_t OptionValueToInt(const OptionValue& value) { return std::visit(Int64Converter{}, value); }

std::string OptionValueToString(const OptionValue& value)
{
    return std::visit(StringConverter{}, value);
}

uint64_t OptionValueToUInt(const OptionValue& value)
{
    return std::visit(UInt64Converter{}, value);
}

std::ostream& operator<<(std::ostream& out, const OptionValue& value)
{
    const auto typeIndex = value.index();
    switch (typeIndex) {
        // std::int8_t, std::int16_t, std::int32_t, int64_t
        case 0:
        case 1:
        case 2:
        case 3: {
            out << OptionValueToInt(value);
            break;
        }

        // std::uint8_t, std::uint16_t, std::uint32_t, uint64_t
        case 4:
        case 5:
        case 6:
        case 7: {
            out << OptionValueToUInt(value);
            break;
        }

        // double
        case 8: {
            out << OptionValueToDouble(value);
            break;
        }

        // bool
        case 9: {
            const bool ok = OptionValueToBool(value);
            out << (ok ? "true" : "false");
            break;
        }

        // string
        case 10: {
            const std::string s = OptionValueToString(value);
            out << s;
            break;
        }

        default: {
            std::ostringstream msg;
            msg << "[pbcopper] command line option ERROR: cannot print value for option of type: \""
                << typeToString.at(typeIndex) << "\"\n";
            throw std::runtime_error{msg.str()};
        }
    }
    return out;
}

}  // namespace CLI_v2
}  // namespace PacBio
