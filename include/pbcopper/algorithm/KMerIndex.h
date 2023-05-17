#ifndef PBCOPPER_ALGORITHM_KMER_INDEX_H
#define PBCOPPER_ALGORITHM_KMER_INDEX_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/container/Unordered.h>
#include <pbcopper/parallel/FireAndForget.h>
#include <pbcopper/pbmer/DnaBit.h>
#include <pbcopper/utility/Ssize.h>

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <limits>
#include <map>
#include <mutex>
#include <numeric>
#include <optional>
#include <queue>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace PacBio {
namespace Algorithm {

struct BottomKKmerQ : public std::priority_queue<std::uint64_t>
{
private:
    using Super = std::priority_queue<std::uint64_t>;
    const int nitems_;

public:
    BottomKKmerQ(int nitems);
    void Update(std::uint64_t item) noexcept;
    std::vector<std::uint64_t>&& ToVec() noexcept;
    std::vector<std::uint64_t>&& ToSortedVec() noexcept;
    void Sort() noexcept;
};

struct SubMerSelection
{
    // Method for selecting a subset of k-mer elements to include
private:
    std::uint64_t pattern_;

    /// Returns the number of set positions in the k-mer
    ///
    ///
    int PopCount() const noexcept;

    // MakePattern makes the 64-bit bitmask for a given
    // bitpattern for subsets of 32-mers to save.
    static std::uint64_t MakePattern(std::uint64_t bitpattern) noexcept;

public:
    operator std::uint64_t() const noexcept;
    // Standard way to generate
    // This requires the k-mer length and the bitpattern.
    SubMerSelection(int k, std::uint64_t bitpattern);
    // Create a SubMerSelection from an existing bitpattern
    // This simply copies the 64-bit integer
    SubMerSelection(std::uint64_t x) noexcept;

    SubMerSelection(SubMerSelection&& o) noexcept = default;
    SubMerSelection(const SubMerSelection& o) noexcept = default;
    SubMerSelection& operator=(const SubMerSelection& o) noexcept = default;
    SubMerSelection& operator=(SubMerSelection&& o) noexcept = default;

    // First SelectSubseq is for bit-sampling LSH
    std::uint64_t SelectSubseq(std::uint64_t kmer) const noexcept;

    // Second SelectSubseq is for k-mer set LSH, which is translationally invariant
    std::uint64_t SelectSubseq(std::uint64_t kmer, int position) const noexcept;

    // The number of sliding windows contained in the full k-mer
    // as selected by the bitpattern pattern_;
    int NumberOfKernels(int k) const noexcept;
    // The width of the kernel
    // For contiguous seeding, this is simply k-mer length,
    // but for spaced seeds, this is the length of the full "comb" width.
    int KernelWidth() const noexcept;
    SubMerSelection& Normalize() noexcept;

    // Reversible Bijection Hash
    // c/o http://web.archive.org/web/20071223173210/http://www.concentric.net/~Ttwang/tech/inthash.htm
    // Also used in klib, cqf, and within PacBio's pbmer
    static constexpr std::uint64_t WangHash(std::uint64_t key) noexcept
    {
        key = (~key) + (key << 21);  // key = (key << 21) - key - 1;
        key = key ^ (key >> 24);
        key = (key + (key << 3)) + (key << 8);  // key * 265
        key = key ^ (key >> 14);
        key = (key + (key << 2)) + (key << 4);  // key * 21
        key = key ^ (key >> 28);
        key = key + (key << 31);
        return key;
    }
    // Select the subsequence corresponding to the k-mer
    // This function is only for the isSliding_ == false mode. (Hamming distance)
    // HashedSubseq is the same, but fed through WangHash.
    std::uint64_t Subseq(std::uint64_t kmer) const noexcept;
    std::uint64_t HashedSubseq(std::uint64_t kmer) const noexcept;

    // This function taking an int is for the k-mer LSH mode, in which
    // isSliding_ is true.
    std::uint64_t HashedSubseq(std::uint64_t kmer, int pos) const noexcept;
    bool operator<(const SubMerSelection o) const noexcept;
    bool operator==(const SubMerSelection o) const noexcept;
    std::string ToString() const;
};

static_assert(sizeof(SubMerSelection) == sizeof(std::uint64_t));

// Generate a vector of contiguous bit-strings spanning the super-Kmer
std::vector<SubMerSelection> GenerateContiguous(int k, int subK);
// Generate a vector of random (non-contiguous) bitstrings contained in the super-Kmer with population count equal to subK
// Set the number of sequences with numSequences and (optionally) set the seed.
// Defaults to 0 for the seed.
std::vector<SubMerSelection> GenerateRandomSubsequences(int k, int subK,
                                                        std::int64_t numSequences = 0,
                                                        std::uint64_t seed = 0);
// Generate the single flat sub k-mer for k and subK
SubMerSelection FlatSubMer(int k, int subK);
// Compute the number of spacings possible
long double MaxNumSpacings(int superK, int subK) noexcept;

template <typename IDType, typename KeyType = std::uint64_t>
class KMerLSHTable
{
    //
    // Build a KMerLSHTable
    // By default, builds a sliding window k-mer set LSH table
    // If sliding is set to false, this becomes a simpler LSH table
    // of bit-sampling, which corresponds to Hamming-distance LSH
    using HashMap = Container::UnorderedMap<KeyType, std::vector<IDType>>;
    std::vector<HashMap> maps_;
    std::vector<SubMerSelection> subMers_;
    const int kmerLength_;
    int bottomK_;
    // Set to > 0 to
    // only return the bottom-k hashes
    // from all tables
    const int isSliding_;
    std::int64_t id_ = 0;
    std::unique_ptr<std::mutex[]> mutexes_;

public:
    KMerLSHTable(const int k, std::vector<SubMerSelection> submers, const int bottomK = -1,
                 bool sliding = true, bool sortKernels = true)
        : subMers_(submers), kmerLength_(k), isSliding_(sliding)
    {
        if (isSliding_) {
            for (auto& kernel : subMers_) {
                kernel.Normalize();
            }
        }
        if (sortKernels) {
            std::sort(subMers_.begin(), subMers_.end());
            subMers_.erase(std::unique(subMers_.begin(), subMers_.end()), subMers_.end());
        }
        BottomK(bottomK);
        if (isSliding_) {
            for (SubMerSelection& x : subMers_) {
                x.Normalize();
            }
        }
    }

    KMerLSHTable(const int k, const int subK, const int bottomK = -1, bool sliding = true)
        : KMerLSHTable(k,
                       std::vector<SubMerSelection>({SubMerSelection(((1ull << (subK * 2)) - 1))}),
                       bottomK, sliding)
    {}

    KMerLSHTable(KMerLSHTable&& o)
        : maps_(std::move(o.maps_))
        , subMers_(std::move(o.subMers_))
        , kmerLength_(std::move(o.kmerLength_))
        , bottomK_(o.bottomK_)
        , isSliding_(o.isSliding_)
        , id_(o.id_)
        , mutexes_(std::make_unique<std::mutex[]>(Utility::Ssize(maps_)))
    {}

    std::int64_t Size() const noexcept { return id_; }

    std::int64_t MapSize() const noexcept
    {
        return std::accumulate(std::begin(maps_), std::end(maps_), std::int64_t(0),
                               [](std::int64_t x, const auto& map) { return x += map.size(); });
    }

    std::int64_t NextId() noexcept { return id_++; }

    std::int64_t NextIdThreadSafe() noexcept
    {
        return std::atomic_fetch_add(reinterpret_cast<std::atomic<std::int64_t>*>(&id_),
                                     std::int64_t(1));
    }

    std::int64_t TotalNumKernels() const
    {
        if (IsSliding()) {
            return std::accumulate(subMers_.begin(), subMers_.end(), std::int64_t(0),
                                   [kl = kmerLength_](std::int64_t x, const auto& mer) {
                                       return x + mer.NumberOfKernels(kl);
                                   });
        } else {
            return subMers_.size();
        }
    }

    bool IsSliding() const noexcept { return isSliding_; }

    int BottomK() const noexcept { return bottomK_; }

    int BottomK(int newBottomK) noexcept
    {
        const int newSize = newBottomK < 0 ? int(Utility::Ssize(subMers_)) : 1;
        maps_.resize(newSize);
        mutexes_ = std::make_unique<std::mutex[]>(newSize);
        return bottomK_ = newBottomK;
    }

    bool operator==(const KMerLSHTable& o) const noexcept
    {
        return std::tie(id_, kmerLength_, bottomK_, isSliding_, subMers_, maps_) ==
               std::tie(o.id_, o.kmerLength_, o.bottomK_, o.isSliding_, o.subMers_, o.maps_);
    }

    bool operator!=(const KMerLSHTable& o) const noexcept { return !(*this == o); }

    std::vector<std::uint64_t> generatePooledBottomK(const std::uint64_t mer) const
    {
        std::vector<std::uint64_t> ret;
        if (const std::int64_t numKernels = TotalNumKernels(); numKernels <= 1024) {
            ret.reserve(numKernels);
            if (isSliding_) {
                for (const SubMerSelection& k : subMers_) {
                    const int nk = k.NumberOfKernels(kmerLength_);
                    for (int i = 0; i < nk; ++i) {
                        ret.emplace_back(k.HashedSubseq(mer, i));
                    }
                }
            } else {
                std::transform(subMers_.begin(), subMers_.end(), std::back_inserter(ret),
                               [mer](const auto& x) { return x.HashedSubseq(mer); });
            }
            const std::ptrdiff_t actualB =
                std::min(std::ptrdiff_t(BottomK()), std::ptrdiff_t(ret.size()));
            if (const auto pSortEnd = ret.begin() + actualB; pSortEnd != ret.end()) {
                std::partial_sort(ret.begin(), pSortEnd, ret.end());
                ret.erase(pSortEnd, ret.end());
            } else {
                std::sort(ret.begin(), ret.end());
            }
        } else {
            BottomKKmerQ q(bottomK_);
            if (isSliding_) {
                for (const SubMerSelection& k : subMers_) {
                    const int nk = k.NumberOfKernels(kmerLength_);
                    for (int i = 0; i < nk; ++i) {
                        q.Update(k.HashedSubseq(mer, i));
                    }
                }
            } else {
                for (const SubMerSelection& x : subMers_) {
                    q.Update(x.HashedSubseq(mer));
                }
            }
            ret = std::move(q.ToSortedVec());
        }
        return ret;
    }

    void Insert(const Pbmer::DnaBit cb, const std::int64_t given_id) { Insert(cb.mer, given_id); }

    void Insert(const Pbmer::DnaBit cb) { Insert(cb.mer, NextId()); }

    template <typename It>
    void Insert(It beg, It end)
    {
        if (end < beg) {
            throw std::invalid_argument(std::string("[pbcopper] kmer index insert ERROR: "
                                                    "end < beg. std::distance(beg, end) = ") +
                                        std::to_string(std::distance(beg, end)));
        }
        while (beg != end) {
            Insert(*beg, id_);
            ++beg;
            ++id_;
        }
    }

    bool IsSorted() const
    {
        return std::all_of(maps_.begin(), maps_.end(), [&](const auto& map) {
            return std::all_of(map.begin(), map.end(), [](const auto& mapPair) {
                return std::is_sorted(mapPair.second.begin(), mapPair.second.end());
            });
        });
    }

    void Sort(int numThreads = 1)
    {
        if (numThreads <= 1) {
            for (auto& map : maps_) {
                for (auto& pair : map) {
                    std::sort(pair.second.begin(), pair.second.end());
                }
            }
        } else {
            Parallel::FireAndForget faf(numThreads);
            for (auto& map : maps_) {
                if (maps_.size() > 1) {
                    faf.ProduceWith([&]() {
                        for (auto& pair : map) {
                            std::sort(pair.second.begin(), pair.second.end());
                        }
                    });
                } else {
                    for (auto& pair : map) {
                        faf.ProduceWith(
                            [&]() { std::sort(pair.second.begin(), pair.second.end()); });
                    }
                }
            }
            faf.Finalize();
        }
    }

    template <typename It>
    void InsertParallel(const It beg, const It end, int numThreads = -1)
    {
        if (end < beg) {
            throw std::invalid_argument(std::string("[pbcopper] kmer index parallel insert ERROR: "
                                                    "end < beg. std::distance(beg, end) = ") +
                                        std::to_string(std::distance(beg, end)));
        }
        if (beg == end) {
            return;
        }
        if (numThreads <= 0) {
            numThreads = std::thread::hardware_concurrency();
        }
        const std::ptrdiff_t n = std::distance(beg, end);
        const std::ptrdiff_t numPerThread = (n + numThreads - 1) / numThreads;
        if ((numPerThread == 1) && (n < numThreads)) {
            numThreads = n;
        }
        const std::int64_t oldID = id_;

        std::vector<std::thread> threads;
        threads.reserve(numThreads);
        for (std::ptrdiff_t i = 0; i < numThreads; ++i) {
            const std::ptrdiff_t startIndex = i * numPerThread;
            const std::ptrdiff_t endIndex = std::min(startIndex + numPerThread, n);
            if (startIndex >= endIndex) {
                break;
            }
            threads.emplace_back(
                [&, this, oldID](const std::ptrdiff_t start, const std::ptrdiff_t stop) {
                    auto it = beg;
                    std::advance(it, start);
                    // This can be slow if you provide non random-access iterators
                    // (e.g., dictionaries).
                    // If this is too slow, fall back to serial insertion
                    for (int id = oldID + start, endID = oldID + stop; id != endID; ++id) {
                        InsertThreadSafe(*it, id);
                        ++it;
                    }
                },
                startIndex, endIndex);
        }
        for (auto& thread : threads) {
            thread.join();
        }
        id_ = oldID + n;
    }

    // Container overload
    template <typename T, typename = std::enable_if_t<!std::is_integral_v<T> &&
                                                      !std::is_same_v<T, Pbmer::DnaBit>>>
    void Insert(const T& x)
    {
        Insert(x.begin(), x.end());
    }
    // K-mer overload
    void Insert(const std::uint64_t mer) { Insert(mer, NextId()); }

    // Actual insert code
    void InsertThreadSafe(const std::uint64_t mer, const std::int64_t myID)
    {
        if (bottomK_ > 0) {
            assert(Utility::Ssize(maps_) == 1);
            const std::vector<std::uint64_t> sampleSet = generatePooledBottomK(mer);
            std::lock_guard<std::mutex> lock(mutexes_[0]);
            for (const std::uint64_t v : sampleSet) {
                maps_.front()[v].push_back(myID);
            }
        } else {
            allMapsInsert<true>(mer, myID);
        }
    }

    void InsertThreadSafe(const std::uint64_t mer) { InsertThreadSafe(mer, NextIdThreadSafe()); }

    void Insert(const std::uint64_t mer, const std::int64_t myID)
    {
        if (bottomK_ > 0) {
            if (Utility::Ssize(maps_) != 1u) {
                throw std::runtime_error(
                    "[pbcopper] lsh index bottom-k insert ERROR: maps_ must be of size 1 for "
                    "bottom-k indexing.");
            }
            for (const std::uint64_t v : generatePooledBottomK(mer)) {
                maps_.front()[v].push_back(myID);
            }
        } else {
            allMapsInsert<false>(mer, myID);
        }
    }

    // Return a vector of tuples for matches
    // These consist of <ID, numberOfMatches>
    std::vector<std::pair<IDType, int>> Query(const Pbmer::DnaBit mer,
                                              std::int64_t stopThreshold = -1) const
    {
        return Query(mer.mer, stopThreshold);
    }

    // Return a vector of tuples for matches
    // These consist of <ID, numberOfMatches>
    // std::uint64_t overload
    std::vector<std::pair<IDType, int>> Query(const std::uint64_t mer,
                                              std::int64_t stopThreshold = -1) const
    {
        std::vector<std::pair<IDType, int>> ret;
        Query(ret, mer, stopThreshold);
        return ret;
    }

    std::map<IDType, int> MapQuery(const Pbmer::DnaBit mer,
                                   const std::int64_t stopThreshold = -1) const
    {
        return MapQuery(mer.mer, stopThreshold);
    }

    std::map<IDType, int> MapQuery(const std::uint64_t mer,
                                   const std::int64_t stopThreshold = -1) const
    {
        std::map<IDType, int> ret;
        MapQuery(ret, mer, stopThreshold);
        return ret;
    }

    // This is like MapQuery, but you provide the container.
    // As long as it's an associative container, it should work.
    template <typename MapType>
    void MapQuery(MapType& hits, const std::uint64_t mer, std::int64_t stopThreshold = -1) const
    {
        const int thresholdIncrement = stopThreshold < 0 ? bottomK_ * 2 : stopThreshold;
        std::ptrdiff_t hitsSize = Utility::Ssize(hits);
        const std::ptrdiff_t threshold = hitsSize + thresholdIncrement;
        if (bottomK_ > 0) {
            if (Utility::Ssize(maps_) != 1u) {
                throw std::runtime_error(
                    "[pbcopper] lsh index mapquery ERROR: maps_ must be of size 1 for bottom-k "
                    "indexing.");
            }
            const auto& singleMap = maps_.front();
            assert(Utility::Ssize(maps_) == 1);
            for (const std::uint64_t v : generatePooledBottomK(mer)) {
                if (auto it = singleMap.find(v); it != singleMap.end()) {
                    for (const IDType id : it->second) {
                        ++hits[id];
                    }
                }
                if (Utility::Ssize(hits) >= threshold) {
                    break;
                }
            }
        } else {
            const std::int64_t e = Utility::Ssize(maps_);
            for (std::int64_t i = 0; i < e; ++i) {
                singleMapQuery(maps_[i], hits, subMers_[i], mer);
                if ((stopThreshold > 0) && (Utility::Ssize(hits) >= threshold)) {
                    break;
                }
            }
        }
    }

private:
    template <typename Map, typename ReturnMap>
    void singleMapQuery(const Map& indexMap, ReturnMap& retMap, const SubMerSelection sel,
                        const std::uint64_t mer) const
    {
        auto update = [&](std::uint64_t hash) {
            if (auto it = indexMap.find(hash); it != indexMap.end()) {
                for (const auto id : it->second) {
                    ++retMap[id];
                }
            }
        };
        if (isSliding_) {
            const int ke = sel.NumberOfKernels(kmerLength_);
            for (int ki = 0; ki < ke; ++ki) {
                update(sel.HashedSubseq(mer, ki));
            }
        } else {
            update(sel.HashedSubseq(mer));
        }
    }

    template <bool ThreadSafe>
    void singleMapInsert(const int index, const std::uint64_t mer, const std::int64_t id)
    {
        // Get max/submer pattern
        auto& map(maps_[index]);
        const SubMerSelection sel = subMers_[index];

        // and, if ThreadSafe, the lock
        using OptLock = std::optional<std::lock_guard<std::mutex>>;
        const OptLock lock(ThreadSafe ? OptLock(mutexes_[index]) : OptLock());

        auto insert = [&map, id](std::uint64_t hash) { map[hash].emplace_back(id); };
        if (isSliding_) {
            const int ke = sel.NumberOfKernels(kmerLength_);
            for (int ki = 0; ki < ke; ++ki) {
                insert(sel.HashedSubseq(mer, ki));
            }
        } else {
            insert(sel.HashedSubseq(mer));
        }
    }
    template <bool ThreadSafe>
    void allMapsInsert(const std::uint64_t mer, const std::int64_t id)
    {
        const int nMaps = Utility::Ssize(maps_);
        for (int i = 0; i < nMaps; ++i) {
            singleMapInsert<ThreadSafe>(i, mer, id);
        }
    }

public:
    void Query(std::vector<std::pair<IDType, int>>& ret, const std::uint64_t mer,
               const std::int64_t stopThreshold = -1) const
    {
        std::map<IDType, int> hits;
        MapQuery(hits, mer, stopThreshold);
        ret.resize(hits.size());
        std::copy(hits.begin(), hits.end(), ret.begin());
        std::sort(ret.begin(), ret.end(), [](std::pair<IDType, int> x, std::pair<IDType, int> y) {
            return std::tie(x.second, x.first) > std::tie(y.second, y.first);
        });
    }

    void Query(std::vector<std::pair<IDType, int>>& ret, const Pbmer::DnaBit mer,
               std::int64_t stopThreshold = -1) const
    {
        Query(ret, mer.mer, stopThreshold);
    }
};

// 32-bit IDs
using KMerLSH32 = KMerLSHTable<std::uint32_t, std::uint32_t>;
using KMerLSH32_32 = KMerLSH32;

using KMerLSH32_64 = KMerLSHTable<std::uint32_t, std::uint64_t>;

// 64-bit IDs
using KMerLSH64 = KMerLSHTable<std::uint64_t, std::uint64_t>;
using KMerLSH64_64 = KMerLSH64;

using KMerLSH64_32 = KMerLSHTable<std::uint64_t, std::uint32_t>;
using KMerIndex = KMerLSH64_64;
// Default to 64-bit IDs and 64-bit hashes
// Users can opt-in to more compact tables by using 32/32
// but this fails if > 4.3 billion items are inserted.

}  // namespace Algorithm
}  // namespace PacBio

#endif /* PBCOPPER_ALGORITHM_KMER_INDEX_H */
