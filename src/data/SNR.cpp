#include <pbcopper/data/SNR.h>

#include <boost/algorithm/clamp.hpp>

#include <ostream>
#include <type_traits>

namespace PacBio {
namespace Data {

static_assert(std::is_trivially_copyable_v<SNR>, "SNR is not a trivially copyable type");

SNR::operator std::vector<float>() const { return {data_[0], data_[1], data_[2], data_[3]}; }

SNR ClampSNR(const SNR& val, const SNR& lo, const SNR& hi) noexcept
{
    return SNR{boost::algorithm::clamp(val[0], lo[0], hi[0]),
               boost::algorithm::clamp(val[1], lo[1], hi[1]),
               boost::algorithm::clamp(val[2], lo[2], hi[2]),
               boost::algorithm::clamp(val[3], lo[3], hi[3])};
}

std::ostream& operator<<(std::ostream& os, const SNR& snr)
{
    return os << "SNR(A=" << snr[0] << ", C=" << snr[1] << ", G=" << snr[2] << ", T=" << snr[3]
              << ')';
}

}  // namespace Data
}  // namespace PacBio
