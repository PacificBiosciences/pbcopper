#include <pbcopper/data/SNR.h>

#include <cassert>
#include <cstring>

#include <algorithm>
#include <ostream>
#include <string>
#include <type_traits>

#include <boost/algorithm/clamp.hpp>
#include <boost/assert.hpp>

namespace PacBio {
namespace Data {

SNR::SNR(const float a, const float c, const float g, const float t) noexcept
    : A{a}, C{c}, G{g}, T{t}
{
}

SNR::SNR(const std::vector<float>& snrs) noexcept
{
    BOOST_ASSERT_MSG(snrs.size() == 4,
                     "[pbcopper] SNR ERROR: initialized with a vector of not exactly 4 elements");

    static_assert(std::is_trivially_copyable<SNR>::value, "SNR is not a trivially copyable type");
    std::memcpy(this, snrs.data(), sizeof(float) * 4);
}

SNR::SNR(const float (&snrs)[4]) noexcept
{
    static_assert(std::is_trivially_copyable<SNR>::value, "SNR is not a trivially copyable type");
    std::memcpy(this, &snrs, sizeof(float) * 4);
}

SNR::operator std::vector<float>() const { return {A, C, G, T}; }

const float& SNR::operator[](const int i) const noexcept
{
    BOOST_ASSERT_MSG((0 <= i) && (i < 4), "[pbcopper] SNR ERROR: index is out of bounds [0, 4)");

    static_assert(offsetof(SNR, A) == 0 * sizeof(float), "A has wrong offset within struct");
    static_assert(offsetof(SNR, C) == 1 * sizeof(float), "C has wrong offset within struct");
    static_assert(offsetof(SNR, G) == 2 * sizeof(float), "G has wrong offset within struct");
    static_assert(offsetof(SNR, T) == 3 * sizeof(float), "T has wrong offset within struct");
    static_assert(std::is_standard_layout<SNR>::value, "SNR is not a standard-layout class");

    return *(reinterpret_cast<const float*>(this) + i);
}

float& SNR::operator[](const int i) noexcept
{
    // casting away const when underlying object is non-const, is well-defined
    // Effective C++ (Third Edition) - Item 3
    return const_cast<float&>(static_cast<const SNR&>(*this)[i]);
}

bool SNR::operator==(const SNR& other) const noexcept
{
    return std::tie(A, C, G, T) == std::tie(other.A, other.C, other.G, other.T);
}

bool SNR::operator!=(const SNR& other) const noexcept { return !(*this == other); }

float SNR::Minimum() const noexcept { return std::min(std::min(A, C), std::min(G, T)); }

SNR ClampSNR(const SNR& val, const SNR& lo, const SNR& hi) noexcept
{
    return SNR{
        boost::algorithm::clamp(val.A, lo.A, hi.A), boost::algorithm::clamp(val.C, lo.C, hi.C),
        boost::algorithm::clamp(val.G, lo.G, hi.G), boost::algorithm::clamp(val.T, lo.T, hi.T)};
}

std::ostream& operator<<(std::ostream& os, const SNR& snr)
{
    return os << "SNR(A=" << snr.A << ", C=" << snr.C << ", G=" << snr.G << ", T=" << snr.T << ')';
}

}  // namespace Data
}  // namespace PacBio
