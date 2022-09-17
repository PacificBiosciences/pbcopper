#ifndef PBCOPPER_QGRAM_HASHING_INL_H
#define PBCOPPER_QGRAM_HASHING_INL_H

#include <pbcopper/PbcopperConfig.h>

#include <array>
#include <stdexcept>
#include <string>

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace PacBio {
namespace QGram {
namespace internal {

constexpr uint8_t ALPHABET_SIZE = 4;  // {A,C,G,T}

constexpr uint8_t BaseCode(const char c)
{
    constexpr std::array<uint8_t, 128> BASE_CODE = {
        // C->1, G->2, T->3, else 0
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    assert(c >= 0);
    return BASE_CODE[static_cast<unsigned char>(c)];
}

// recursive q-gram hash calculator
inline uint64_t HashImpl(uint64_t hash, std::string::const_iterator iter, size_t q)
{
    if (q == 1) {
        return hash;
    } else {
        ++iter;
        return HashImpl(hash * ALPHABET_SIZE + BaseCode(*iter), iter, q - 1);
    }
}

struct Shape
{
public:
    const size_t q_;                    // q-gram size
    const uint32_t hashFactor_;         // hash multiplier
    const std::string& seq_;            // input sequence
    std::string::const_iterator iter_;  // sequence iterator
    uint64_t currentHash_;              // current hash value
    char leftChar_;                     // leftmost character

public:
    Shape(const size_t q, const std::string& seq)
        : q_{q}
        , hashFactor_{
            // need to perform the range check before initializing,
            // as this would trigger undefined behavior otherwise
            ((1 <= q_) && (q_ <= 16))
                          ? static_cast<uint32_t>(std::pow(ALPHABET_SIZE, q_ - 1))
                          : throw std::invalid_argument{"[pbcopper] qgram ERROR: qgram size (" + std::to_string(q_) +
                                                        ") must be in the range [1,16]"}}
        , seq_(seq)
        , iter_{seq_.cbegin()}
        , currentHash_{0}
        , leftChar_{'\0'}
    {
        // We check for q & sequence size in the main QGram::Index entry points,
        // but want to still enforce this requirement here (in case of clients
        // 'sneaking' into internal implementation or, more likely, in case later
        // refactoring upstream removes the check).
        if (seq.size() < q_) {
            throw std::invalid_argument{"[pbcopper] qgram ERROR: sequence size (" +
                                        std::to_string(seq.size()) + ") must be >= q (" +
                                        std::to_string(q_)};
        }

        currentHash_ = HashImpl(BaseCode(*iter_), iter_, q_ - 1);
    }

    uint64_t HashNext()
    {
        currentHash_ = ((currentHash_ - BaseCode(leftChar_) * hashFactor_) * ALPHABET_SIZE) +
                       BaseCode(*(iter_ + (q_ - 1)));
        leftChar_ = *iter_;
        ++iter_;
        return currentHash_;
    }
};

class HpHasher
{
public:
    HpHasher(const size_t q)
    {
        constexpr std::array<char, ALPHABET_SIZE> DNA = {'A', 'C', 'G', 'T'};
        for (size_t i = 0; i < ALPHABET_SIZE; i++) {
            const std::string s = std::string(q, DNA[i]);
            const auto it = s.cbegin();
            const auto h = BaseCode(*it);
            hashes[i] = HashImpl(h, it, q);
        }
    }

    bool operator()(const uint64_t h) const noexcept
    {
        return (h == hashes[0] || h == hashes[1] || h == hashes[2] || h == hashes[3]);
    }

private:
    uint64_t hashes[ALPHABET_SIZE];
};

}  // namespace internal
}  // namespace QGram
}  // namespace PacBio

#endif  // PBCOPPER_QGRAM_HASHING_INL_H
