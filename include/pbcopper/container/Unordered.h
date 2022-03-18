#ifndef PBCOPPER_CONTAINER_UNORDERED_H
#define PBCOPPER_CONTAINER_UNORDERED_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/container/RHUnordered.h>

namespace PacBio {
namespace Container {

// To use a specific hash map:
// RHUnordered{Set,Map}
// Currently, RobinHood is the default, but this may change.
// To choose robin-hood specifically, use RHUnorderedSet and RHUnorderedMap

template <typename Key, typename Hash = robin_hood::hash<Key>,
          typename KeyEqual = std::equal_to<Key>, size_t MaxLoadFactor100 = DefaultMaxLoadFactor100>
using UnorderedSet = RHUnorderedSet<Key, Hash, KeyEqual, MaxLoadFactor100>;

template <typename Key, typename Value, typename Hash = robin_hood::hash<Key>,
          typename KeyEqual = std::equal_to<Key>, size_t MaxLoadFactor100 = DefaultMaxLoadFactor100>
using UnorderedMap = RHUnorderedMap<Key, Value, Hash, KeyEqual, MaxLoadFactor100>;

}  // namespace Container
}  // namespace PacBio

#endif
