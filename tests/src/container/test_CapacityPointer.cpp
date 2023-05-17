#include <pbcopper/container/CapacityPointer.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <sstream>
#include <string_view>

using namespace PacBio;

TEST(Container_CapacityPointer, Basic)
{
    static std::int32_t deallocations = 0;

    {
        struct DummyAllocate
        {
            std::byte* operator()(const std::ptrdiff_t size) const noexcept
            {
                return static_cast<std::byte*>(std::malloc(size));
            }
        };

        struct DummyDeallocate
        {
            void operator()(std::byte* ptr) const noexcept
            {
                std::free(ptr);
                ++deallocations;
            }
        };

        Container::CapacityPointer<std::byte, DummyAllocate, DummyDeallocate> ptr{12};
        EXPECT_EQ(ptr.Capacity(), 12);

        ptr.Reserve(16);
        EXPECT_EQ(ptr.Capacity(), 18);  // 12 * 150% == 18
        std::fill_n(ptr.get(), ptr.Capacity(), std::byte{});

        const std::string_view str{"CapacityPointer"};
        std::copy(std::cbegin(str), std::cend(str), reinterpret_cast<char*>(ptr.get()));

        std::ostringstream os;
        os << reinterpret_cast<const char*>(ptr.get());

        EXPECT_EQ(os.str(), str);
    }

    EXPECT_EQ(deallocations, 2);
}
