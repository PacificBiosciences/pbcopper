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

template <int32_t TotalBits, int32_t ElementBits, bool FixedWidth>
class DNA2bitStringImpl
    : public std::conditional_t<FixedWidth, BitmaskContainer<TotalBits, ElementBits>,
                                BitContainer<TotalBits, ElementBits>>
{
private:
    using Base = std::conditional_t<FixedWidth, BitmaskContainer<TotalBits, ElementBits>,
                                    BitContainer<TotalBits, ElementBits>>;

private:
    constexpr int32_t SizeImpl() const noexcept
    {
        if constexpr (FixedWidth) {
            return this->Capacity();
        } else {
            return this->Size();
        }
    }

public:
    using Base::Base;

    constexpr explicit DNA2bitStringImpl(const std::string_view str) noexcept
    {
        const int32_t strSize = str.size();
        assert(strSize <= this->Capacity());

        // variable-width
        if constexpr (!FixedWidth) {
            this->size_ = strSize;
        }

        for (int32_t i = 0; i < strSize; ++i) {
            this->Set(i, ConvertAsciiTo2bit(str[i]));
        }
    }

    // variable-width
    template <bool F = FixedWidth, typename std::enable_if_t<!F, int32_t> = 0>
    constexpr DNA2bitStringImpl(const DNA2bitStringImpl<TotalBits, ElementBits, true> cont,
                                const int32_t size) noexcept
        : Base{cont, size}
    {}

public:
    explicit operator std::string() const
    {
        std::string result;
        result.reserve(this->Capacity());

        for (int32_t i = 0; i < this->SizeImpl(); ++i) {
            result.push_back(Container::Convert2bitToAscii((*this)[i]));
        }

        return result;
    }

public:
    constexpr void ReverseComp() noexcept
    {
        this->data_ = ~this->data_;
        this->Reverse();
    }
};

using DNA2bitStringFixed32 = DNA2bitStringImpl<32, 2, true>;
using DNA2bitStringFixed64 = DNA2bitStringImpl<64, 2, true>;

using DNA2bitStringVariable32 = DNA2bitStringImpl<32, 2, false>;
using DNA2bitStringVariable64 = DNA2bitStringImpl<64, 2, false>;

}  // namespace Container
}  // namespace PacBio

#endif  // PBCOPPER_CONTAINER_DNASTRING_H
