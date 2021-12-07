#include <pbcopper/data/QualityValue.h>

#include <algorithm>
#include <type_traits>

#include <cstdint>

namespace PacBio {
namespace Data {

const uint8_t QualityValue::MAX = 93;

QualityValue::QualityValue(const uint8_t value)
    : value_{// clamp QV
             std::min(value, QualityValue::MAX)}
{
}

char QualityValue::Fastq() const { return static_cast<char>(value_ + 33); }

QualityValue::operator uint8_t() const noexcept { return value_; }

QualityValue QualityValue::FromFastq(const char c) { return {static_cast<uint8_t>(c - 33)}; }

}  // namespace Data
}  // namespace PacBio
