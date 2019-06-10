// Author: Brett Bowman

#ifndef PBCOPPER_ALIGN_SPARSEALIGNMENT_H
#define PBCOPPER_ALIGN_SPARSEALIGNMENT_H

#include <vector>

#include <pbcopper/align/Seeds.h>

#include <pbcopper/align/ChainSeeds.h>
#include <pbcopper/align/FindSeeds.h>

namespace PacBio {
namespace Align {

namespace {

/// \brief Generate an SDP alignment from two sequences
///
/// \param[in] qGramSize    qgram size to use for index hashing
/// \param[in] seq1         The query sequence
/// \param[in] seq2         The reference sequence
/// \param[in] filterHomopolymers If true, homopolymer k-mers will be filtered before searching the index.
///
/// \returns   The SDP alignment as a vector of Seeds
///
inline std::vector<Seed> SparseAlignSeeds(const size_t qGramSize, const std::string& seq1,
                                          const std::string& seq2, const bool filterHomopolymers)
{
    const auto seeds = FindSeeds(qGramSize, seq1, seq2, filterHomopolymers);
    const auto chains = ChainSeeds(seeds, ChainSeedsConfig{});
    if (chains.empty()) return std::vector<Seed>{};
    return chains[0];
}

/// \brief Generate an SDP alignment from two sequences
///
/// This overload enables homopolymer-filtering when FILTERHOMOPOLYMERS is defined.
///
/// \param[in] qGramSize    qgram size to use for index hashing
/// \param[in] seq1         The query sequence
/// \param[in] seq2         The reference sequence
///
/// \returns   The SDP alignment as a vector of Seeds
///
inline std::vector<Seed> SparseAlignSeeds(const size_t qGramSize, const std::string& seq1,
                                          const std::string& seq2)
{
#ifdef FILTERHOMOPOLYMERS
    const bool filterHomopolymers = true;
#else
    const bool filterHomopolymers = false;
#endif

    return SparseAlignSeeds(qGramSize, seq1, seq2, filterHomopolymers);
}

/// \brief Generate an SDP alignment from the best orientation of two sequences
///
/// \param[in] seq1   The query sequence as a DnaString
/// \param[in] seq2   The reference sequence as a DnaString
/// \param[in] filterHomopolymers If true, homopolymer k-mers will be filtered before searching the index.
///
/// \returns   A flag for the best orientation found, and the SDP alignment
///             from that orientation as a SeedString, in an std::pair
///
inline std::pair<size_t, std::vector<Seed>> BestSparseAlign(const std::string& seq1,
                                                            const std::string& seq2,
                                                            const bool filterHomopolymers)
{
    const auto seq2rc = ::PacBio::Data::ReverseComplement(seq2);

    const auto fwd = SparseAlignSeeds(10, seq1, seq2, filterHomopolymers);
    const auto rev = SparseAlignSeeds(10, seq1, seq2rc, filterHomopolymers);

    if (fwd.size() > rev.size()) return std::make_pair(0, fwd);
    return std::make_pair(1, rev);
}

/// \brief Generate an SDP alignment from the best orientation of two sequences
///
/// This overload enables homopolymer-filtering when FILTERHOMOPOLYMERS is defined.
///
/// \param[in] seq1   The query sequence as a DnaString
/// \param[in] seq2   The reference sequence as a DnaString
///
/// \returns   A flag for the best orientation found, and the SDP alignment
///             from that orientation as a SeedString, in an std::pair
///
inline std::pair<size_t, std::vector<Seed>> BestSparseAlign(const std::string& seq1,
                                                            const std::string& seq2)
{
#ifdef FILTERHOMOPOLYMERS
    const bool filterHomopolymers = true;
#else
    const bool filterHomopolymers = false;
#endif

    return BestSparseAlign(seq1, seq2, filterHomopolymers);
}

/// \brief Generate an SDP alignment from two sequences and hide the
///         SeqAn library dependencies
///
/// \param[in] qGramSize qgram size to use for index hashing
/// \param[in] seq1      The query sequence as an std::string
/// \param[in] seq2      The reference sequence as a std::string
/// \param[in] filterHomopolymers If true, homopolymer k-mers will be filtered before searching the index.
///
/// \returns   A vector of pairs, representing Kmer start positions
///             that match in the query and reference sequences
///
inline const std::vector<std::pair<size_t, size_t>> SparseAlign(const size_t qGramSize,
                                                                const std::string& seq1,
                                                                const std::string& seq2,
                                                                const bool filterHomopolymers)
{
    std::vector<std::pair<size_t, size_t>> result;

    const auto chain = SparseAlignSeeds(qGramSize, seq1, seq2, filterHomopolymers);
    for (const auto& s : chain)
        result.emplace_back(s.BeginPositionH(), s.BeginPositionV());

    return result;
}

/// \brief Generate an SDP alignment from two sequences and hide the
///         SeqAn library dependencies
///
/// This overload enables homopolymer-filtering when FILTERHOMOPOLYMERS is defined.
///
/// \param[in] qGramSize qgram size to use for index hashing
/// \param[in] seq1      The query sequence as an std::string
/// \param[in] seq2      The reference sequence as a std::string
///
/// \returns   A vector of pairs, representing Kmer start positions
///             that match in the query and reference sequences
///
inline const std::vector<std::pair<size_t, size_t>> SparseAlign(const size_t qGramSize,
                                                                const std::string& seq1,
                                                                const std::string& seq2)
{
#ifdef FILTERHOMOPOLYMERS
    const bool filterHomopolymers = true;
#else
    const bool filterHomopolymers = false;
#endif

    return SparseAlign(qGramSize, seq1, seq2, filterHomopolymers);
}

}  // Anonymous namespace
}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_SPARSEALIGNMENT_H
