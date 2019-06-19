#include <pbcopper/cli2/Result.h>

#include <cassert>
#include <type_traits>

namespace PacBio {
namespace CLI_v2 {

static_assert(std::is_copy_constructible<Result>::value, "Result(const Result&) is not = default");
static_assert(std::is_copy_assignable<Result>::value,
              "Result& operator=(const Result&) is not = default");

static_assert(std::is_nothrow_move_constructible<Result>::value,
              "Result(Result&&) is not = noexcept");
static_assert(std::is_nothrow_move_assignable<Result>::value ==
                  std::is_nothrow_move_assignable<OptionValue>::value,
              "");

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
