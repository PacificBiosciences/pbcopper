#include <pbcopper/container/BitContainer.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <vector>

using namespace PacBio;

namespace PacBio {
namespace Container {

template <int32_t TotalBits, int32_t ElementBits>
bool operator==(const BitContainer<TotalBits, ElementBits>& lhs,
                const std::vector<typename BitContainer<TotalBits, ElementBits>::ValueType>& rhs)
{
    if (lhs.Size() != static_cast<int32_t>(rhs.size())) {
        return false;
    }

    for (size_t i = 0; i < rhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

}  // namespace Container
}  // namespace PacBio

TEST(Container_BitContainer, Constexpr)
{
    //                                                       T  G  C  A
    //                                                       3  2  1  0
    constexpr Container::BitContainer<32, 8> constexprBct{0x54'47'43'41U, 4};

    EXPECT_EQ(sizeof(constexprBct), 2 * sizeof(uint32_t));
    EXPECT_EQ(constexprBct.Capacity(), 4);
    EXPECT_EQ(constexprBct.Size(), 4);

    EXPECT_EQ(constexprBct[0], 'A');
    EXPECT_EQ(constexprBct[1], 'C');
    EXPECT_EQ(constexprBct[2], 'G');
    EXPECT_EQ(constexprBct[3], 'T');
}

TEST(Container_BitContainer, Equal)
{
    const Container::BitContainer<32, 8> bct1{0x00'47'43'41U, 3};
    const Container::BitContainer<32, 8> bct2{0x47'43'41U, 3};

    EXPECT_EQ(bct1, bct2);
}

TEST(Container_BitContainer, NonEqual1)
{
    const Container::BitContainer<32, 8> bct1{0x00'47'43'41U, 4};
    const Container::BitContainer<32, 8> bct2{0x00'47'43'41U, 3};

    EXPECT_NE(bct1, bct2);
}

TEST(Container_BitContainer, NonEqual2)
{
    const Container::BitContainer<32, 8> bct1{0x00'46'43'41U, 3};
    const Container::BitContainer<32, 8> bct2{0x00'47'43'41U, 3};

    EXPECT_NE(bct1, bct2);
}

TEST(Container_BitContainer, VariadicCtor)
{
    constexpr auto constexprBct = Container::BitContainer<32, 8>::MakeFromArray('G', 'A', 'T', 'C');

    EXPECT_EQ(sizeof(constexprBct), 2 * sizeof(uint32_t));
    EXPECT_EQ(constexprBct.Capacity(), 4);
    EXPECT_EQ(constexprBct.Size(), 4);

    EXPECT_EQ(constexprBct[0], 'G');
    EXPECT_EQ(constexprBct[1], 'A');
    EXPECT_EQ(constexprBct[2], 'T');
    EXPECT_EQ(constexprBct[3], 'C');
    EXPECT_EQ(constexprBct.RawData(), 0x43'54'41'47U);
}

TEST(Container_BitContainer, ConstexprDependencyInjection)
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

    constexpr auto constexprBctDI =
        Container::BitContainer<8, 2>::MakeFromTransform(converter, str);
    EXPECT_EQ(sizeof(constexprBctDI), 2 * sizeof(uint32_t));
    EXPECT_EQ(constexprBctDI.Capacity(), 4);
    EXPECT_EQ(constexprBctDI.Size(), 4);

    EXPECT_EQ(constexprBctDI[0], 3);
    EXPECT_EQ(constexprBctDI[1], 2);
    EXPECT_EQ(constexprBctDI[2], 1);
    EXPECT_EQ(constexprBctDI[3], 0);

    EXPECT_EQ(constexprBctDI.RawData(), 0b00'01'10'11);

    std::ostringstream os;
    os << constexprBctDI;
    EXPECT_EQ(os.str(), "BitContainer(Size=4, BitmaskContainer(3, 2, 1, 0)");
}

TEST(Container_BitContainer, Basic)
{
    Container::BitContainer<8, 2> bct{0b11'00'10'01, 4};
    EXPECT_EQ(sizeof(bct), 2 * sizeof(uint32_t));
    EXPECT_EQ(bct.Capacity(), 4);
    EXPECT_EQ(bct.Size(), 4);

    EXPECT_EQ(bct[0], 0b01);
    EXPECT_EQ(bct[1], 0b10);
    EXPECT_EQ(bct[2], 0b00);
    EXPECT_EQ(bct[3], 0b11);

    bct.Insert(0, 0b00);
    EXPECT_EQ(bct.Size(), 4);
    EXPECT_EQ(bct.RawData(), 0b00'10'01'00);

    bct.Remove(1);
    EXPECT_EQ(bct.Size(), 3);
    EXPECT_EQ(bct.RawData(), 0b00'10'00);

    bct.Set(2, 0b11);
    EXPECT_EQ(bct.Size(), 3);
    EXPECT_EQ(bct.RawData(), 0b11'10'00);

    bct.Insert(2, 0b01);
    EXPECT_EQ(bct.Size(), 4);
    EXPECT_EQ(bct.RawData(), 0b11'01'10'00);

    bct.Clear();
    EXPECT_EQ(bct.Size(), 0);
    EXPECT_EQ(bct.RawData(), 0);
}

TEST(Container_BitContainer, PushBack)
{
    Container::BitContainer<8, 2> bct{0b11, 1};
    EXPECT_EQ(sizeof(bct), 2 * sizeof(uint32_t));
    EXPECT_EQ(bct.Capacity(), 4);
    EXPECT_EQ(bct.Size(), 1);

    bct.PushBack(2);
    bct.PushBack(0);
    bct.PushBack(3);

    EXPECT_EQ(bct[0], 0b11);
    EXPECT_EQ(bct[1], 0b10);
    EXPECT_EQ(bct[2], 0b00);
    EXPECT_EQ(bct[3], 0b11);
    EXPECT_EQ(bct.Size(), 4);
}

TEST(Container_BitContainer, Range)
{
    using ContType = Container::BitContainer<32, 4>;

    const auto bct = ContType::MakeFromArray(1, 7, 9, 15, 3, 6, 5, 4);
    EXPECT_EQ(sizeof(bct), 2 * sizeof(uint32_t));
    EXPECT_EQ(bct.Capacity(), 8);
    EXPECT_EQ(bct.Size(), 8);

    // copy the whole range
    {
        const auto bctNew = bct.Range(0, 8);
        EXPECT_EQ(bctNew, bct);
    }

    // copy the whole range, but exceed length
    {
        const auto bctNew = bct.Range(0, 34837);
        EXPECT_EQ(bctNew, bct);
    }

    // copy nothing from the start
    {
        const auto bctNew = bct.Range(0, 0);
        EXPECT_EQ(bctNew, ContType{});
    }

    // copy nothing from the middle
    {
        const auto bctNew = bct.Range(4, 0);
        EXPECT_EQ(bctNew, ContType{});
    }

    // copy nothing from the end
    {
        const auto bctNew = bct.Range(8, 2);
        EXPECT_EQ(bctNew, ContType{});
    }

    // copy 1 base from the start
    {
        const auto bctNew = bct.Range(0, 1);
        EXPECT_EQ(bctNew, ContType::MakeFromArray(1));
    }

    // copy 2 bases from the start
    {
        const auto bctNew = bct.Range(0, 2);
        EXPECT_EQ(bctNew, ContType::MakeFromArray(1, 7));
    }

    // copy 1 base from the middle
    {
        const auto bctNew = bct.Range(4, 1);
        EXPECT_EQ(bctNew, ContType::MakeFromArray(3));
    }

    // copy 2 bases from the middle
    {
        const auto bctNew = bct.Range(4, 2);
        EXPECT_EQ(bctNew, ContType::MakeFromArray(3, 6));
    }

    // copy 1 base from the end
    {
        const auto bctNew = bct.Range(6, 1);
        EXPECT_EQ(bctNew, ContType::MakeFromArray(5));
    }

    // copy 2 bases from the end
    {
        const auto bctNew = bct.Range(6, 2);
        EXPECT_EQ(bctNew, ContType::MakeFromArray(5, 4));
    }

    // copy all bases from the 2nd base
    {
        const auto bctNew = bct.Range(1, 3457874);
        EXPECT_EQ(bctNew, ContType::MakeFromArray(7, 9, 15, 3, 6, 5, 4));
    }
}

template <int32_t TotalBits, int32_t ElementBits>
void TestBitContainer()
{
    std::random_device rd;
    std::mt19937 gen{rd()};

    constexpr int32_t rounds = 100'000;

    Container::BitContainer<TotalBits, ElementBits> bct{};
    std::vector<typename decltype(bct)::ValueType> vec;

    std::uniform_int_distribution<typename decltype(bct)::ValueType> elementDistribution{
        0, bct.MaximumValue};

    for (int32_t i = 0; i < rounds; ++i) {
        const auto randVal = elementDistribution(gen);

        if (vec.size() == 0) {
            // can only insert
            bct.Insert(0, randVal);
            vec.insert(std::begin(vec), randVal);
        } else {
            std::uniform_int_distribution<int32_t> moveDistribution{
                0, 1 + (static_cast<int32_t>(vec.size()) < bct.Capacity())};
            std::uniform_int_distribution<int32_t> posDistribution(0, vec.size() - 1);
            const int32_t moveType = moveDistribution(gen);
            const int32_t pos = posDistribution(gen);

            switch (moveType) {
                case 0:  // Set
                    bct.Set(pos, randVal);
                    vec[pos] = randVal;
                    break;
                case 1:  // Delete
                    bct.Remove(pos);
                    vec.erase(std::begin(vec) + pos);
                    break;
                case 2:  // Insert
                    bct.Insert(pos, randVal);
                    vec.insert(std::begin(vec) + pos, randVal);
                    break;
            }
        }

        EXPECT_EQ(bct, vec);
    }
}

TEST(Container_BitContainer, 8bit) { TestBitContainer<8, 1>(); }

TEST(Container_BitContainer, 16bit) { TestBitContainer<16, 4>(); }

TEST(Container_BitContainer, 32bit) { TestBitContainer<32, 8>(); }

TEST(Container_BitContainer, 64bit) { TestBitContainer<64, 32>(); }

TEST(Container_BitContainer, ReverseBasic)
{
    auto bct = Container::BitContainer<32, 8>::MakeFromArray(1, 2, 3);
    auto bctRev = Container::BitContainer<32, 8>::MakeFromArray(3, 2, 1);

    EXPECT_EQ(bct.Capacity(), 4);
    EXPECT_EQ(bct.Size(), 3);
    EXPECT_EQ(bctRev.Capacity(), 4);
    EXPECT_EQ(bctRev.Size(), 3);

    bct.Reverse();
    EXPECT_EQ(bct, bctRev);
}

TEST(Container_BitContainer, ReverseEmpty)
{
    Container::BitContainer<64, 8> bct{0, 0};

    EXPECT_EQ(bct.Capacity(), 8);
    EXPECT_EQ(bct.Size(), 0);

    bct.Reverse();
    EXPECT_EQ(bct.Size(), 0);
    EXPECT_EQ(bct.RawData(), 0U);
}

TEST(Container_BitContainer, ReverseOne)
{
    Container::BitContainer<32, 8> bct{255U, 1};

    EXPECT_EQ(bct.Capacity(), 4);
    EXPECT_EQ(bct.Size(), 1);

    bct.Reverse();
    EXPECT_EQ(bct.Size(), 1);
    EXPECT_EQ(bct.RawData(), 255U);
}

template <int32_t TotalBits, int32_t ElementBits>
void TestBitReverse()
{
    std::random_device rd;
    std::mt19937 gen{rd()};

    constexpr int32_t rounds = 100'000;

    using BitVec = typename Container::BitContainer<TotalBits, ElementBits>;
    constexpr int32_t capacity = BitVec::Capacity();

    std::uniform_int_distribution<typename BitVec::ValueType> elementDistribution{
        0, BitVec::MaximumValue};
    std::uniform_int_distribution<int32_t> lengthDistribution{1, capacity};

    for (int32_t i = 0; i < rounds; ++i) {
        const int32_t length = lengthDistribution(gen);

        BitVec fwdBct{0, length};
        std::vector<typename BitVec::ValueType> fwdVec(length);

        for (int32_t j = 0; j < length; ++j) {
            const auto val = elementDistribution(gen);

            fwdBct.Set(j, val);
            fwdVec[j] = val;
        }

        EXPECT_EQ(fwdBct, fwdVec);

        // reverse both now
        BitVec revBct = fwdBct;
        std::vector<typename BitVec::ValueType> revVec = fwdVec;

        revBct.Reverse();
        std::reverse(std::begin(revVec), std::end(revVec));

        EXPECT_EQ(revBct, revVec);
    }
}

TEST(Container_BitContainer, Reverse8_1bit) { TestBitReverse<8, 1>(); }
TEST(Container_BitContainer, Reverse8_2bit) { TestBitReverse<8, 2>(); }
TEST(Container_BitContainer, Reverse8_4bit) { TestBitReverse<8, 4>(); }

TEST(Container_BitContainer, Reverse16_1bit) { TestBitReverse<16, 1>(); }
TEST(Container_BitContainer, Reverse16_2bit) { TestBitReverse<16, 2>(); }
TEST(Container_BitContainer, Reverse16_4bit) { TestBitReverse<16, 4>(); }
TEST(Container_BitContainer, Reverse16_8bit) { TestBitReverse<16, 8>(); }

TEST(Container_BitContainer, Reverse32_1bit) { TestBitReverse<32, 1>(); }
TEST(Container_BitContainer, Reverse32_2bit) { TestBitReverse<32, 2>(); }
TEST(Container_BitContainer, Reverse32_4bit) { TestBitReverse<32, 4>(); }
TEST(Container_BitContainer, Reverse32_8bit) { TestBitReverse<32, 8>(); }
TEST(Container_BitContainer, Reverse32_16bit) { TestBitReverse<32, 16>(); }

TEST(Container_BitContainer, Reverse64_1bit) { TestBitReverse<64, 1>(); }
TEST(Container_BitContainer, Reverse64_2bit) { TestBitReverse<64, 2>(); }
TEST(Container_BitContainer, Reverse64_4bit) { TestBitReverse<64, 4>(); }
TEST(Container_BitContainer, Reverse64_8bit) { TestBitReverse<64, 8>(); }
TEST(Container_BitContainer, Reverse64_16bit) { TestBitReverse<64, 16>(); }
TEST(Container_BitContainer, Reverse64_32bit) { TestBitReverse<64, 32>(); }

TEST(Container_BitContainer, Reverse128_1bit) { TestBitReverse<128, 1>(); }
TEST(Container_BitContainer, Reverse128_2bit) { TestBitReverse<128, 2>(); }
TEST(Container_BitContainer, Reverse128_4bit) { TestBitReverse<128, 4>(); }
TEST(Container_BitContainer, Reverse128_8bit) { TestBitReverse<128, 8>(); }
TEST(Container_BitContainer, Reverse128_16bit) { TestBitReverse<128, 16>(); }
TEST(Container_BitContainer, Reverse128_32bit) { TestBitReverse<128, 32>(); }
TEST(Container_BitContainer, Reverse128_64bit) { TestBitReverse<128, 64>(); }
