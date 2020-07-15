// Author: David Seifert

#ifndef PBCOPPER_UTILITY_STRINGVIEW_H
#define PBCOPPER_UTILITY_STRINGVIEW_H

#include <pbcopper/PbcopperConfig.h>

#include <cstdint>

#include <algorithm>
#include <iosfwd>
#include <string>

#include <boost/assert.hpp>

namespace PacBio {
namespace Utility {

// NIH class copying std::/boost::string_view, as this
// has to be compilable on the host and device.

class StringView
{
public:
    StringView(const char* str) noexcept : ptr_{str}, len_{0}
    {
        BOOST_ASSERT_MSG(str, "Provided a null pointer");
        while (ptr_[len_])
            ++len_;
    }

    StringView(const std::string& str) noexcept : ptr_{str.data()}, len_(str.length()) {}

    PB_CUDA_HOST PB_CUDA_DEVICE StringView(const char* str, const int32_t len) noexcept
        : ptr_{str}, len_{len}
    {
        BOOST_ASSERT_MSG(str, "Provided a null pointer");
    }

public:
    PB_CUDA_HOST PB_CUDA_DEVICE const char* Data() const noexcept { return ptr_; }

    PB_CUDA_HOST PB_CUDA_DEVICE int32_t Size() const noexcept { return len_; }

    PB_CUDA_HOST PB_CUDA_DEVICE int32_t Length() const noexcept { return Size(); }

    PB_CUDA_HOST PB_CUDA_DEVICE char operator[](const int32_t pos) const noexcept
    {
        BOOST_ASSERT_MSG(pos >= 0, "Provided a negative index");
        BOOST_ASSERT_MSG(pos < Size(), "Provided an out-of-bounds index");

        return ptr_[pos];
    }

    PB_CUDA_HOST PB_CUDA_DEVICE char Back() const noexcept { return ptr_[Size() - 1]; }

    PB_CUDA_HOST PB_CUDA_DEVICE StringView Substr(const int32_t pos, const int32_t newLength) const
        noexcept
    {
        BOOST_ASSERT_MSG(pos <= Size(), "New pos is out of bounds");

        return StringView{Data() + pos, std::min(Size() - pos, newLength)};
    }

private:
    const char* ptr_;
    int32_t len_;
};

inline bool operator==(const StringView lhs, const StringView rhs) noexcept
{
    if (lhs.Size() != rhs.Size()) return false;

    for (int32_t i = 0; i < lhs.Size(); ++i) {
        if (lhs[i] != rhs[i]) return false;
    }

    return true;
}

std::ostream& operator<<(std::ostream& os, const StringView str);

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_STRINGVIEW_H
