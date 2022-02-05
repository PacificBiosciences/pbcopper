#include <pbcopper/container/BitmaskContainer.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <vector>

#include <pbcopper/container/BitContainer.h>

using namespace PacBio;

namespace PacBio {
namespace Container {

template <int32_t TotalBits, int32_t ElementBits>
bool operator==(
    const BitmaskContainer<TotalBits, ElementBits>& lhs,
    const std::vector<typename BitmaskContainer<TotalBits, ElementBits>::ValueType>& rhs)
{
    EXPECT_TRUE(rhs.size() <= lhs.Capacity());

    for (size_t i = 0; i < rhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

}  // namespace Container
}  // namespace PacBio

TEST(Container_BitmaskContainer, Constexpr)
{
    //                                                            T  G  C  A
    //                                                            3  2  1  0
    constexpr Container::BitmaskContainer<32, 8> constexprBmct{0x54'47'43'41U};

    EXPECT_EQ(sizeof(constexprBmct), sizeof(uint32_t));
    EXPECT_EQ(constexprBmct.Capacity(), 4);

    EXPECT_EQ(constexprBmct[0], 'A');
    EXPECT_EQ(constexprBmct[1], 'C');
    EXPECT_EQ(constexprBmct[2], 'G');
    EXPECT_EQ(constexprBmct[3], 'T');
}

TEST(Container_BitmaskContainer, VariadicCtor)
{
    constexpr auto constexprBmct =
        Container::BitmaskContainer<32, 8>::MakeFromArray('G', 'A', 'T', 'C');

    EXPECT_EQ(sizeof(constexprBmct), sizeof(uint32_t));
    EXPECT_EQ(constexprBmct.Capacity(), 4);

    EXPECT_EQ(constexprBmct[0], 'G');
    EXPECT_EQ(constexprBmct[1], 'A');
    EXPECT_EQ(constexprBmct[2], 'T');
    EXPECT_EQ(constexprBmct[3], 'C');
    EXPECT_EQ(constexprBmct.RawData(), 0x43'54'41'47U);
}

TEST(Container_BitmaskContainer, ConstexprDependencyInjection)
{
    constexpr std::string_view str{"ACGT"};
    constexpr auto converter = [](const char c) {
        if (c == 'T') {
            return 0;
        } else if (c == 'G') {
            return 1;
        } else if (c == 'C') {
            return 2;
        } else if (c == 'A') {
            return 3;
        }

        throw std::logic_error{"encountered an invalid base"};
    };

    constexpr auto constexprBmctDI =
        Container::BitmaskContainer<8, 2>::MakeFromTransform(converter, str);
    EXPECT_EQ(sizeof(constexprBmctDI), sizeof(uint8_t));
    EXPECT_EQ(constexprBmctDI.Capacity(), 4);

    EXPECT_EQ(constexprBmctDI[0], 3);
    EXPECT_EQ(constexprBmctDI[1], 2);
    EXPECT_EQ(constexprBmctDI[2], 1);
    EXPECT_EQ(constexprBmctDI[3], 0);

    EXPECT_EQ(constexprBmctDI.RawData(), 0b00'01'10'11);

    std::ostringstream os;
    os << constexprBmctDI;
    EXPECT_EQ(os.str(), "BitmaskContainer(3, 2, 1, 0)");
}

TEST(Container_BitmaskContainer, Basic)
{
    Container::BitmaskContainer<8, 2> bmct{0b11'00'10'01};
    EXPECT_EQ(sizeof(bmct), sizeof(uint8_t));
    EXPECT_EQ(bmct.Capacity(), 4);

    EXPECT_EQ(bmct[0], 0b01);
    EXPECT_EQ(bmct[1], 0b10);
    EXPECT_EQ(bmct[2], 0b00);
    EXPECT_EQ(bmct[3], 0b11);

    bmct.Insert(0, 0b00);
    EXPECT_EQ(bmct.RawData(), 0b00'10'01'00);

    bmct.Remove(1);
    EXPECT_EQ(bmct.RawData(), 0b00'00'10'00);

    bmct.Set(3, 0b11);
    EXPECT_EQ(bmct.RawData(), 0b11'00'10'00);

    bmct.Insert(2, 0b01);
    EXPECT_EQ(bmct.RawData(), 0b00'01'10'00);
}

template <int32_t TotalBits, int32_t ElementBits>
void TestBitmaskContainer()
{
    std::random_device rd;
    std::mt19937 gen{rd()};

    constexpr int32_t rounds = 100'000;

    Container::BitmaskContainer<TotalBits, ElementBits> bmct{};
    std::vector<typename decltype(bmct)::ValueType> vec;

    std::uniform_int_distribution<typename decltype(bmct)::ValueType> elementDistribution{
        0, bmct.MaximumValue};

    for (int32_t i = 0; i < rounds; ++i) {
        const auto randVal = elementDistribution(gen);

        if (vec.size() == 0) {
            // can only insert
            bmct.Insert(0, randVal);
            vec.insert(std::begin(vec), randVal);
        } else {
            std::uniform_int_distribution<int32_t> moveDistribution{
                0, 1 + (static_cast<int32_t>(vec.size()) < bmct.Capacity())};
            std::uniform_int_distribution<int32_t> posDistribution(0, vec.size() - 1);
            const int32_t moveType = moveDistribution(gen);
            const int32_t pos = posDistribution(gen);

            switch (moveType) {
                case 0:  // Set
                    bmct.Set(pos, randVal);
                    vec[pos] = randVal;
                    break;
                case 1:  // Delete
                    bmct.Remove(pos);
                    vec.erase(std::begin(vec) + pos);
                    break;
                case 2:  // Insert
                    bmct.Insert(pos, randVal);
                    vec.insert(std::begin(vec) + pos, randVal);
                    break;
            }
        }

        EXPECT_EQ(bmct, vec);
    }
}

TEST(Container_BitmaskContainer, 8bit) { TestBitmaskContainer<8, 4>(); }

TEST(Container_BitmaskContainer, 16bit) { TestBitmaskContainer<16, 4>(); }

TEST(Container_BitmaskContainer, 32bit) { TestBitmaskContainer<32, 16>(); }

TEST(Container_BitmaskContainer, 64bit) { TestBitmaskContainer<64, 2>(); }

TEST(Container_BitmaskContainer, 128bit) { TestBitmaskContainer<128, 4>(); }
