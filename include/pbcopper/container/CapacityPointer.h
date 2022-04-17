#ifndef PBCOPPER_CONTAINER_CAPACITYPOINTER_H
#define PBCOPPER_CONTAINER_CAPACITYPOINTER_H

#include <pbcopper/PbcopperConfig.h>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <memory>

namespace PacBio {
namespace Container {

///
/// OneHalfGrowthPolicy grows the capacity by 50% relative to the current capacity.
///
class OneHalfGrowthPolicy
{
public:
    constexpr ptrdiff_t operator()(const ptrdiff_t currentCapacity) const noexcept
    {
        assert(currentCapacity >= 0);
        return currentCapacity * 3 / 2ULL;
    }
};

///
/// CapacityPointer is a std::unique_ptr<T, Deleter> that knows the size it has
/// allocated. Similar to a std::vector<T>, its capacity can grow. Importantly,
/// GrowthPolicy has to be a stateless functor that returns at least the same
/// capacity as given to it.
///
template <typename T, typename Allocator, typename Deleter,
          typename GrowthPolicy = OneHalfGrowthPolicy>
class CapacityPointer : private std::unique_ptr<T, Deleter>, private Allocator, private GrowthPolicy
{
public:
    constexpr CapacityPointer() = default;

    CapacityPointer(const ptrdiff_t capacity)
        : std::unique_ptr<T, Deleter>{static_cast<const Allocator&>(*this)(capacity)}
        , capacity_{capacity}
    {
        assert(capacity_ >= 0);
    }

public:
    using std::unique_ptr<T, Deleter>::get;

public:
    ptrdiff_t Capacity() const noexcept { return capacity_; }

    void Reserve(const ptrdiff_t capacity)
    {
        assert(capacity >= 0);

        if (capacity > capacity_) {
            const ptrdiff_t putativeCapacity = static_cast<const GrowthPolicy&>(*this)(capacity_);
            assert(putativeCapacity >= capacity_);

            const ptrdiff_t newCapacity = std::max(capacity, putativeCapacity);

            T* newPtr = static_cast<const Allocator&>(*this)(newCapacity);
            assert(newPtr);
            std::unique_ptr<T, Deleter>::reset(newPtr);

            capacity_ = newCapacity;
        }
    }

private:
    ptrdiff_t capacity_ = 0;
};

}  // namespace Container
}  // namespace PacBio

#endif  // PBCOPPER_CONTAINER_CAPACITYPOINTER_H
