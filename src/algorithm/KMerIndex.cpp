#include <pbcopper/algorithm/KMerIndex.h>

#include <pbcopper/container/Unordered.h>
#include <pbcopper/utility/Intrinsics.h>
#include <pbcopper/utility/Random.h>

#include <iomanip>
#include <numeric>
#include <random>

#include <cinttypes>
#include <climits>

namespace PacBio {

namespace Algorithm {

std::vector<SubMerSelection> GenerateContiguous(const int k, const int subK)
{
    if (subK > k) {
        throw std::invalid_argument(
            "[pbcopper] generate contiguous submers ERROR: subK must be <= k");
    }
    const std::uint64_t completePattern = (1ull << k) - 1;
    const int diff = k - subK;
    const std::uint64_t subPattern = completePattern >> diff;
    std::vector<SubMerSelection> ret;
    ret.reserve(diff + 1);
    for (int i = 0; i < diff + 1; ++i) {
        ret.emplace_back(k, subPattern << i);
    }
    return ret;
}

std::vector<SubMerSelection> GenerateRandomSubsequences(const int k, const int subK,
                                                        std::int64_t numSequences,
                                                        std::uint64_t seed)
{
    seed = std::mt19937_64(seed)();
    if (numSequences > MaxNumSpacings(k, subK)) {
        throw std::invalid_argument(
            std::string(
                "[pbcopper] kmer index ERROR: Cannot generate random subsequences with k = ") +
            std::to_string(k) + " and subK = " + std::to_string(subK) + " and make " +
            std::to_string(numSequences));
    }
    if (numSequences == 0) {
        numSequences = std::max(std::int64_t((1LL << (k - subK)) / 20 * 5), std::int64_t(1));
    }
    Container::UnorderedSet<std::uint64_t> keys;
    const std::uint64_t randomValueMask = (std::uint64_t(1) << k) - 1;
    for (;;) {
        //This RNG is not as fast as possible, uses rejection sampling.
        //And we randomly generate bit-strings until we have
        std::uint64_t randomValue = Utility::WyHash64Step(seed);
        std::uint64_t rmask = randomValue & randomValueMask;
        auto popCount = Utility::PopCount(rmask);
        if (popCount != subK) {
            // if the number of 1s is wrong, flip. Otherwise we'll have to generate more numbers.
            rmask = ~randomValue & randomValueMask;
            popCount = Utility::PopCount(rmask);
        }
        if (popCount == subK && keys.find(rmask) == keys.end()) {
            keys.emplace(rmask);
            if (Utility::Ssize(keys) >= numSequences) {
                break;
            }
        }
    }
    std::vector<SubMerSelection> ret;
    std::transform(keys.begin(), keys.end(), std::back_inserter(ret),
                   [k](const std::uint64_t x) { return SubMerSelection(k, x); });
    return ret;
}

SubMerSelection FlatSubMer(const int k, const int subK)
{
    if (subK > k) {
        throw std::invalid_argument(
            std::string("[pbcopper] kmer index ERROR: FlatSubMer expects subK to be >= k. k = ") +
            std::to_string(k) + ", subk = " + std::to_string(subK));
    }
    return {k, (std::uint64_t(1) << subK) - 1};
}

uint64_t SubMerSelection::SelectSubseq(std::uint64_t kmer) const noexcept
{
    return kmer & pattern_;
}

uint64_t SubMerSelection::SelectSubseq(std::uint64_t kmer, int position) const noexcept
{
    const int bitshift = position * 2;
    return (kmer & (pattern_ << bitshift)) >> bitshift;
}

int SubMerSelection::NumberOfKernels(int k) const noexcept { return k - KernelWidth() + 1; }

int SubMerSelection::KernelWidth() const noexcept
{
    const int lastSetPos = (sizeof(pattern_) * CHAR_BIT - Utility::CountLeadingZeros(pattern_));
    const int firstSetPos = Utility::CountTrailingZeros(pattern_);
    return (lastSetPos - firstSetPos) / 2;
}

SubMerSelection& SubMerSelection::Normalize() noexcept
{
    pattern_ >>= Utility::CountTrailingZeros(pattern_);
    return *this;
}

uint64_t SubMerSelection::HashedSubseq(std::uint64_t kmer) const noexcept
{
    return WangHash(SelectSubseq(kmer));
}

uint64_t SubMerSelection::Subseq(std::uint64_t kmer) const noexcept { return SelectSubseq(kmer); }

uint64_t SubMerSelection::HashedSubseq(std::uint64_t kmer, int pos) const noexcept
{
    return WangHash(SelectSubseq(kmer, pos));
}

std::string SubMerSelection::ToString() const
{
    constexpr std::size_t BUFFER_SIZE = 128;

    std::string ret(BUFFER_SIZE, '\0');
    const int rc = std::snprintf(ret.data(), BUFFER_SIZE, "%" PRIu64 "\n", pattern_);
    ret.resize(rc);
    for (std::uint64_t cv = pattern_; cv;) {
        const int ind = Utility::CountTrailingZeros(cv);
        int kmerIndex = ind >> 1;
        ret += "Pos";
        ret += std::to_string(kmerIndex);
        ret += ';';
        cv ^= (3ull << ind);
    }
    return ret;
}

uint64_t SubMerSelection::MakePattern(std::uint64_t bitPattern) noexcept
{
    std::uint64_t ret = 0;
    for (int i = 0; i < 32; ++i) {
        if ((bitPattern >> i) & 1) {
            ret |= 0x3ull << (i * 2);
        }
    }
    return ret;
}

int SubMerSelection::PopCount() const noexcept { return Utility::PopCount(pattern_) >> 1; }

SubMerSelection::SubMerSelection(int k, std::uint64_t bitPattern)
    : pattern_(MakePattern(bitPattern))
{
    if (k > 32) {
        throw std::invalid_argument(
            std::string("[pbcopper] submer selection ERROR: k must be <= 32;") + std::to_string(k) +
            " is invalid.");
    }
    if (const int popCount = PopCount(); popCount > k || popCount <= 0) {
        throw std::invalid_argument(
            std::string("[pbcopper] kmer index ERROR: Invalid bitPattern: PopCount of ") +
            std::to_string(popCount) + " out of range");
    }
}

SubMerSelection::operator std::uint64_t() const noexcept { return this->pattern_; }

SubMerSelection::SubMerSelection(std::uint64_t x) noexcept : pattern_(x) {}

bool SubMerSelection::operator<(const SubMerSelection o) const noexcept
{
    // Higher-weight, longer-kernel, and lower bit value, in that order,
    // define lexicographic order
    // We then iterate in order from most specific to least specific
    // This way, k-mers occurring earlier in the string as considered less
    const int myPopCount = PopCount();
    const int otherPopCount = o.PopCount();
    const int myKernelWidth = KernelWidth();
    const int otherKernelWidth = KernelWidth();
    const std::uint64_t myInvertedN = std::uint64_t(-1) - *this;
    const std::uint64_t otherInvertedN = std::uint64_t(-1) - o;
    return std::tie(myPopCount, myKernelWidth, myInvertedN) >
           std::tie(otherPopCount, otherKernelWidth, otherInvertedN);
}

bool SubMerSelection::operator==(const SubMerSelection o) const noexcept
{
    return pattern_ == o.pattern_;
}

void BottomKKmerQ::Update(std::uint64_t item) noexcept
{
    if (const std::ptrdiff_t osize = Utility::Ssize(*this); osize < nitems_) {
        return Super::push(item);
    }
    if (item < Super::top()) {
        Super::pop();
        Super::push(item);
    }
}

std::vector<std::uint64_t>&& BottomKKmerQ::ToVec() noexcept { return std::move(this->c); }

void BottomKKmerQ::Sort() noexcept { std::sort(this->c.begin(), this->c.end()); }

std::vector<std::uint64_t>&& BottomKKmerQ::ToSortedVec() noexcept
{
    Sort();
    return ToVec();
}

BottomKKmerQ::BottomKKmerQ(int nitems) : nitems_(nitems) { this->c.reserve(nitems); }

long double MaxNumSpacings(int superK, int subK) noexcept
{
    long double frac = 1L;
    for (int i = subK; i < superK; ++i) {
        frac *= i;
    }
    for (int i = 1; i < (superK - subK); ++i) {
        frac /= i;
    }
    return std::max(std::floor(frac), 1.L);
}

}  // namespace Algorithm
}  // namespace PacBio
