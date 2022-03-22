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
template <int32_t TotalBits, int32_t ElementBits>
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

    constexpr BitContainer(const UnderlyingType val, const int32_t size) noexcept
        : Base{val}, size_{size}
    {
        assert(size_ >= 0);
        assert(size_ <= Capacity());

#ifndef NDEBUG
        // check whether bits in "inactive" positions are set
        constexpr ComputationType allOnes = -1;

        assert(val <= (size_ ? (allOnes >> (ElementBits * (Capacity() - size_))) : 0));
#endif
    }

protected:
    constexpr BitContainer(const Base val, const int32_t size) noexcept : Base(val), size_{size}
    {
        assert(size_ >= 0);
        assert(size_ <= Capacity());
    }

public:
    template <typename Callable, typename T>
    constexpr static BitContainer MakeFromTransform(Callable transform, const T& input) noexcept
    {
        return {Base::MakeFromTransform(transform, input), static_cast<int32_t>(input.size())};
    }

    template <typename... Args>
    constexpr static BitContainer MakeFromArray(Args&&... args) noexcept
    {
        return {Base::MakeFromArray(std::forward<Args>(args)...), sizeof...(Args)};
    }

public:
    using Base::Capacity;
    using Base::MaximumValue;

    constexpr int32_t Size() const noexcept { return size_; }

    constexpr ValueType operator[](const int32_t idx) const noexcept
    {
        assert(idx < Size());

        return Base::operator[](idx);
    }

public:
    constexpr void Clear() noexcept
    {
        Base::Clear();
        size_ = 0;
    }

    constexpr void Set(const int32_t idx, const ValueType val) noexcept
    {
        assert(idx < Size());

        Base::Set(idx, val);
    }

    constexpr void Remove(const int32_t idx) noexcept
    {
        assert(idx < Size());

        Base::Remove(idx);
        --size_;
    }

    constexpr void Insert(const int32_t idx, const ValueType val) noexcept
    {
        assert(idx <= Size());

        Base::Insert(idx, val);
        // truncating insertion, shifts the last element into oblivion
        size_ = std::min(size_ + 1, Capacity());
    }

    constexpr void PushBack(const ValueType val) noexcept
    {
        assert(Size() < Capacity());

        Insert(Size(), val);
    }

    constexpr void Reverse() noexcept
    {
        Base::Reverse();

        const int32_t unusedElems = Capacity() - std::max(1, Size());
        Base::data_ >>= (ElementBits * unusedElems);
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
        for (int32_t i = 0; i < cont.Size(); ++i) {
            os << (i ? ", " : "") << cont[i];
        }
        return os << ')';
    }

protected:
    int32_t size_ = 0;
};

}  // namespace Container
}  // namespace PacBio

#endif  // PBCOPPER_CONTAINER_BITCONTAINER_H
