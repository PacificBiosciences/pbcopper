#include <pbcopper/cli2/Option.h>

namespace PacBio {
namespace CLI_v2 {

Option::Option(std::string def) : text{std::move(def)}, defaultValue{boost::none} {}

Option::Option(std::string def, bool on) : text{std::move(def)}, defaultValue{on} {}

Option::Option(std::string def, int val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(std::string def, unsigned int val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(std::string def, float val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(std::string def, std::string val) : text{std::move(def)}, defaultValue{val} {}

Option::Option(const Option&) = default;

Option::Option(Option&&) noexcept = default;

Option& Option::operator=(const Option&) = default;

Option& Option::operator=(Option&&) noexcept = default;

Option::~Option() = default;

}  // namespace CLI_v2
}  // namespace PacBio
