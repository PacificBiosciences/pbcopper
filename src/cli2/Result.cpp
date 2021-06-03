#include <pbcopper/cli2/Result.h>

#include <ostream>
#include <type_traits>

namespace PacBio {
namespace CLI_v2 {

Result::Result(OptionValue data, SetByMode setBy) : data_{std::move(data)}, setBy_{setBy} {}

bool Result::IsUserProvided() const { return setBy_ == SetByMode::USER; }

bool Result::IsDefault() const { return setBy_ == SetByMode::DEFAULT; }

SetByMode Result::SetBy() const { return setBy_; }

Result& Result::SetBy(SetByMode setBy)
{
    setBy_ = setBy;
    return *this;
}

Result::operator float() const
{
    const auto x = OptionValueToDouble(data_);
    return static_cast<float>(x);
}

Result::operator double() const { return OptionValueToDouble(data_); }

Result::operator bool() const { return OptionValueToBool(data_); }

Result::operator std::string() const { return OptionValueToString(data_); }

Result::operator int8_t() const
{
    const auto x = OptionValueToInt(data_);
    return static_cast<int8_t>(x);
}

Result::operator uint8_t() const
{
    const auto x = OptionValueToUInt(data_);
    return static_cast<uint8_t>(x);
}

Result::operator int16_t() const
{
    const auto x = OptionValueToInt(data_);
    return static_cast<int16_t>(x);
}

Result::operator uint16_t() const
{
    const auto x = OptionValueToUInt(data_);
    return static_cast<uint16_t>(x);
}

Result::operator int32_t() const
{
    const auto x = OptionValueToInt(data_);
    return static_cast<int32_t>(x);
}

Result::operator uint32_t() const
{
    const auto x = OptionValueToUInt(data_);
    return static_cast<uint32_t>(x);
}

Result::operator int64_t() const
{
    const auto x = OptionValueToInt(data_);
    return static_cast<int64_t>(x);
}

Result::operator uint64_t() const
{
    const auto x = OptionValueToUInt(data_);
    return static_cast<uint64_t>(x);
}

size_t Result::which() const { return data_.which(); }

const OptionValue& Result::RawValue() const { return data_; }

std::ostream& operator<<(std::ostream& out, const Result& result)
{
    out << result.RawValue();
    return out;
}

}  // namespace CLI_v2
}  // namespace PacBio
