#ifndef PBCOPPER_ALIGN_FINDSEEDS_H
#define PBCOPPER_ALIGN_FINDSEEDS_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/align/Seeds.h>

#include <map>
#include <optional>
#include <string>

/*
 * This file contains a few minimal wrapper functions around the index types
 * provided by SeqAn for finding K-mer seeds between some query sequence
 * and some reference sequence or index.
 *
 * NOTE: Though these functions should in theory work with any index type
 * supported by SeqAn, they have only been extensively tested with the QGram
 * index specialization.  Use other types at your own risk.
 *
 * In addition to the templating around the TConfig, of which more details
 * are in FindSeedConfig.h, there are two pre-processor directives that
 * can be used to further customize the code, described below.  Speed costs
 * associated with either are highly application-specific, so we recommend
 * testing both for each new application.
 *
 * MERGESEEDS: There are two common specializations of the addSeeds function
 *      that we use to add a K-mer hit to a SeedSet - Single() and Merge().
 *      By default FindSeeds uses Single(), which is the fastest method
 *      because it does no chaining at all.  Alternatively, we can use the
 *      Merge() function from SeqAn, which combines seeds that precisely
 *      overlap with each other but is slightly slower.  The advantage of
 *      Merge() is that the resulting SeedSets are smaller, so down-stream
 *      processes that require for sorting and iterating may be greatly
 *      expedited.  Enable this directive to use Merge() instead of Single().
 *
 * FILTERHOMOPOLYMERS: By default, FindSeeds treats all K-mer seeds it finds
 *      as equal.  However if the sequences might contain large homopolymers,
 *      or if there is a large number of sequences in the the reference /
 *      index, it may be faster in the long run to spend some CPU cycles
 *      checking whether a K-mer is a homopolymer before searching the index
 *      for it.  Enable this directive to enable that filter.
 */

namespace PacBio {

namespace QGram {
class Index;
}  // namespace QGram

namespace Align {

/// Find all matching seeds between a DNA index and the sequences
/// represented in some supplied index of the type specified in TConfig.
/// Since some index types, most notably the QGram index, can store seeds
/// from multiple references, the return value has to be a map of seed sets
/// rather than a single one.  In addition the query sequence may itself
/// be in the index, in which case we pass in it's known index so we do
/// not count it.
///
/// \param[in]  index               The hashed index on the reference sequence(s)
/// \param[in]  seq                 The query sequence
/// \param[in]  qIdx                (optional) The index of the query sequence, so it can be ignored
/// \param[in]  filterHomopolymers  If true, homopolymer k-mers will be filtered before searching the index.
///
/// \return map containing Seeds for each referenceIndex with a hit
///
std::map<size_t, Seeds> FindSeeds(const PacBio::QGram::Index& index, const std::string& seq,
                                  std::optional<size_t> qIdx, bool filterHomopolymers);

/// Find all matching seeds between a DNA index and the sequences
/// represented in some supplied index of the type specified in TConfig.
/// Since some index types, most notably the QGram index, can store seeds
/// from multiple references, the return value has to be a map of seed sets
/// rather than a single one.  In addition the query sequence may itself
/// be in the index, in which case we pass in it's known index so we do
/// not count it.
///
/// This overload enables homopolymer-filtering when FILTERHOMOPOLYMERS is defined.
///
/// \param[in]  index   The hashed index on the reference sequence(s)
/// \param[in]  seq     The query sequence
/// \param[in]  qIdx    (optional) The index of the query sequence, so it can be ignored
///
/// \return  map containing Seeds for each referenceIndex with a hit
///
std::map<size_t, Seeds> FindSeeds(const PacBio::QGram::Index& index, const std::string& seq,
                                  std::optional<size_t> qIdx);

/// Find all matching seeds between a DNA index and the sequences
/// represented in some supplied index of the type specified in TConfig.
/// Since some index types, most notably the QGram index, can store seeds
/// from multiple references, the return value has to be a map of seed sets
/// rather than a single one.
///
/// \param[in]  index               The hashed index on the reference sequence(s)
/// \param[in]  seq                 The query sequence
/// \param[in]  filterHomopolymers  If true, homopolymer k-mers will be filtered before searching the index.
///
/// \return  map containing Seeds for each referenceIndex with a hit
///
std::map<size_t, Seeds> FindSeeds(const PacBio::QGram::Index& index, const std::string& seq,
                                  bool filterHomopolymers);

/// Find all matching seeds between a DNA index and the sequences
/// represented in some supplied index of the type specified in TConfig.
/// Since some index types, most notably the QGram index, can store seeds
/// from multiple references, the return value has to be a map of seed sets
/// rather than a single one.
///
/// This overload enables homopolymer-filtering when FILTERHOMOPOLYMERS is defined.
///
/// \param[in]  index  The hashed index on the reference sequence(s)
/// \param[in]  seq    The query sequence
///
/// \return  map containing Seeds for each referenceIndex with a hit
///
std::map<size_t, Seeds> FindSeeds(const PacBio::QGram::Index& index, const std::string& seq);

/// Find all matching seeds between two DNA sequences
///
/// \param[in]  qGramSize           qgram size to use for index hashing
/// \param[in]  seq1                The first, or query, sequence
/// \param[in]  seq2                The second, or reference, sequence
/// \param[in]  filterHomopolymers  If true, homopolymer k-mers will be filtered before searching the index.
///
/// \return Seeds collection containing all hits
///
Seeds FindSeeds(size_t qGramSize, const std::string& seq1, const std::string& seq2,
                bool filterHomopolymers);

/// Find all matching seeds between two DNA sequences
///
/// This overload enables homopolymer-filtering when FILTERHOMOPOLYMERS is defined.
///
/// \param[in]  qGramSize   qgram size to use for index hashing
/// \param[in]  seq1        The first, or query, sequence
/// \param[in]  seq2        The second, or reference, sequence
///
/// \return Seeds collection containing all hits
///
Seeds FindSeeds(size_t qGramSize, const std::string& seq1, const std::string& seq2);

}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_FINDSEEDS_H
