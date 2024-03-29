#ifndef PBCOPPER_CONTAINER_BITMASKCONTAINER_H
#define PBCOPPER_CONTAINER_BITMASKCONTAINER_H

#include <pbcopper/PbcopperConfig.h>

#include <boost/integer.hpp>

#include <algorithm>
#include <cassert>
#include <climits>
#include <cstdint>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <type_traits>

namespace PacBio {
namespace Container {

///
/// BitmaskContainer is a template class that packs elements that can be encoded
/// with a maximum of ElementBits into some unsigned integer with a maximum of
/// TotalBits. It does not have a length parameter, assuming that all
/// (TotalBits / ElementBits, i.e., the Capacity) participate in the
/// representation at all times. It is the semantic equivalent of
/// std::array<TotalBits / ElementBits>.
///
template <std::int32_t TotalBits, std::int32_t ElementBits>
class BitmaskContainer
{
public:
    using UnderlyingType = std::conditional_t<
        TotalBits == 8, std::uint8_t,
        std::conditional_t<
            TotalBits == 16, std::uint16_t,
            std::conditional_t<
                TotalBits == 32, std::uint32_t,
                std::conditional_t<TotalBits == 64, std::uint64_t,
                                   std::conditional_t<TotalBits == 128, __uint128_t, void> > > > >;

    // We cap the working value type to a minimum of 32 bits. C++ is full
    // of extremely surprising behavior, and integer promotion is one of
    // them. By only using std::uint32_t as a minimum type, we can avoid these
    // kinds of pitfalls.
    using ValueType = typename boost::uint_t<std::max(32, ElementBits)>::exact;

protected:
    using ComputationType = std::common_type_t<UnderlyingType, ValueType>;

private:
    constexpr static std::int32_t CAPACITY_ = TotalBits / ElementBits;
    static_assert(CAPACITY_ >= 2, "TotalBits is not >= 2 * ElementBits");
    static_assert(TotalBits % ElementBits == 0, "Cannot handle padding bits");

public:
    PB_CUDA_HOST PB_CUDA_DEVICE constexpr static std::int32_t Capacity() { return CAPACITY_; }

    constexpr static ComputationType MAXIMUM_VALUE{
        std::numeric_limits<UnderlyingType>::max() >>
        (sizeof(UnderlyingType) * CHAR_BIT - ElementBits)};

public:
    constexpr BitmaskContainer() noexcept = default;

    PB_CUDA_HOST PB_CUDA_DEVICE constexpr explicit BitmaskContainer(
        const UnderlyingType val) noexcept
        : data_{val}
    {}

public:
    template <typename Callable, typename T>
    constexpr static BitmaskContainer MakeFromTransform(Callable transform, const T& input) noexcept
    {
        const std::int32_t inputSize = input.size();
        assert(inputSize <= Capacity());

        UnderlyingType data{};

        for (std::int32_t i = inputSize - 1; i >= 0; --i) {
            data <<= ElementBits;

            const ComputationType val = transform(input[i]);
            assert(val <= MAXIMUM_VALUE);

            data |= val;
        }

        return BitmaskContainer{data};
    }

    template <typename... Args>
    PB_CUDA_HOST PB_CUDA_DEVICE constexpr static BitmaskContainer MakeFromArray(
        Args&&... args) noexcept
    {
        static_assert(sizeof...(Args) <= Capacity());

        UnderlyingType data{};

        std::int32_t idx = 0;
        (..., (assert(static_cast<ComputationType>(args) <= MAXIMUM_VALUE), data |= (args << idx),
               idx += ElementBits));

        return BitmaskContainer{data};
    }

public:
    PB_CUDA_HOST PB_CUDA_DEVICE constexpr ValueType operator[](
        const std::int32_t idx) const noexcept
    {
        assert(idx >= 0);
        assert(idx < Capacity());

        const std::int32_t shiftBits = ElementBits * idx;
        return (data_ >> shiftBits) & MAXIMUM_VALUE;
    }

public:
    PB_CUDA_HOST PB_CUDA_DEVICE constexpr void Clear() noexcept { data_ = 0; }

    PB_CUDA_HOST PB_CUDA_DEVICE constexpr void Set(const std::int32_t idx,
                                                   const ComputationType val) noexcept
    {
        assert(idx >= 0);
        assert(idx < Capacity());
        assert(val <= MAXIMUM_VALUE);

        const std::int32_t shiftBits = ElementBits * idx;
        data_ = (val << shiftBits) | (data_ & ~(MAXIMUM_VALUE << shiftBits));
    }

    PB_CUDA_HOST PB_CUDA_DEVICE constexpr void Remove(const std::int32_t idx) noexcept
    {
        assert(idx >= 0);
        assert(idx < Capacity());

        //                    example position = 1 ─┐
        //                                          v
        //                                 4  3  2  1  0
        //   data_:                       11'01'00'11'10
        //
        //   lowerBitMask:                00'00'00'00'11
        //
        // bases before position:
        //   (data_ & lowerBitMask):      00'00'00'00'10 (lower)
        //
        // bases past position:
        //   (data_ >> 2):                00'11'01'00'11
        //   (~lowerBitMask):             11'11'11'11'00
        //   bit-&:                       00'11'01'00'00 (upper)
        //
        //                                    3  2  1  0
        // lower | upper:                 00'11'01'00'10
        const std::int32_t shiftBits = ElementBits * idx;
        const auto lowerBitMask = (ComputationType{1} << shiftBits) - 1;

        data_ = (data_ & lowerBitMask) | ((data_ >> ElementBits) & ~lowerBitMask);
    }

    PB_CUDA_HOST PB_CUDA_DEVICE constexpr void Insert(const std::int32_t idx,
                                                      const ComputationType val) noexcept
    {
        assert(idx >= 0);
        assert(idx < Capacity());
        assert(val <= MAXIMUM_VALUE);

        const std::int32_t shiftBits = ElementBits * idx;
        const auto lowerBitMask = (ComputationType{1} << shiftBits) - 1;

        data_ =
            (data_ & lowerBitMask) | (val << shiftBits) | ((data_ & ~lowerBitMask) << ElementBits);
    }

protected:
    PB_CUDA_HOST PB_CUDA_DEVICE constexpr static ComputationType GenerateMovePattern(
        const std::int32_t round)
    {
        // TotalBits = 32
        // ElementBits = 2
        //
        // round 0: 00'11'00'11'00'11'00'11'00'11'00'11'00'11'00'11 (<= size 2),  8 repeats
        // round 1: 00'00'11'11'00'00'11'11'00'00'11'11'00'00'11'11 (<= size 4),  4 repeats
        // round 2: 00'00'00'00'11'11'11'11'00'00'00'00'11'11'11'11 (<= size 8),  2 repeats
        // round 3: 00'00'00'00'00'00'00'00'11'11'11'11'11'11'11'11 (<= size 16), 1 repeat

        assert(round >= 0);
        assert(1 << (round + 1) <= Capacity());

        ComputationType result{};
        const std::int32_t lengthOnes = ElementBits * (1 << round);
        const std::int32_t lengthPattern = 2 * lengthOnes;
        const std::int32_t numRepeats = TotalBits / lengthPattern;
        const auto repeatPattern = (ComputationType{1} << lengthOnes) - 1;

        for (std::int32_t i = 0; i < numRepeats; ++i) {
            result |= (repeatPattern << (lengthPattern * i));
        }
        return result;
    }

    // why the template manoeuvring? GCC (unlike Clang) won't unroll the vanilla
    // for loop, even though the compile-time bounds are all known. By using this
    // template, we can force all computation into a constexpr context, which is
    // a lot easier for the optimizer.
    template <std::int32_t round, std::int32_t capacity>
    PB_CUDA_HOST PB_CUDA_DEVICE constexpr void ReverseImpl() noexcept
    {
        if constexpr (capacity > 0) {
            constexpr ComputationType BIT_PATTERN = GenerateMovePattern(round);
            constexpr std::int32_t BITS_TO_SHIFT = ElementBits * (1 << round);

            data_ = ((data_ & ~BIT_PATTERN) >> BITS_TO_SHIFT) |
                    ((data_ & BIT_PATTERN) << BITS_TO_SHIFT);

            ReverseImpl<round + 1, capacity / 2>();
        }
    }

public:
    PB_CUDA_HOST PB_CUDA_DEVICE constexpr void Reverse() noexcept
    {
        ReverseImpl<0, Capacity() - 1>();
    }

public:
    PB_CUDA_HOST PB_CUDA_DEVICE constexpr UnderlyingType RawData() const noexcept { return data_; }

public:
    constexpr bool operator==(const BitmaskContainer& rhs) const noexcept
    {
        return data_ == rhs.data_;
    }

public:
    friend std::ostream& operator<<(std::ostream& os, const BitmaskContainer cont)
    {
        os << "BitmaskContainer(" << cont[0];
        for (std::int32_t i = 1; i < cont.Capacity(); ++i) {
            os << ", " << cont[i];
        }
        return os << ')';
    }

protected:
    UnderlyingType data_ = 0;
};

}  // namespace Container
}  // namespace PacBio

#endif  // PBCOPPER_CONTAINER_BITMASKCONTAINER_H
