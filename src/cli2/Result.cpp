#include <pbcopper/cli2/Result.h>

namespace PacBio {
namespace CLI_v2 {

Result::Result() : data_(), setBy_{SetByMode::DEFAULT} {}

Result::Result(OptionValue data, SetByMode setBy) : data_{std::move(data)}, setBy_{setBy} {}

bool Result::IsUserProvided() const { return setBy_ == SetByMode::USER; }

bool Result::IsDefault() const { return setBy_ == SetByMode::DEFAULT; }

SetByMode Result::SetBy() const { return setBy_; }

Result& Result::SetBy(SetByMode setBy)
{
    setBy_ = setBy;
    return *this;
}

Result::operator int() const { return boost::get<int>(data_); }

Result::operator unsigned int() const { return boost::get<unsigned int>(data_); }

Result::operator float() const { return boost::get<float>(data_); }

Result::operator bool() const { return boost::get<bool>(data_); }

Result::operator std::string() const { return boost::get<std::string>(data_); }

size_t Result::which() const { return data_.which(); }

}  // namespace CLI_v2
}  // namespace PacBio