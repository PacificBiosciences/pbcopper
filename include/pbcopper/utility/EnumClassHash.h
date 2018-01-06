#ifndef PBCOPPER_UTILITY_ENUMCLASSHASH_H
#define PBCOPPER_UTILITY_ENUMCLASSHASH_H

#include <cstddef>

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Utility {

///
/// \brief The EnumClassHash struct enables the use of enum class types as keys
///        for std::unordered_map.
///
/// Allows something like:
///
/// \code{.cpp}
///    std::unordered_map<Key_t, Value_t, EnumClassHash> myLookup;
/// \endcode
///
/// where Key_t is an enum class. Without this sort of extra hand-holding to
/// provide a 'manual' hash value, enum classes as keys will fail to compile.
///
/// \note This approach might be unnecessary in C++14, if I understand some of
/// the changes correctly. But this works for C++11 and should continue beyond.
///
/// \sa http://stackoverflow.com/questions/18837857/cant-use-enum-class-as-unordered-map-key
///
struct EnumClassHash
{
    template<typename T> size_t operator()(const T t) const
    { return static_cast<size_t>(t); }
};

} // namespace Utility
} // namespace PacBio

#endif // PBCOPPER_UTILITY_ENUMCLASSHASH_H
