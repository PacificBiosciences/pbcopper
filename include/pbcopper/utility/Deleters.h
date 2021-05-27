#ifndef PBCOPPER_UTILITY_DELETERS_H
#define PBCOPPER_UTILITY_DELETERS_H

#include <cstdio>

namespace PacBio {
namespace Utility {

//
// Convenience deleters intended for use with std::shared_ptr<T>,
// std::unique_ptr<T>, etc.
//

struct FileDeleter
{
    void operator()(std::FILE* fp) const noexcept;
};

// For pointers originating from C 'malloc' (and friends), instead of 'new'
struct FreeDeleter
{
    void operator()(void* p) const noexcept;

    void operator()(const void* p) const noexcept;
};

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_DELETERS_H
