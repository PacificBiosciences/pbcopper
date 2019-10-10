#include <pbcopper/cli2/Option.h>

#include <cassert>
#include <type_traits>

namespace PacBio {
namespace CLI_v2 {

static_assert(std::is_copy_constructible<Option>::value, "Option(const Option&) is not = default");
static_assert(std::is_copy_assignable<Option>::value,
              "Option& operator=(const Option&) is not = default");

#ifndef __INTEL_COMPILER
static_assert(std::is_nothrow_move_constructible<Option>::value,
              "Option(Option&&) is not = noexcept");
static_assert(std::is_nothrow_move_assignable<Option>::value ==
                  std::is_nothrow_move_assignable<std::string>::value,
              "");
#endif

Option::Option(std::string def) : text{std::move(def)}, defaultValue{boost::none} {}

Option::Option(std::string def, bool on) : text{std::move(def)}, defaultValue{on} {}

Option::Option(std::string def, int8_t val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(std::string def, int16_t val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(std::string def, int32_t val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(std::string def, int64_t val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(std::string def, uint8_t val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(std::string def, uint16_t val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(std::string def, uint32_t val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(std::string def, uint64_t val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(std::string def, float val)
    : text{std::move(def)}, defaultValue{static_cast<double>(val)}
{
}

Option::Option(std::string def, double val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(std::string def, std::string val) : text{std::move(def)}, defaultValue{val} {}

}  // namespace CLI_v2
}  // namespace PacBio
