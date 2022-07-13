#ifndef PBCOPPER_CONTAINER_CONTAINS_H
#define PBCOPPER_CONTAINER_CONTAINS_H

#include <pbcopper/PbcopperConfig.h>

#include <algorithm>
#include <iterator>

namespace PacBio {
namespace Container {

// Helper functions for checking if containers contain a value, when a
// member c.contains(x) is not available.

template <typename ContainerType, typename T>
concept HasFindMember = requires(const ContainerType& c, const T& value)
{
    c.find(value);
};

template <typename ContainerType, typename T>
requires HasFindMember<ContainerType, T>
bool Contains(const ContainerType& c, const T& value) noexcept
{
    return c.find(value) != std::end(c);
}

template <typename ContainerType, typename T>
bool Contains(const ContainerType& c, const T& value) noexcept
{
    return std::find(std::begin(c), std::end(c), value) != std::end(c);
}

}  // namespace Container
}  // namespace PacBio

#endif  // PBCOPPER_CONTAINER_CONTAINS_H
