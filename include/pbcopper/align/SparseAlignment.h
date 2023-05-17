#ifndef PBCOPPER_ALIGN_SPARSEALIGNMENT_H
#define PBCOPPER_ALIGN_SPARSEALIGNMENT_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/align/Seed.h>

#include <string>
#include <utility>
#include <vector>

namespace PacBio {
namespace Align {

/// \brief Generate an SDP alignment from two sequences
///
/// \param[in] qGramSize    qgram size to use for index hashing
/// \param[in] seq1         The query sequence
/// \param[in] seq2         The reference sequence
/// \param[in] filterHomopolymers If true, homopolymer k-mers will be filtered before searching the index.
///
/// \returns   The SDP alignment as a vector of Seeds
///
std::vector<Seed> SparseAlignSeeds(std::size_t qGramSize, const std::string& seq1,
                                   const std::string& seq2, bool filterHomopolymers);

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
std::vector<Seed> SparseAlignSeeds(std::size_t qGramSize, const std::string& seq1,
                                   const std::string& seq2);

/// \brief Generate an SDP alignment from the best orientation of two sequences
///
/// \param[in] seq1   The query sequence as a DnaString
/// \param[in] seq2   The reference sequence as a DnaString
/// \param[in] filterHomopolymers If true, homopolymer k-mers will be filtered before searching the index.
///
/// \returns   A flag for the best orientation found, and the SDP alignment
///             from that orientation as a SeedString, in an std::pair
///
std::pair<std::size_t, std::vector<Seed>> BestSparseAlign(const std::string& seq1,
                                                          const std::string& seq2,
                                                          bool filterHomopolymers);

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
std::pair<std::size_t, std::vector<Seed>> BestSparseAlign(const std::string& seq1,
                                                          const std::string& seq2);

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
std::vector<std::pair<std::size_t, std::size_t>> SparseAlign(std::size_t qGramSize,
                                                             const std::string& seq1,
                                                             const std::string& seq2,
                                                             bool filterHomopolymers);

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
std::vector<std::pair<std::size_t, std::size_t>> SparseAlign(std::size_t qGramSize,
                                                             const std::string& seq1,
                                                             const std::string& seq2);

}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_SPARSEALIGNMENT_H
