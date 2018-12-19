// Author: Derek Barnett

#ifndef PBCOPPER_QGRAM_INDEX_H
#define PBCOPPER_QGRAM_INDEX_H

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/PbcopperMakeUnique.h>
#include <pbcopper/qgram/IndexHits.h>

namespace PacBio {
namespace QGram {

namespace internal {
class IndexImpl;
}

///
/// \brief The Index class provides efficient retrieval of all occurrences of
///        a particular q-gram in the input sequence.
///
/// Algorithm and structure adapted to our needs from:
///     <seqan/index/index_qgram.h>
///
class Index
{
public:
    ///
    /// \brief Index
    /// \param[in] q    q-gram size
    /// \param[in] seq  construct index from this sequence
    /// \throws std::runtime_error if q-gram == 0
    ///
    Index(size_t q, std::string seq);

    ///
    /// \brief Index
    /// \param[in] q        q-gram size
    /// \param[in] seqs     construct index from these sequences
    /// \throws std::runtime_error if q-gram == 0
    ///
    Index(size_t q, std::vector<std::string> seqs);

    Index(const Index& other);
    Index(Index&& /*unused*/) = default;
    Index& operator=(const Index& other);
    Index& operator=(Index&& /*unused*/) = default;
    ~Index() = default;

public:
    ///
    /// \brief Hits
    /// \param[in] seq                  query sequence
    /// \param[in] filterHomopolymers   do not count hits on homopolymers (len == q)
    /// \return a vector of IndexHits results.
    ///
    std::vector<IndexHits> Hits(const std::string& seq,
                                const bool filterHomopolymers = false) const;

    ///
    /// \brief Size
    /// \return q-gram size
    ///
    size_t Size() const;

private:
    std::unique_ptr<internal::IndexImpl> d_;
};

}  // namespace QGram
}  // namespace PacBio

#include <pbcopper/qgram/internal/Index-inl.h>

#endif  // PBCOPPER_QGRAM_INDEX_H
