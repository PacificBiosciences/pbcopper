#ifndef PBCOPPER_CONTAINER_DNASTRING_H
#define PBCOPPER_CONTAINER_DNASTRING_H

#include <pbcopper/PbcopperConfig.h>

#include "BitContainer.h"
#include "BitConversion.h"
#include "BitmaskContainer.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>

namespace PacBio {
namespace Container {

template <std::int32_t TotalBits, std::int32_t ElementBits, bool FixedWidth>
class DNA2bitStringImpl
    : public std::conditional_t<FixedWidth, BitmaskContainer<TotalBits, ElementBits>,
                                BitContainer<TotalBits, ElementBits>>
{
private:
    using Base = std::conditional_t<FixedWidth, BitmaskContainer<TotalBits, ElementBits>,
                                    BitContainer<TotalBits, ElementBits>>;
    using UnderlyingType = typename Base::UnderlyingType;

private:
    PB_CUDA_HOST PB_CUDA_DEVICE constexpr std::int32_t SizeImpl() const noexcept
    {
        if constexpr (FixedWidth) {
            return this->Capacity();
        } else {
            return this->Size();
        }
    }

public:
    PB_CUDA_HOST PB_CUDA_DEVICE constexpr explicit DNA2bitStringImpl(const Base base) noexcept
        : Base{base}
    {}

    PB_CUDA_HOST PB_CUDA_DEVICE constexpr explicit DNA2bitStringImpl(
        const UnderlyingType val, const std::int32_t size) noexcept
        : Base{val, size}
    {}

    constexpr explicit DNA2bitStringImpl(const std::string_view str) noexcept
    {
        const std::int32_t strSize = str.size();
        assert(strSize <= this->Capacity());

        // variable-width
        if constexpr (!FixedWidth) {
            this->size_ = strSize;
        }

        for (std::int32_t i = 0; i < strSize; ++i) {
            this->Set(i, ConvertAsciiTo2bit(str[i]));
        }
    }

    // variable-width
    template <bool F = FixedWidth, typename std::enable_if_t<!F, std::int32_t> = 0>
    constexpr DNA2bitStringImpl(const DNA2bitStringImpl<TotalBits, ElementBits, true> cont,
                                const std::int32_t size) noexcept
        : Base{cont, size}
    {}

public:
    explicit operator std::string() const
    {
        std::string result;
        result.reserve(this->Capacity());

        for (std::int32_t i = 0; i < this->SizeImpl(); ++i) {
            result.push_back(Container::Convert2bitToAscii((*this)[i]));
        }

        return result;
    }

public:
    PB_CUDA_HOST PB_CUDA_DEVICE constexpr std::int32_t Length() const noexcept
    {
        return SizeImpl();
    }

    PB_CUDA_HOST PB_CUDA_DEVICE constexpr void ReverseComplement() noexcept
    {
        this->data_ = ~this->data_;
        this->Reverse();
    }

    PB_CUDA_HOST PB_CUDA_DEVICE constexpr DNA2bitStringImpl Range(
        const std::int32_t pos, const std::int32_t len) const noexcept
    {
        return DNA2bitStringImpl{Base::Range(pos, len)};
    }
};

using DNA2bitStringFixed32 = DNA2bitStringImpl<32, 2, true>;
using DNA2bitStringFixed64 = DNA2bitStringImpl<64, 2, true>;

using DNA2bitStringVariable32 = DNA2bitStringImpl<32, 2, false>;
using DNA2bitStringVariable64 = DNA2bitStringImpl<64, 2, false>;

}  // namespace Container
}  // namespace PacBio

#endif  // PBCOPPER_CONTAINER_DNASTRING_H
