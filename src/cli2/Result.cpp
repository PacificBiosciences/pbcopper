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

Result::operator std::int8_t() const
{
    const auto x = OptionValueToInt(data_);
    return static_cast<std::int8_t>(x);
}

Result::operator std::uint8_t() const
{
    const auto x = OptionValueToUInt(data_);
    return static_cast<std::uint8_t>(x);
}

Result::operator std::int16_t() const
{
    const auto x = OptionValueToInt(data_);
    return static_cast<std::int16_t>(x);
}

Result::operator std::uint16_t() const
{
    const auto x = OptionValueToUInt(data_);
    return static_cast<std::uint16_t>(x);
}

Result::operator std::int32_t() const
{
    const auto x = OptionValueToInt(data_);
    return static_cast<std::int32_t>(x);
}

Result::operator std::uint32_t() const
{
    const auto x = OptionValueToUInt(data_);
    return static_cast<std::uint32_t>(x);
}

Result::operator std::int64_t() const
{
    const auto x = OptionValueToInt(data_);
    return static_cast<std::int64_t>(x);
}

Result::operator std::uint64_t() const
{
    const auto x = OptionValueToUInt(data_);
    return static_cast<std::uint64_t>(x);
}

std::size_t Result::which() const { return data_.index(); }

const OptionValue& Result::RawValue() const { return data_; }

std::ostream& operator<<(std::ostream& out, const Result& result)
{
    out << result.RawValue();
    return out;
}

}  // namespace CLI_v2
}  // namespace PacBio
