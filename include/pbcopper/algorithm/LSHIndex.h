#ifndef PBCOPPER_ALGORITHM_LSHINDEX_H
#define PBCOPPER_ALGORITHM_LSHINDEX_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/container/Unordered.h>
#include <pbcopper/utility/Deleters.h>
#include <pbcopper/utility/FastMod.h>
#include <pbcopper/utility/Intrinsics.h>
#include <pbcopper/utility/Random.h>

#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <thread>
#include <variant>
#include <vector>

namespace PacBio {
namespace Algorithm {

/*
 * LSHIndex
 *
 * This class maintains a locality-sensitive hashing (LSH) index over a set of sketches
 *
 * Sketching must be done elsewhere;
 * For set (MinHash) sketching, one must either:
 * 1. Use bottom-k sketching with a single hash map,
 * 2. Use K-Mins/K-Partition MinHash with one hash map per hash function
 * The latter is stronger but more complex, and bottom-k is easier.
 *
 * The bottom-k index is also slower to update in highly multithreaded applications.
 *
 * You construct with parameters relating to sketch size,
 * then you insert individual items or ranges of them,
 * and then you query with new hashes, which you can use for filtration of potential
 * near-neighbors.
 */
template <typename KeyT = uint64_t, typename IdT = uint32_t>
class LSHIndex
{
private:
    int64_t sketchSize_;
    using HashV = std::vector<Container::UnorderedMap<KeyT, std::vector<IdT>>>;
    std::vector<HashV> packedMaps_;
    std::vector<int64_t> registersPerTable_;
    int64_t totalIDs_;
    std::vector<std::vector<std::mutex>> mutexes_;
    bool isBottomKOnly_{false};

    static constexpr inline uint64_t wangHash(uint64_t key) noexcept
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

    static void BufferedFWrite(std::FILE* fp, const void* ptr, int64_t nb)
    {
        if (const int64_t nWritten = std::fwrite(ptr, 1, nb, fp); nWritten != nb) {
            throw std::runtime_error(
                std::string(
                    "[pbcopper] std::fwrite ERROR: Failed to BufferedFWrite from file; read ") +
                std::to_string(nWritten) + " bytes instead of " + std::to_string(nb) +
                ". Is disk full?");
        }
    }

    static void BufferedFRead(std::FILE* fp, void* ptr, int64_t nb)
    {
        if (const int64_t nRead = std::fread(ptr, 1, nb, fp); nRead != nb) {
            throw std::runtime_error(
                std::string(
                    "[pbcopper] std::fread ERROR: Failed to BufferedFRead from file; read ") +
                std::to_string(nRead) + " bytes instead of " + std::to_string(nb));
        }
    }

public:
    using key_type = KeyT;
    using id_type = IdT;

    int64_t M() const noexcept { return sketchSize_; }

    int64_t Size() const noexcept { return totalIDs_; }

    int64_t Size(int64_t totalIDs) noexcept { return totalIDs_ = totalIDs; }

    int64_t NTables() const noexcept { return Utility::Ssize(packedMaps_); }

    bool IsBottomK() const noexcept { return isBottomKOnly_; }

    // Most general constructor
    // Create an index with groupings dictated by numberHashesPerSignatures
    // and grouping sampling counts from numberSignaturesPerRows
    // For more advanced usage.
    template <typename IT, typename Alloc, typename OIT, typename OAlloc>
    LSHIndex(int64_t m, const std::vector<IT, Alloc>& numberHashesPerSignatures,
             const std::vector<OIT, OAlloc>& numberSignaturesPerRows)
        : sketchSize_(m)
    {
        if (Utility::Ssize(numberHashesPerSignatures) != Utility::Ssize(numberSignaturesPerRows)) {
            throw std::invalid_argument(
                "[pbcopper] lsh index ERROR: LSHIndex constructor requires numberSignaturesPerRows "
                "and numberHashesPerSignatures to have the same size");
        }
        const std::ptrdiff_t e = Utility::Ssize(numberHashesPerSignatures);
        for (std::ptrdiff_t i = 0; i < e; ++i) {
            const IT v = numberHashesPerSignatures[i];
            registersPerTable_.push_back(v);
            if (v <= 0) {
                throw std::invalid_argument(
                    "[pbcopper] lsh index construction. ERROR: Cannot use <= 0 registers per "
                    "signature.");
            }
            const OIT v2 = numberSignaturesPerRows[i] > 0 ? OIT(numberSignaturesPerRows[i])
                                                          : OIT(sketchSize_ / v);
            packedMaps_.emplace_back(v2);
            mutexes_.emplace_back(v2);
        }
        totalIDs_ = 0;
    }

    void Unlock() noexcept { mutexes_.clear(); }

    // Second-most general constructor
    // Create an index with sketchsize = M
    // which tables of register groupings of sizes
    // dictated by numberHashesPerSignatures
    // The number of signatures per row defaults to m / size
    template <typename IT, typename Alloc>
    LSHIndex(int64_t m, const std::vector<IT, Alloc>& numberHashesPerSignatures) : sketchSize_(m)
    {
        totalIDs_ = 0;
        for (const auto v : numberHashesPerSignatures) {
            registersPerTable_.push_back(v);
            packedMaps_.emplace_back(HashV(sketchSize_ / v));
            mutexes_.emplace_back(std::vector<std::mutex>(sketchSize_ / v));
        }
    }

    // Create index with  sketchsize = M
    // and floor(log2(m)) register groupings
    // If densified = true, this uses ranges of integers (1, 2, 3, 4...)
    // instead of the default powers of 2 (1, 2, 4, 8, 16...)
    LSHIndex(int64_t m, bool densified = false) : sketchSize_(m)
    {
        totalIDs_ = 0;
        const int64_t numRegisterSizes =
            densified ? m : static_cast<int64_t>(Utility::IntegralLog2(Utility::BitCeiling(m)));
        registersPerTable_.reserve(numRegisterSizes);
        packedMaps_.reserve(numRegisterSizes);
        for (int64_t registersPerHash = 1; registersPerHash <= sketchSize_;) {
            registersPerTable_.push_back(registersPerHash);
            packedMaps_.emplace_back(HashV(sketchSize_ / registersPerHash));
            mutexes_.emplace_back(HashV(sketchSize_ / registersPerHash));
            if (densified) {
                ++registersPerHash;
            } else {
                registersPerHash <<= 1;
            }
        }
    }

    // Builds a bottom-K LSH index
    LSHIndex() : LSHIndex(1, std::vector<IdT>{1})
    {
        packedMaps_.resize(1);
        packedMaps_.front().resize(1);
        mutexes_.emplace_back(1);
        registersPerTable_ = {1};
        isBottomKOnly_ = true;
    }

    LSHIndex(const LSHIndex& o) { *this = o; }

    LSHIndex(LSHIndex&& o) = default;

    static LSHIndex CreateBottomK(int64_t k)
    {
        LSHIndex ret;
        ret.sketchSize_ = k;
        return ret;
    }

    LSHIndex& operator=(const LSHIndex& o)
    {
        totalIDs_ = o.totalIDs_;
        registersPerTable_ = o.registersPerTable_;
        packedMaps_ = o.packedMaps_;
        mutexes_.resize(Utility::Ssize(o.mutexes_));
        std::transform(cbegin(o.mutexes_), cend(o.mutexes_), begin(mutexes_),
                       [](const auto& x) { return std::vector<std::mutex>(Utility::Ssize(x)); });
        isBottomKOnly_ = o.isBottomKOnly_;
        return *this;
    }

    LSHIndex& operator=(LSHIndex&& o) = default;

    // Creates an empty LSHIndex
    // of the same dimensions/sizes.
    static LSHIndex CloneLike(const LSHIndex& o)
    {
        if (o.isBottomKOnly_) {
            return LSHIndex();
        }
        LSHIndex res;
        res.packedMaps_.resize(Utility::Ssize(o.packedMaps_));
        res.registersPerTable_ = o.registersPerTable_;
        for (int64_t i = 0; i < Utility::Ssize(o.packedMaps_); ++i) {
            res.packedMaps_[i].resize(o.packedMaps_[i].size());
        }
        res.mutexes_.clear();
        for (auto& mut : o.mutexes_) {
            res.mutexes_.emplace_back(Utility::Ssize(mut));
        }
        res.isBottomKOnly_ = o.isBottomKOnly__;
        return res;
    }
    LSHIndex Clone() const { return CloneLike(*this); }

    // Read a serialized LSHIndex
    LSHIndex(std::FILE* fp)
    {
        int64_t numberMapSets;
        std::vector<int64_t> mapSizes;
        registersPerTable_.clear();

        BufferedFRead(fp, &totalIDs_, sizeof(totalIDs_));
        BufferedFRead(fp, &numberMapSets, sizeof(int64_t));
        mapSizes.reserve(numberMapSets);
        packedMaps_.resize(mapSizes.size());
        while (mapSizes.size() < numberMapSets) {
            int64_t v;
            BufferedFRead(fp, &v, sizeof(int64_t));
            mapSizes.push_back(v);
            packedMaps_.emplace_back(HashV(v));
        }
        while (Utility::Ssize(registersPerTable_) < numberMapSets) {
            int64_t v;
            BufferedFRead(fp, &v, sizeof(int64_t));
            registersPerTable_.push_back(v);
        }
        uint8_t isBottomK;
        uint8_t islocked;
        BufferedFRead(fp, &isBottomK, 1);
        BufferedFRead(fp, &islocked, 1);
        isBottomKOnly_ = isBottomK;
        const int64_t packedMapSize = Utility::Ssize(packedMaps_);
        for (int64_t i = 0; i < packedMapSize; ++i) {
            auto& mapVec = packedMaps_[i];
            const int64_t je = Utility::Ssize(packedMaps_[i]);
            for (int64_t j = 0; j < je; ++j) {
                int64_t sz;
                BufferedFRead(fp, &sz, sizeof(sz));
                for (int64_t k = 0; k < sz; ++k) {
                    int64_t psz;
                    KeyT key;
                    BufferedFRead(fp, &psz, sizeof(psz));
                    BufferedFRead(fp, &key, sizeof(key));
                    std::vector<KeyT> vals(psz);
                    BufferedFRead(fp, vals.data(), sizeof(KeyT) * vals.size());
                    mapVec[j].emplace(key, std::move(vals));
                }
            }
        }
        if (islocked) {
            mutexes_.resize(mapSizes.size());
            std::transform(begin(mapSizes), end(mapSizes), mutexes_.begin(),
                           [](auto x) { return std::vector<std::mutex>(x); });
        }
    }

    bool operator==(const LSHIndex& o) const noexcept
    {
        return (totalIDs_ == o.totalIDs_) &&
               (Utility::Ssize(registersPerTable_) == Utility::Ssize(o.registersPerTable_)) &&
               (Utility::Ssize(packedMaps_) == Utility::Ssize(o.packedMaps_)) &&
               (std::equal(packedMaps_.cbegin(), packedMaps_.cend(), o.packedMaps_.cbegin()));
    }

    // This function inserts the item "item" into the sketch,
    // while querying for neighbors, returning up to maxCandidates candidates
    // Since we are updating while querying, we perform all updates.
    // (IE, there is no early stopping.)
    // It returns:
    //    std::vector<IdT> - matching IDs
    //    std::vector<int32_t> - matching counts
    //    std::vector<int32_t> - number of matches per rows
    // The first two are sufficient to order the matches by counts
    // and the last is used for identifying which matching IDs were found
    // in which row, which tells us how strong the matches are.
    //
    // For simply querying, use Query.
    // For simply inserting, use Insert.
    template <typename Sketch>
    std::tuple<std::vector<IdT>, std::vector<int32_t>, std::vector<int32_t>> UpdateQuery(
        const Sketch& item, int64_t maxCandidates)
    {
        // Check exceptions
        if (!isBottomKOnly_ && Utility::Ssize(item) < sketchSize_) {
            throw std::invalid_argument(
                std::string("[pbcopper] lsh index ERROR: Item has wrong size: ") +
                std::to_string(Utility::Ssize(item)) + ", expected" + std::to_string(sketchSize_));
        }

        const int64_t myID = NextID();

        // Initialize return structures
        Container::UnorderedMap<IdT, int32_t> returnSet;
        std::vector<IdT> passingIDs;
        std::vector<int32_t> itemsPerRow;

        returnSet.reserve(maxCandidates);
        passingIDs.reserve(maxCandidates);

        // For each subtable, insert the item into the table
        // and return matching IDs
        const int64_t nSubTableLists = Utility::Ssize(registersPerTable_);
        for (int64_t i = 0; i < nSubTableLists; ++i) {
            auto& subTable = packedMaps_[i];
            std::vector<std::mutex>* const mptr =
                mutexes_.size() > i ? static_cast<std::vector<std::mutex>*>(nullptr) : &mutexes_[i];
            const int64_t numberSubTables = Utility::Ssize(subTable);
            for (int64_t j = 0; j < numberSubTables; ++j) {
                assert(j < Utility::Ssize(subTable));
                auto& table = subTable[j];
                KeyT myHash = hashIndex(item, i, j);
                // Lock if necessary, don't otherwise.
                std::optional<std::lock_guard<std::mutex>> lock(
                    mptr ? std::optional<std::lock_guard<std::mutex>>((*mptr)[j])
                         : std::optional<std::lock_guard<std::mutex>>());
                auto it = table.find(myHash);
                if (it == std::cend(table)) {
                    table.emplace(myHash, std::vector<IdT>{static_cast<IdT>(myID)});
                } else {
                    for (const IdT id : it->second) {
                        assert(id < totalIDs_);
                        auto rit2 = returnSet.find(id);
                        if (rit2 == end(returnSet)) {
                            returnSet.emplace(id, 1);
                            passingIDs.push_back(id);
                        } else {
                            ++rit2->second;
                        }
                    }
                    it->second.emplace_back(myID);
                }
            }
        }
        std::vector<int32_t> passingCounts;
        if (!passingIDs.empty()) {
            passingCounts.resize(Utility::Ssize(passingIDs));
            std::transform(cbegin(passingIDs), cend(passingIDs), begin(passingCounts),
                           [&returnSet](IdT x) { return returnSet[x]; });
        }
        return std::make_tuple(passingIDs, passingCounts, itemsPerRow);
    }

    // Uses atomic increment to provide thread-safe access to numeric IDs
    int64_t NextID() noexcept
    {
        return std::atomic_fetch_add(reinterpret_cast<std::atomic<int64_t>*>(&totalIDs_),
                                     int64_t(1));
    };

    template <typename Sketch>
    std::tuple<std::vector<IdT>, std::vector<int32_t>, std::vector<int32_t>> UpdateQueryBottomK(
        const Sketch& item, int64_t maxToQuery = -1)
    {
        if (maxToQuery < 0) {
            maxToQuery = std::numeric_limits<int64_t>::max();
        }
        std::map<IdT, int32_t> matches;
        auto& map = packedMaps_.front().front();
        const int64_t myID = NextID();
        for (const auto v : item) {
            auto it = map.find(v);
            if (it == map.end()) {
                map.emplace(v, std::vector<IdT>{static_cast<IdT>(myID)});
            } else {
                for (const IdT matchind : it->second) {
                    ++matches[matchind];
                }
                it->second.emplace_back(myID);
            }
        }
        std::tuple<std::vector<IdT>, std::vector<int32_t>, std::vector<int32_t>> ret;
        std::vector<std::pair<IdT, int32_t>> mvec(matches.cbegin(), matches.cend());
        std::sort(mvec.cbegin(), mvec.cend(),
                  [](std::pair<IdT, int32_t> x, std::pair<IdT, int32_t> y) {
                      return std::tie(x.second, x.first) > std::tie(y.second, y.first);
                  });
        auto& first = std::get<0>(ret);
        auto& second = std::get<1>(ret);
        first.resize(matches.size());
        second.resize(matches.size());
        int64_t i = 0;
        for (const auto& pair : mvec) {
            first[i] = pair.first;
            second[i] = pair.second;
            ++i;
        }
        if (first.size() > maxToQuery) {
            first.resize(maxToQuery);
            second.resize(maxToQuery);
        }
        return ret;
    }

    // Used to insert into bottom-k only structures.
    // Is slower in multithreaded scenarios because there is only one map.
    template <typename Sketch>
    void InsertBottomK(const Sketch& item, int64_t myID)
    {
        auto& map = packedMaps_.front().front();
        using LockT = std::lock_guard<std::mutex>;
        const bool useLock = !mutexes_.empty() && !mutexes_.front().empty();
        std::optional<LockT> lock(useLock ? std::optional<LockT>(mutexes_.front().front())
                                          : std::optional<LockT>(std::nullopt));
        for (const auto v : item) {
            map[v].push_back(myID);
        }
    }

    template <typename Sketch>
    int64_t UpdateMultiThreaded(const Sketch& item, int nthreads = -1)
    {
        if (!isBottomKOnly_ && Utility::Ssize(item) < sketchSize_) {
            throw std::invalid_argument(
                std::string("[pbcopper] lsh index ERROR: Item has wrong size: ") +
                std::to_string(Utility::Ssize(item)) + ", expected" + std::to_string(sketchSize_));
        }

        const int64_t myID = NextID();
        if (isBottomKOnly_) {
            InsertBottomK(item, myID);
            return myID;
        }

        if (nthreads < 0) {
            if (char* s = std::getenv("OMP_NUM_THREADS"); s) {
                if (const int nt = std::atoi(s); nt > 0) {
                    nthreads = nt;
                }
            }
            if (nthreads < 0) {
                nthreads = std::thread::hardware_concurrency();
            }
        }
        const int64_t e = Utility::Ssize(registersPerTable_);
        for (int64_t i = 0; i < e; ++i) {
            auto& subTable = packedMaps_[i];
            std::vector<std::mutex>* const mptr =
                mutexes_.size() > i ? static_cast<std::vector<std::mutex>*>(nullptr) : &mutexes_[i];
            const int64_t numberSubTables = Utility::Ssize(subTable);
#ifdef _OPENMP
#pragma omp parallel for
            for (int64_t j = 0; j < numberSubTables; ++j) {
                KeyT myHash = hashIndex(item, i, j);
                auto& subSubTable = subTable[j];
                std::optional<std::lock_guard<std::mutex>> lock(
                    mptr ? std::optional<std::lock_guard<std::mutex>>((*mptr)[j])
                         : std::optional<std::lock_guard<std::mutex>>());
                if (auto it = subSubTable.find(myHash); it == subSubTable.end()) {
                    subSubTable.emplace(myHash, std::vector<IdT>{static_cast<IdT>(myID)});
                } else {
                    it->second.push_back(myID);
                }
            }
#else
            std::vector<std::thread> threads;
            std::atomic<int> dispatcher{0};
            for (const int nthreadsToLaunch = std::min(int(numberSubTables), nthreads);
                 threads.size() < nthreadsToLaunch;) {
                threads.emplace_back([i, &dispatcher, &subTable, &item, mptr, myID,
                                      nthreadsToLaunch, this]() {
                    for (;;) {
                        const int currentIndex = dispatcher++;
                        if (currentIndex >= nthreadsToLaunch) {
                            return;
                        }
                        const KeyT myHash = hashIndex(item, i, currentIndex);
                        auto& subSubTable = subTable[currentIndex];
                        const std::optional<std::lock_guard<std::mutex>> lock(
                            mptr ? std::optional<std::lock_guard<std::mutex>>((*mptr)[currentIndex])
                                 : std::optional<std::lock_guard<std::mutex>>());
                        if (auto it = subSubTable.find(myHash); it == subSubTable.end()) {
                            subSubTable.emplace(myHash, std::vector<IdT>{static_cast<IdT>(myID)});
                        } else {
                            it->second.push_back(myID);
                        }
                    }
                });
            }
            for (auto& thread : threads) {
                thread.join();
            }
#endif
        }
        return myID;
    }

    static constexpr int64_t DEFAULT_ID = std::numeric_limits<int64_t>::max();
    // Insert a single-item with the specific ID.
    // If no ID is provided, the structure provides a thread-safe ID
    template <typename Sketch>
    int64_t Insert(const Sketch& item, int64_t myID = DEFAULT_ID)
    {
        return Update(item, myID);
    }

    // Inserts all items in [beg, end)
    template <typename BIT, typename EIT, typename = std::enable_if_t<!std::is_integral_v<EIT>>>
    void Insert(BIT beg, EIT end)
    {
        while (beg != end) {
            Insert(*beg);
            ++beg;
        }
    }

    // General update code.
    // Handles both BottomK and regular insertion.
    template <typename Sketch>
    int64_t Update(const Sketch& item, int64_t myID = DEFAULT_ID)
    {
        if (!isBottomKOnly_ && int64_t(Utility::Ssize(item)) < sketchSize_) {
            throw std::invalid_argument(
                std::string("[pbcopper] lsh index ERROR: Item has wrong size: ") +
                std::to_string(Utility::Ssize(item)) + ", expected" + std::to_string(sketchSize_));
        }

        if (myID == DEFAULT_ID) {
            myID = NextID();
        }

        if (isBottomKOnly_) {
            InsertBottomK(item, myID);
            return myID;
        }

        const int64_t nSubTableLists = Utility::Ssize(registersPerTable_);
        for (int64_t i = 0; i < nSubTableLists; ++i) {
            auto& subTable = packedMaps_[i];
            std::vector<std::mutex>* mptr = nullptr;
            if (Utility::Ssize(mutexes_) > i) mptr = &mutexes_[i];
            const int64_t numberSubTables = Utility::Ssize(subTable);
            for (int64_t j = 0; j < numberSubTables; ++j) {
                const KeyT myHash = hashIndex(item, i, j);
                const std::optional<std::lock_guard<std::mutex>> lock(
                    mptr ? std::optional<std::lock_guard<std::mutex>>((*mptr)[j])
                         : std::optional<std::lock_guard<std::mutex>>());
                subTable[j][myHash].push_back(myID);
            }
        }
        return myID;
    }

private:
    uint64_t simpleFastHash(uint64_t x) const
    {
        return (((x ^ static_cast<uint64_t>(0x533f8c2151b20f97)) * 0x9a98567ed20c127dull) ^
                0x691a9d706391077a);
    }

    inline KeyT hashMemory256(const uint64_t* x) const
    {
        uint64_t v[4];
        std::memcpy(&v, x, sizeof(v));
        return wangHash(simpleFastHash(v[0]) ^
                        (simpleFastHash(v[1]) * simpleFastHash(v[2]) - v[3]));
    }

    inline KeyT hashMemory128(const uint64_t* x) const
    {
        uint64_t v[2];
        std::memcpy(&v, x, sizeof(v));
        v[0] = simpleFastHash(v[0]);
        v[1] = simpleFastHash(v[1] ^ v[0]);
        return wangHash(v[0] ^ v[1]);
    }

    inline KeyT hashMemory64(const uint64_t* x) const
    {
        uint64_t v;
        std::memcpy(&v, x, sizeof(v));
        v = wangHash(v);
        return v;
    }

    inline KeyT hashMemory32(const uint32_t* x) const
    {
        // MurMur3 finalizer
        uint32_t v;
        std::memcpy(&v, x, sizeof(v));
        v ^= v >> 16;
        v *= 0x85ebca6b;
        v ^= v >> 13;
        v *= 0xc2b2ae35;
        v ^= v >> 16;
        return v;
    }

    template <typename T>
    inline KeyT hashMemory(const T& x, int64_t n) const noexcept
    {
        // n must be > 0
        // This is enforced by registersPerTable_ checking in the constructor, which
        // throws a std::invalid_argument
        KeyT ret = 0;
        int64_t nb = sizeof(T) * n;
        if (nb <= 4) {
            uint32_t v = 0;
            std::memcpy(&v, &x, nb);
            return hashMemory32(&v);
        }
        const uint64_t* xptr = reinterpret_cast<const uint64_t*>(&x);
        switch (nb) {
            case 8:
                ret = hashMemory64(xptr);
                break;
            case 16:
                ret = hashMemory128(xptr);
                break;
            case 32:
                ret = hashMemory256(xptr);
                break;
            default: {
                while (nb >= 32) {
                    const uint64_t newHash = hashMemory256(xptr);
                    ret ^= (ret >> 31);
                    ret += newHash;
                    xptr += 4;
                    nb -= 32;
                }
                if (nb > 0) {
                    std::array<char, 32> chars{0};
                    std::memcpy(chars.data(), xptr, nb);
                    ret *= hashMemory256(reinterpret_cast<const uint64_t*>(chars.data()));
                    ret ^= (ret >> 31);
                }
            } break;
        }
        return ret;
    }

    template <typename RegT>
    static inline uint64_t seedUpdate(uint64_t& seed, const RegT& item) noexcept
    {
        seed += static_cast<uint64_t>(item);
        return Utility::WyHash64Step(seed);
    }

    template <typename Sketch>
    inline KeyT hashIndex(const Sketch& item, int64_t i, int64_t j) const noexcept
    {
        if (isBottomKOnly_) {
            return item[j];
        }
        const int64_t nreg = registersPerTable_[i];
        if (const int64_t offset = j * nreg; offset + nreg <= sketchSize_) {
            return hashMemory(item[offset], nreg);
        }
        uint64_t seed = simpleFastHash(((i << 32) ^ (i >> 32)) | j);
        uint64_t pos = seedUpdate(seed, 137u);
        using VT = std::variant<Utility::FastMod<uint32_t>, Utility::FastMod<uint64_t>>;
        const VT divVariant(sketchSize_ > 0xFFFFFFFF ? VT(Utility::FastMod<uint64_t>(sketchSize_))
                                                     : VT(Utility::FastMod<uint32_t>(sketchSize_)));
        auto singleUpdate = [&]() {
            std::visit([&seed, &item,
                        &pos](const auto& div) { pos = seedUpdate(seed, item[div.Modulus(pos)]); },
                       divVariant);
        };
        for (int64_t ri8 = nreg / 8; ri8--;) {
            singleUpdate();
            singleUpdate();  // 2
            singleUpdate();
            singleUpdate();  // 4
            singleUpdate();
            singleUpdate();  // 6
            singleUpdate();
            singleUpdate();  // 8
        }
        for (int64_t ri = (nreg / 8) * 8; ri < nreg; ++ri) {
            singleUpdate();
        }
        return seedUpdate(seed, 137u);
    }

public:
    /*
     *  Returns ids matching input minhash sketches, in order from most specific/least sensitive
     *  to least specific/most sensitive
     *  Can be then used, along with sketches, to select nearest neighbors
     *  */
    template <typename Sketch>
    std::tuple<std::vector<IdT>, std::vector<int32_t>, std::vector<int32_t>> Query(
        const Sketch& item, int64_t maxCandidates = 0, int64_t startingIdx = int64_t(-1),
        bool earlyStop = true) const
    {
        if (startingIdx < 0 || startingIdx > Utility::Ssize(registersPerTable_)) {
            startingIdx = Utility::Ssize(registersPerTable_);
        }
        Container::UnorderedMap<IdT, int32_t> returnSet;
        std::vector<IdT> passingIDs;
        std::vector<int32_t> itemsPerRow;
        returnSet.reserve(maxCandidates);
        passingIDs.reserve(maxCandidates);
        if (maxCandidates == 0) {
            maxCandidates = std::numeric_limits<int64_t>::max();
        }
        itemsPerRow.reserve(startingIdx);
        if (isBottomKOnly_) {
            auto& m = packedMaps_.front().front();
            for (int64_t j = 0;
                 j < Utility::Ssize(item) && Utility::Ssize(returnSet) < maxCandidates; ++j) {
                if (auto it = m.find(item[j]); it != cend(m)) {
                    for (const auto id : it->second) {
                        if (auto rit2 = returnSet.find(id); rit2 == end(returnSet)) {
                            returnSet.emplace(id, 1);
                            passingIDs.push_back(id);
                            if (earlyStop && Utility::Ssize(returnSet) == maxCandidates) {
                                goto bk_end;
                            }
                        } else {
                            ++rit2->second;
                        }
                    }
                }
            }
        bk_end:
            itemsPerRow.push_back(Utility::Ssize(passingIDs));
        } else {
            // Iterate through tables in order from most-specific to least-specific
            // allowing early termination to speed up filtering.
            for (std::ptrdiff_t i = startingIdx;
                 --i >= 0 && Utility::Ssize(returnSet) < maxCandidates;) {
                auto& m = packedMaps_[i];
                const int64_t numberSubTables = m.size();
                const int64_t itemsBefore = Utility::Ssize(passingIDs);
                for (int64_t j = 0; j < numberSubTables; ++j) {
                    const KeyT myHash = hashIndex(item, i, j);
                    if (auto it = m[j].find(myHash); it != m[j].cend()) {
                        for (const auto id : it->second) {
                            if (auto rit2 = returnSet.find(id); rit2 == end(returnSet)) {
                                returnSet.emplace(id, 1);
                                passingIDs.push_back(id);
                                if (earlyStop && Utility::Ssize(returnSet) == maxCandidates) {
                                    itemsPerRow.push_back(Utility::Ssize(passingIDs) - itemsBefore);
                                    goto end;
                                }
                            } else {
                                ++rit2->second;
                            }
                        }
                    }
                }
                itemsPerRow.push_back(Utility::Ssize(passingIDs) - itemsBefore);
            }
        end:;
        }
        std::vector<int32_t> passingCounts(Utility::Ssize(passingIDs));
        std::transform(cbegin(passingIDs), cend(passingIDs), begin(passingCounts),
                       [&returnSet](auto x) { return returnSet[x]; });
        return std::make_tuple(passingIDs, passingCounts, itemsPerRow);
    }

    // Writes an LSHIndex to the relevant path
    void Write(std::string path) const
    {
        std::unique_ptr<std::FILE, Utility::FileDeleter> fp{std::fopen(path.data(), "w")};
        Write(fp.get());
    }

    // Writes an LSHIndex to the relevant std::FILE *
    void Write(std::FILE* fp) const
    {
        BufferedFWrite(fp, &totalIDs_, sizeof(totalIDs_));
        int64_t nms = packedMaps_.size();
        BufferedFWrite(fp, &nms, sizeof(nms));
        for (auto& map : packedMaps_) {
            const int64_t v = Utility::Ssize(map);
            BufferedFWrite(fp, &v, sizeof(v));
        }
        BufferedFWrite(fp, registersPerTable_.data(),
                       Utility::Ssize(registersPerTable_) * sizeof(registersPerTable_.front()));
        const uint8_t isBottomK = isBottomKOnly_;
        const uint8_t islocked = !mutexes_.empty();
        BufferedFWrite(fp, &isBottomK, 1);
        BufferedFWrite(fp, &islocked, 1);
        for (int64_t i = 0; i < packedMaps_.size(); ++i) {
            for (int64_t j = 0; j < packedMaps_[i].size(); ++j) {
                auto& map = packedMaps_[i][j];
                const uint64_t sz = map.size();
                BufferedFWrite(fp, &sz, sizeof(sz));
                for (auto& pair : map) {
                    uint64_t psz = pair.second.size();
                    BufferedFWrite(fp, &psz, sizeof(psz));
                    BufferedFWrite(fp, &pair.first, sizeof(pair.first));
                    BufferedFWrite(fp, pair.second.data(), sizeof(KeyT) * pair.second.size());
                }
            }
        }
    }

    void Clear()
    {
        totalIDs_ = 0;
        packedMaps_.clear();
        mutexes_.clear();
        registersPerTable_.clear();
    }
};

}  // namespace Algorithm
}  // namespace PacBio

#endif /* #ifndef PBCOPPER_ALGORITHM_LSHINDEX_H */
