#include <pbcopper/utility/FastMod.h>
#include <pbcopper/utility/Random.h>

#include <vector>

#include <gtest/gtest.h>

using namespace PacBio;
template <typename T>
void TestFM()
{
    std::uint64_t seed = 0;
    for (int i = 0; i < 1000000; ++i) {
        const T modV = Utility::WyHash64Step(seed);
        Utility::FastMod<T> fm(modV);
        for (int j = 0; j < 100; ++j) {
            const T rV = Utility::WyHash64Step(seed);
            EXPECT_EQ(rV % modV, fm.Modulus(rV));
            const auto [div, mod] = fm.DivMod(rV);
            EXPECT_EQ(fm.Modulus(rV), mod);
            EXPECT_EQ(div * modV + mod, rV);
        }
    }
}

TEST(Utility_FastMod, fm)
{
    TestFM<std::uint32_t>();
    TestFM<std::uint64_t>();
}
