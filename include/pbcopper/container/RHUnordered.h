#ifndef PBCOPPER_CONTAINER_RH_UNORDERED_H
#define PBCOPPER_CONTAINER_RH_UNORDERED_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/third-party/robin_hood/robin_hood.h>

namespace PacBio {
namespace Container {

static constexpr std::size_t DefaultMaxLoadFactor100 = 80;

// To use a specific hash map:
// RHUnordered{Set,Map}

template <typename Key, typename Hash = robin_hood::hash<Key>,
          typename KeyEqual = std::equal_to<Key>,
          std::size_t MaxLoadFactor100 = DefaultMaxLoadFactor100>
using RHSetBase = robin_hood::unordered_set<Key, Hash, KeyEqual, MaxLoadFactor100>;

template <typename Key, typename Value, typename Hash = robin_hood::hash<Key>,
          typename KeyEqual = std::equal_to<Key>,
          std::size_t MaxLoadFactor100 = DefaultMaxLoadFactor100>
using RHMapBase = robin_hood::unordered_map<Key, Value, Hash, KeyEqual, MaxLoadFactor100>;

template <typename Key, typename Hash = robin_hood::hash<Key>,
          typename KeyEqual = std::equal_to<Key>,
          std::size_t MaxLoadFactor100 = DefaultMaxLoadFactor100>
using RHUnorderedSet = RHSetBase<Key, Hash, KeyEqual, MaxLoadFactor100>;

template <typename Key, typename Value, typename Hash = robin_hood::hash<Key>,
          typename KeyEqual = std::equal_to<Key>,
          std::size_t MaxLoadFactor100 = DefaultMaxLoadFactor100>
using RHUnorderedMap = RHMapBase<Key, Value, Hash, KeyEqual, MaxLoadFactor100>;

// To use the "default" hash map.
// Currently, that's robin_hood because it's fast, but different applications will have different trade-offs.

}  // namespace Container
}  // namespace PacBio

#endif
