#ifndef PBCOPPER_QGRAM_INDEX_INL_H
#define PBCOPPER_QGRAM_INDEX_INL_H

#include "pbcopper/qgram/Index.h"
#include "pbcopper/qgram/internal/Hashing-inl.h"
#include "pbcopper/utility/MoveAppend.h"
#include "pbcopper/utility/SafeSubtract.h"
#include <stdexcept>

namespace PacBio {
namespace QGram {
namespace internal {

class IndexImpl
{
public:
    using SuffixArray_t = std::vector<IndexHit>;
    using HashLookup_t  = std::vector<uint64_t>;

public:
    // ctor
    IndexImpl(const size_t q, const std::vector<std::string>& seqs);
    IndexImpl(const size_t q, std::vector<std::string>&& seqs);

    // index lookup API
    IndexHit Hit(const Shape& shape) const;
    IndexHits Hits(const Shape& shape, const size_t queryPos) const;
    std::vector<IndexHits> Hits(const std::string& seq,
                                const bool filterHomopolymers) const;

    // "private" method(s) - index construction
    void Init(void);

    // "private" method(s) - purely for testing access
    const HashLookup_t& HashLookup(void) const;
    const size_t& Size(void) const;
    const SuffixArray_t& SuffixArray(void) const;

private:
    const size_t q_;                        // qGramSize
    const std::vector<std::string> seqs_;   // underlying text
    SuffixArray_t suffixArray_;             // suffix array sorted by the first q chars
    HashLookup_t hashLookup_;               // hash value -> SA index
};

inline IndexImpl::IndexImpl(const size_t q, const std::vector<std::string>& seqs)
    : q_{q}
    , seqs_{seqs}
{
    Init();
}

inline IndexImpl::IndexImpl(const size_t q, std::vector<std::string>&& seqs)
    : q_{q}
    , seqs_{std::move(seqs)}
{
    Init();
}

inline const IndexImpl::HashLookup_t& IndexImpl::HashLookup(void) const
{ return hashLookup_; }

inline void IndexImpl::Init(void)
{
    if (q_ == 0 || q_ > 16)
        throw std::runtime_error("QGram::Index: q must be in the range from [1-16]");

    // init hash lookup (and calculate totalNumQGrams for later suffixArray)
    const auto lookupSize = static_cast<size_t>(std::pow(4, q_) + 1);
    hashLookup_.resize(lookupSize, 0);
    uint32_t totalNumQGrams = 0;
    for (const auto& seq : seqs_) {
        const auto seqLength = seq.size();
        if (seqLength < q_)
            continue;
        const auto numQGrams = seqLength - q_ + 1;
        Shape shape{ q_, seq };
        for (size_t i = 0; i < numQGrams; ++i)
            ++hashLookup_[shape.HashNext()];
        totalNumQGrams += numQGrams;
    }

    // update hash lookup values (cumulative sum along the table)
    uint64_t prevDiff  = 0;
    uint64_t prevDiff2 = 0;
    uint64_t sum       = 0;
    for (auto& hash : hashLookup_) {
        sum += prevDiff2;
        prevDiff2 = prevDiff;
        prevDiff = hash;
        hash = sum;
    }

    // init suffix array
    suffixArray_.resize(totalNumQGrams);
    uint32_t seqNo = 0;
    for (const auto& seq : seqs_) {
        const auto seqLength = seq.size();
        if (seqLength < q_)
            continue;

        Shape shape{ q_, seq };
        const auto numQGrams = seqLength - q_ + 1;
        for (uint32_t i = 0; i < numQGrams; ++i)
            suffixArray_[hashLookup_[shape.HashNext()+1]++] = IndexHit{seqNo, i};

        ++seqNo;
    }
}

inline IndexHit IndexImpl::Hit(const Shape& shape) const
{
    const auto b = shape.currentHash_;
    const auto i = hashLookup_[b];
    return suffixArray_[i];
}

inline IndexHits IndexImpl::Hits(const Shape& shape, const size_t queryPos) const
{
    const auto b = shape.currentHash_;
    const auto iter = hashLookup_[b];
    const auto end = hashLookup_[b+1];
    return IndexHits { &suffixArray_, iter, end, queryPos };
}

inline std::vector<IndexHits> IndexImpl::Hits(const std::string& seq,
                                              const bool filterHomopolymers) const
{
    std::vector<IndexHits> result;
    const auto end = ::PacBio::Utility::SafeSubtract(seq.size()+1, q_);
    result.reserve(end);
    Shape shape{ q_, seq };

    if (!filterHomopolymers) {
        for (size_t i = 0; i < end; ++i) {
            shape.HashNext();
            result.emplace_back(Hits(shape, i));
        }
    }
    else {
        HpHasher isHomopolymer(q_);
        for (size_t i = 0; i < end; ++i) {
            if (!isHomopolymer(shape.HashNext()))
                result.emplace_back(Hits(shape, i));
        }
    }
    return result;
}

inline const size_t& IndexImpl::Size(void) const
{ return q_; }

inline const IndexImpl::SuffixArray_t& IndexImpl::SuffixArray(void) const
{ return suffixArray_; }

} // namespace internal

inline Index::Index(const size_t q, const std::string& seq)
    : Index{ q, {1, seq} }
{ }

inline Index::Index(const size_t q, std::string&& seq)
    : Index{ q, {1, std::move(seq)} }
{ }

inline Index::Index(const size_t q, const std::vector<std::string>& seqs)
    : d_(new internal::IndexImpl(q, seqs))
{ }

inline Index::Index(const size_t q, std::vector<std::string>&& seqs)
    : d_(new internal::IndexImpl(q, std::move(seqs)))
{ }

inline std::vector<IndexHits> Index::Hits(const std::string& seq,
                                          const bool filterHomopolymers) const
{
    assert(d_);
    return d_->Hits(seq, filterHomopolymers);
}

inline size_t Index::Size(void) const
{
    assert(d_);
    return d_->Size();
}

} // namespace QGram
} // namespace PacBio

#endif // PBCOPPER_QGRAM_INDEX_INL_H
