#include <pbcopper/cli2/Option.h>

#include <type_traits>

namespace PacBio {
namespace CLI_v2 {

Option::Option(std::string def) : text{std::move(def)} {}

Option::Option(std::string def, bool on) : text{std::move(def)}, defaultValue{on} {}

Option::Option(std::string def, std::int8_t val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(std::string def, std::int16_t val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(std::string def, std::int32_t val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(std::string def, std::int64_t val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(std::string def, std::uint8_t val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(std::string def, std::uint16_t val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(std::string def, std::uint32_t val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(std::string def, std::uint64_t val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(std::string def, float val)
    : text{std::move(def)}, defaultValue{static_cast<double>(val)}
{}

Option::Option(std::string def, double val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(std::string def, std::string val) : text{std::move(def)}, defaultValue{val} {}

}  // namespace CLI_v2
}  // namespace PacBio
