#ifndef PBCOPPER_CONTAINER_BITCONTAINER_H
#define PBCOPPER_CONTAINER_BITCONTAINER_H

#include <pbcopper/PbcopperConfig.h>

#include "BitmaskContainer.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <ostream>
#include <type_traits>
#include <utility>

namespace PacBio {
namespace Container {

///
/// BitContainer is a template class that uses BitmaskContainer to pack elements
/// with an associated Size() function. It can be thought of as a small,
/// tightly packed std::vector that is constexpr and GPU friendly. It is the
/// semantic equivalent of boost::container::static_vector<TotalBits / ElementBits>.
///
template <std::int32_t TotalBits, std::int32_t ElementBits>
class BitContainer : protected BitmaskContainer<TotalBits, ElementBits>
{
private:
    using Base = BitmaskContainer<TotalBits, ElementBits>;

public:
    using UnderlyingType = typename Base::UnderlyingType;
    using ValueType = typename Base::ValueType;

protected:
    using ComputationType = typename Base::ComputationType;

public:
    constexpr BitContainer() noexcept = default;

    PB_CUDA_HOST PB_CUDA_DEVICE constexpr BitContainer(const UnderlyingType val,
                                                       const std::int32_t size) noexcept
        : Base{val}, size_{size}
    {
        assert(size_ >= 0);
        assert(size_ <= Capacity());

#ifndef NDEBUG
        // check whether bits in "inactive" positions are set
        constexpr auto ALL_ONES = static_cast<ComputationType>(-1);

        assert(val <= (size_ ? (ALL_ONES >> (ElementBits * (Capacity() - size_))) : 0));
#endif
    }

protected:
    constexpr BitContainer(const Base val, const std::int32_t size) noexcept
        : Base(val), size_{size}
    {
        assert(size_ >= 0);
        assert(size_ <= Capacity());
    }

public:
    template <typename Callable, typename T>
    constexpr static BitContainer MakeFromTransform(Callable transform, const T& input) noexcept
    {
        return {Base::MakeFromTransform(transform, input), static_cast<std::int32_t>(input.size())};
    }

    template <typename... Args>
    constexpr static BitContainer MakeFromArray(Args&&... args) noexcept
    {
        return {Base::MakeFromArray(std::forward<Args>(args)...), sizeof...(Args)};
    }

public:
    using Base::Capacity;
    using Base::MAXIMUM_VALUE;

    PB_CUDA_HOST PB_CUDA_DEVICE constexpr std::int32_t Size() const noexcept { return size_; }

    PB_CUDA_HOST PB_CUDA_DEVICE constexpr ValueType operator[](
        const std::int32_t idx) const noexcept
    {
        assert(idx < Size());

        return Base::operator[](idx);
    }

public:
    PB_CUDA_HOST PB_CUDA_DEVICE constexpr void Clear() noexcept
    {
        Base::Clear();
        size_ = 0;
    }

    PB_CUDA_HOST PB_CUDA_DEVICE constexpr void Set(const std::int32_t idx,
                                                   const ValueType val) noexcept
    {
        assert(idx < Size());

        Base::Set(idx, val);
    }

    PB_CUDA_HOST PB_CUDA_DEVICE constexpr void Remove(const std::int32_t idx) noexcept
    {
        assert(idx < Size());

        Base::Remove(idx);
        --size_;
    }

    PB_CUDA_HOST PB_CUDA_DEVICE constexpr void Insert(const std::int32_t idx,
                                                      const ValueType val) noexcept
    {
        assert(idx <= Size());

        Base::Insert(idx, val);
        // truncating insertion, shifts the last element into oblivion
#ifndef __CUDA_ARCH__
        // host (uses <algorithm>)
        using std::min;
#endif
        size_ = min(size_ + 1, Capacity());
    }

    PB_CUDA_HOST PB_CUDA_DEVICE constexpr void PushBack(const ValueType val) noexcept
    {
        assert(Size() < Capacity());

        Insert(Size(), val);
    }

    PB_CUDA_HOST PB_CUDA_DEVICE constexpr void Reverse() noexcept
    {
#ifndef __CUDA_ARCH__  // host
        using std::max;
#endif

        Base::Reverse();

        const std::int32_t unusedElems = Capacity() - max(1, Size());
        Base::data_ >>= (ElementBits * unusedElems);
    }

    PB_CUDA_HOST PB_CUDA_DEVICE constexpr BitContainer Range(const std::int32_t pos,
                                                             const std::int32_t len) const noexcept
    {
#ifndef __CUDA_ARCH__  // host
        using std::min;
#endif

        assert(pos >= 0);
        assert(pos <= Size());
        assert(len >= 0);

        const std::int32_t pastEndIdx = min(Size(), pos + len);
        const std::int32_t unusedElems = Capacity() - pastEndIdx;

        if ((unusedElems == Capacity()) || (pos == Capacity())) {
            // avoid UB by shifting TotalBits
            return {};
        }

        constexpr auto ALL_ONES = static_cast<ComputationType>(-1);
        const ComputationType bitmask{ALL_ONES >> (ElementBits * unusedElems)};
        const ComputationType newData = (RawData() & bitmask) >> (ElementBits * pos);
        return {newData, pastEndIdx - pos};
    }

public:
    using Base::RawData;

public:
    constexpr bool operator==(const BitContainer& rhs) const noexcept
    {
        // yes, the bit-wise '&' is correct here:
        // By avoiding short-circuit evaluation, we can avoid branches
        // in GCC's code, which improves ILP and therefore performance.
        return (size_ == rhs.size_) &
               (static_cast<const Base&>(*this) == static_cast<const Base&>(rhs));
    }

    constexpr bool operator!=(const BitContainer& rhs) const noexcept { return !(*this == rhs); }

public:
    friend std::ostream& operator<<(std::ostream& os, const BitContainer& cont)
    {
        os << "BitContainer(Size=" << cont.Size() << ", BitmaskContainer(";
        for (std::int32_t i = 0; i < cont.Size(); ++i) {
            os << (i ? ", " : "") << cont[i];
        }
        return os << ')';
    }

protected:
    std::int32_t size_ = 0;
};

}  // namespace Container
}  // namespace PacBio

#endif  // PBCOPPER_CONTAINER_BITCONTAINER_H
