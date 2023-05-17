#include <pbcopper/cuda/AsciiConversion.h>

#include <gtest/gtest.h>

#include <pbcopper/container/DNAString.h>

#include <array>
#include <cstdint>
#include <cstring>
#include <random>
#include <string>

using namespace PacBio;

TEST(Cuda_AsciiConversion, SingleBase)
{
    {
        std::uint32_t arr[8] = {'A'};
        const auto result = Cuda::AsciiToBitmaskContainer<64, 2>(arr, 1);
        EXPECT_EQ(result.RawData(), 0);
    }

    {
        std::uint32_t arr[8] = {'C'};
        const auto result = Cuda::AsciiToBitmaskContainer<64, 2>(arr, 1);
        EXPECT_EQ(result.RawData(), 1);
    }

    {
        std::uint32_t arr[8] = {'G'};
        const auto result = Cuda::AsciiToBitmaskContainer<64, 2>(arr, 1);
        EXPECT_EQ(result.RawData(), 2);
    }

    {
        std::uint32_t arr[8] = {'T'};
        const auto result = Cuda::AsciiToBitmaskContainer<64, 2>(arr, 1);
        EXPECT_EQ(result.RawData(), 3);
    }
}

TEST(Cuda_AsciiConversion, Comprehensive)
{
    std::random_device rd;
    std::mt19937 gen{rd()};

    const std::int32_t rounds = 100'000;

    constexpr std::array<char, 4> DNA_BASES{'A', 'C', 'G', 'T'};
    std::uniform_int_distribution<std::int32_t> baseDistribution{0, 3};
    std::uniform_int_distribution<std::int32_t> lengthDistribution{0, 31};

    for (std::int32_t r = 0; r < rounds; ++r) {
        // generate a random ASCII string
        const std::int32_t length = lengthDistribution(gen);
        std::string ascii1;
        for (std::int32_t i = 0; i < length; ++i) {
            ascii1.push_back(DNA_BASES[baseDistribution(gen)]);
        }

        std::uint32_t arr1[8] = {};
        std::memcpy(arr1, ascii1.c_str(), length);

        // test CUDA ASCII → 2-bit conversion
        const auto comp1 = Cuda::AsciiToBitmaskContainer<64, 2>(arr1, length);
        const Container::DNA2bitStringVariable64 dna1{ascii1};
        EXPECT_EQ(dna1.RawData(), comp1.RawData());
        EXPECT_EQ(dna1.Length(), length);

        // test CUDA 2-bit conversion → ASCII
        std::uint32_t arr2[8] = {};
        Cuda::BitmaskContainerToAscii(comp1, length, arr2);
        EXPECT_EQ(std::strcmp(ascii1.c_str(), reinterpret_cast<const char*>(arr2)), 0);
        static_assert(sizeof(arr1) == 32);
        EXPECT_EQ(std::memcmp(arr1, arr2, sizeof(arr1)), 0);

        // test CUDA ASCII → 2-bit conversion (roundtrip)
        const auto comp2 = Cuda::AsciiToBitmaskContainer<64, 2>(arr2, length);
        EXPECT_EQ(dna1.RawData(), comp2.RawData());
        EXPECT_EQ(comp1, comp2);
    }
}
