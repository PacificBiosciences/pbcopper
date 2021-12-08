#ifndef PBCOPPER_ALIGN_CHAINSEEDS_H
#define PBCOPPER_ALIGN_CHAINSEEDS_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/align/ChainSeedsConfig.h>
#include <pbcopper/align/Seeds.h>

#include <boost/optional.hpp>

#include <map>
#include <queue>
#include <set>
#include <utility>
#include <vector>

namespace PacBio {
namespace Align {

// H is query, V is reference

/// Calculate number of bases between a seed and the diagonal
///  axis of the matrix it's in as extended outward from the
///  upper-left-hand corner of the matrix toward the lower right.
///  High numbers are closer to the upper-right, negative numbers
///  closer to the lower-left.
///
/// \param  seed  The seed whose diagonal is desired.
///
/// \return  long  The distance from the seed's upper-left corner and
///                the diagonal axis of the matrix.
///
long Diagonal(const Seed& seed);

/// Compare seeds for sorting, first in the horizontal (query)
///  dimension, then in the vertical (reference) dimension.
///
/// \param  lhs  The left-hand side seed to compare
/// \param  rhs  The right-hand side seed to compare
///
/// \return  bool  Whether the left seed preceeds the right seed
///
bool HVCompare(const Seed& lhs, const Seed& rhs);

/// Compare seeds for sorting, first in the vertical (reference)
///  dimension, then in the horizontal (query) dimension.
///
/// \param  lhs  The left-hand side seed to compare
/// \param  rhs  The right-hand side seed to compare
///
/// \return  bool  Whether the left seed preceeds the right seed
///
bool VHCompare(const Seed& lhs, const Seed& rhs);

/// Compare seeds for sorting, by whether one seed is higher or
/// lower than another in the sparse matrix of their alignment,
/// according to their diagonals.  Seeds near the upper-right
/// corner are said to preceed seeds closer to the lower-left.
///
/// \param  lhs  The left-hand seed to compare
/// \param  rhs  The right-hand seed to compare
///
/// \return  bool  Whether the left seed preceeds the right seed
///
bool DiagonalCompare(const Seed& lhs, const Seed& rhs) noexcept;

/// Score the possible linkage of two seeds based on 3 criteria:
///     (A) The number of bases in the shortest seed
///     (B) The number of bases between the two seeds
///     (C) The size of the difference between their diagonals
/// each with it's own weight(s).
///
/// \param  lhs     The left-hand side seed to link
/// \param  rhs     The right-hand side seed to link
/// \param  config  Provides scoring values to use when chaining
///
/// \return  long  The score associated with the linkage
///
long LinkScore(const Seed& lhs, const Seed& rhs, const ChainSeedsConfig& config);

///
/// A Sparse Dynamic Programming hit.  A wrapper around SeqAn's Seed class
/// with an additional field for storing it's index in the original seed set.
///
/// TODO (bbowman): Can I replace this struct with just Seeds by using the
///                 build in SeedScore field and setter/getters?
///
struct SDPHit : public Seed
{
    size_t Index;

    SDPHit(const Seed& seed, const size_t index);
    bool operator<(const SDPHit& other) const noexcept;
};

/// Compare two seeds (SDPHits) according to their original indices in
/// their source seedSet
///
/// \param  lhs  The left-hand side seed to compare
/// \param  rhs  The right-hand side seed to compare
///
/// \return  bool  Whether the original index of LHS is less than RHS
///
bool IndexCompare(const SDPHit& lhs, const SDPHit& rhs);

///
/// A column in the Sparse Dynamic Programming matrix.  A wrapper around the above
///  SDPHit struct to give it a column field and a comparison operator that uses it.
///
/// TODO (bbowman): Can I replace this struct with just SDPHits or just Seeds, since
///                 we get the Column from SeqAn function endPositionH() function
///                 anyway?
///
struct SDPColumn
{
    boost::optional<SDPHit> Seed;
    size_t Column;

    SDPColumn(size_t column, const boost::optional<SDPHit> seed = boost::none);
    bool operator<(const SDPColumn& other) const noexcept;
};

///
///
/// \param  seeds  A vector of the SDPHits, sorted by their position in the
///                horizontal (query)
/// \param  sweepSet
///
/// \return  vector<optional<SDPHits>>  For each seed in the input vector, the
///             first index that is visible to it's left, if any.
std::vector<boost::optional<SDPHit>> ComputeVisibilityLeft(const std::vector<SDPHit>& seeds,
                                                           std::set<SDPHit>& sweepSet);

///
/// A possible chain of SDP seeds.  A simple struct for wrapping the
///  three pieces of information we need to filter and possibly
///  reconstruct a chain that we've found:
///     (A) The reference sequence where the chain was found
///     (B) The terminal seed in the chain
///     (C) The chain's score
///
struct ChainHit
{
    size_t seedSetIdx;
    size_t endIndex;
    long score;
};

//
// Functor for sorting ChainHits by score
//
class ChainHitCompare
{
public:
    bool operator()(const ChainHit& lhs, const ChainHit& rhs) const noexcept;
};

/// Though we expect to receive the Seeds we'll be chaining in a tree-like
///  Seeds collection, we need them and their scores in well-ordered vectors to
///  perform the actual chaining ourselves.  This function is a simple
///  helper for abstracting away that process.
///
/// \param  seedSet  The collection of seeds to be chained
/// \param  seeds    The vector of SDPHit objects that will actually be chained
/// \param  scores   The vector of scores to initialize with default per-seed scores
void InitializeSeedsAndScores(const Seeds& seedSet, std::vector<SDPHit>* seeds,
                              std::vector<long>* scores);

/// Search a SeedSet for the best numCandidates sets of locally-chainable
/// seeds according to some scoring criteria.  Seed chains are scored based
/// on their length and penalized according to the distance between them and
/// how far apart their diagonals are.  Final scores for a chain must be above
/// some minScore threshold to be reported.
///
/// Roughly equivalent to BLASR's SDPAlign
///
/// TODO (bbowman): I shouldn't report partial and complete chains of the same seeds
/// TODO (bbowman): Figure out why my penalties need to be lower than BLASR's
///                 for similar results
/// TODO (dbarnett): This parameter documentation below is _way_ out of sync.
///
/// \param  chains  A vector of SeedSets to store and return collections of
///                 locally chained seeds in.
/// \param  seedSet  The SeedSet to search for chains in
/// \param  numCandidates  The maximum number of chains to report
/// \param  minScore  The minimum score to require from a reported chain
/// \param  match  Score to add to each matching base in each seed
/// \param  nonmatch  Penalty for each non-aligned base between seeds
///                   along whichever dimension they are closest together
/// \param  insertion  Penalty for each base along the diagonal that
///                    separates two seeds, if the 'upstream' seed is on
///                    a higher diagonal than the 'downstream' seed.
/// \param  deletion  Penalty for each base along the diagonal that
///                   separates two seeds, if the 'upstream' seed is on
///                   a lower diagonal than the 'downstream' seed.
void __attribute__((__unused__))
ChainSeedsImpl(std::priority_queue<ChainHit, std::vector<ChainHit>, ChainHitCompare>* chainHits,
               std::vector<boost::optional<size_t>>* chainPred, std::vector<SDPHit>* seeds,
               std::vector<long>& scores, const size_t seedSetIdx, const ChainSeedsConfig& config);

/// Search a Seed set for the best numCandidates sets of locally-chainable
/// seeds according to some scoring criteria.  Seed chains are scored based
/// on their length and penalized according to the distance between them and
/// how far apart their diagonals are.  Final scores for a chain must be above
/// some minScore threshold to be reported.
///
/// Roughly equivalent to BLASR's SDPAlign
///
/// \param  seedSet The Seeds set to search for chains in
/// \param  config  Provides scoring values to use when chaining
///
/// \return  A vector of Seed vectors containing locally chained seeds.
///
std::vector<std::vector<Seed>> ChainSeeds(const Seeds& seedSet, const ChainSeedsConfig& config);

/// Search a Seed set for the best numCandidates sets of locally-chainable
/// seeds according to some scoring criteria.  Seed chains are scored based
/// on their length and penalized according to the distance between them and
/// how far apart their diagonals are.  Final scores for a chain must be above
/// some minScore threshold to be reported.
///
/// Roughly equivalent to BLASR's SDPAlign
///
/// \param  seedSet The SeedSet to search for chains in
/// \param  config  Provides scoring values to use when chaining
///
/// \return A vector of Seed sets containing locally chained seeds.
///
std::vector<Seeds> ChainedSeedSets(const Seeds& seedSet, const ChainSeedsConfig& config);

/// Search a Seed set for the best numCandidates sets of locally-chainable
/// seeds according to some scoring criteria.  Seed chains are scored based
/// on their length and penalized according to the distance between them and
/// how far apart their diagonals are.  Final scores for a chain must be above
/// some minScore threshold to be reported.
///
/// Roughly equivalent to BLASR's SDPAlign
///
/// \param  seedSet The SeedSet to search for chains in
/// \param  config  Provides scoring values to use when chaining
///
/// \return  A vector of SeedSets containing locally chained seeds.
///
std::vector<std::pair<size_t, Seeds>> ChainSeeds(const std::map<size_t, Seeds> seedSets,
                                                 const ChainSeedsConfig config);

}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_CHAINSEEDS_H
