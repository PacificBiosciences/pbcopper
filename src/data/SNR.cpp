#include <pbcopper/data/SNR.h>

#include <ostream>
#include <type_traits>

namespace PacBio {
namespace Data {

static_assert(std::is_trivially_copyable_v<SNR>, "SNR is not a trivially copyable type");

SNR::operator std::vector<float>() const { return {data_[0], data_[1], data_[2], data_[3]}; }

std::ostream& operator<<(std::ostream& os, const SNR& snr)
{
    return os << "SNR(A=" << snr[0] << ", C=" << snr[1] << ", G=" << snr[2] << ", T=" << snr[3]
              << ')';
}

}  // namespace Data
}  // namespace PacBio
