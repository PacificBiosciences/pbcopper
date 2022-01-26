#include <pbcopper/align/ChainSeeds.h>

#include <algorithm>
#include <limits>
#include <tuple>

namespace PacBio {
namespace Align {

SDPHit::SDPHit(const Seed& seed, const size_t index) : Seed{seed}, Index{index} {}

bool SDPHit::operator<(const SDPHit& other) const noexcept { return DiagonalCompare(*this, other); }

SDPColumn::SDPColumn(size_t column, const std::optional<SDPHit> seed) : Seed{seed}, Column{column}
{
}

bool SDPColumn::operator<(const SDPColumn& other) const noexcept { return Column < other.Column; }

bool ChainHitCompare::operator()(const ChainHit& lhs, const ChainHit& rhs) const noexcept
{
    return lhs.score > rhs.score;
}

long Diagonal(const Seed& seed)
{
    const auto v = seed.BeginPositionV();
    const auto h = seed.BeginPositionH();

    if (v > h) {
        return -static_cast<long>(v - h);
    }

    return static_cast<long>(h - v);
}

bool HVCompare(const Seed& lhs, const Seed& rhs)
{
    const auto leftH = lhs.BeginPositionH();
    const auto rightH = rhs.BeginPositionH();

    if (leftH < rightH) {
        return true;
    }

    if (leftH == rightH) {
        return lhs.EndPositionV() < rhs.EndPositionV();
    }

    return false;
}

bool VHCompare(const Seed& lhs, const Seed& rhs)
{
    const auto leftV = lhs.BeginPositionV();
    const auto rightV = rhs.BeginPositionV();

    if (leftV < rightV) {
        return true;
    }

    if (leftV == rightV) {
        return lhs.EndPositionH() < rhs.EndPositionH();
    }

    return false;
}

bool DiagonalCompare(const Seed& lhs, const Seed& rhs) noexcept
{
    return std::make_tuple(lhs.BeginDiagonal(), lhs.BeginPositionH()) <
           std::make_tuple(rhs.BeginDiagonal(), rhs.BeginPositionH());
}

long LinkScore(const Seed& lhs, const Seed& rhs, const ChainSeedsConfig& config)
{
    const auto lH = static_cast<long>(lhs.BeginPositionH());
    const auto lV = static_cast<long>(lhs.BeginPositionV());
    const auto rH = static_cast<long>(rhs.BeginPositionH());
    const auto rV = static_cast<long>(rhs.BeginPositionV());
    const auto k = std::min(static_cast<long>(lhs.Size()), static_cast<long>(rhs.Size()));
    const auto fwd = std::min(lH - rH, lV - rV);

    // matchReward = # of anchor bases * matchScore;
    const auto matches = k - std::max(0l, k - fwd);
    const auto matchReward = matches * config.matchScore;

    // nonMatchPenalty = # of non-anchor, on-diagonal bases * nonMatchPenalty
    const auto nonMatches = fwd - matches;
    const auto nonMatchScorePenalty = nonMatches * config.nonMatchPenalty;

    // Ignore any linkage over a certain size, no matter the score
    if (nonMatches > config.maxSeedGap) {
        return -1;
    }

    // indelPenalty = difference in the seed diagonals * indelScore
    const auto diagL = lhs.Diagonal();
    const auto diagR = rhs.Diagonal();
    const auto drift = diagL - diagR;
    long indelScorePenalty = 0;
    if (drift > 0) {
        indelScorePenalty = drift * config.insertionPenalty;
    } else if (drift < 0) {
        indelScorePenalty = -drift * config.deletionPenalty;
    }

    return matchReward + indelScorePenalty + nonMatchScorePenalty;
}

bool IndexCompare(const SDPHit& lhs, const SDPHit& rhs) { return lhs.Index < rhs.Index; }

std::vector<std::optional<SDPHit>> ComputeVisibilityLeft(const std::vector<SDPHit>& seeds,
                                                         std::set<SDPHit>& sweepSet)
{
    std::vector<std::optional<SDPHit>> visible(seeds.size());  // Output

    auto toRemove = seeds.begin();
    for (auto it = seeds.begin(); it != seeds.end();) {
        const auto col = (*it).BeginPositionH();
        const auto start = it;

        // Advance the iterator to the end of the current column in our
        //  column-sorted vector of seeds
        for (; it != seeds.end() && col == (*it).BeginPositionH(); ++it) {
            // For each seed, record in the output vector, record the first
            //  seed after it in the sweepSet (if any).  Since the sweepSet
            //  only contains seeds from previous columns and is sorted by
            //  their diagonals, this means the seeds found in this way will
            //  all (A) Start to the left and (B) Start on a higher diagonal
            auto succ = sweepSet.upper_bound(*it);
            if (succ != sweepSet.end()) {
                visible[it->Index] = *succ;
            }
        }

        // Add all seeds to the sweepSet that start in the current column
        sweepSet.insert(start, it);

        // Remove all seeds from the sweepSet that end before the current column
        for (; toRemove != seeds.end() && (*toRemove).EndPositionH() < col; ++toRemove) {
            sweepSet.erase(*toRemove);
        }
    }

    // clear the sweepSet after use
    sweepSet.clear();

    return visible;
}

void InitializeSeedsAndScores(const Seeds& seedSet, std::vector<SDPHit>* seeds,
                              std::vector<long>* scores)
{
    size_t i = 0;
    for (const auto& seed : seedSet) {
        seeds->push_back(SDPHit(seed, i));
        (*scores)[i] = seed.Size();
        ++i;
    }
}

void ChainSeedsImpl(
    std::priority_queue<ChainHit, std::vector<ChainHit>, ChainHitCompare>* chainHits,
    std::vector<std::optional<size_t>>* chainPred, std::vector<SDPHit>* seeds,
    std::vector<long>& scores, const size_t seedSetIdx, const ChainSeedsConfig& config)
{
    // compute visibility left, requires H-sorted seeds
    std::set<SDPHit> sweepSet;
    sort(seeds->begin(), seeds->end(), HVCompare);
    auto visible = ComputeVisibilityLeft(*seeds, sweepSet);

    // compute the visibility above, requires V-sorted seeds
    sort(seeds->begin(), seeds->end(), VHCompare);
    auto toRemove = seeds->begin();
    std::set<SDPColumn> colSet;

    auto zScore = [&scores](const SDPHit& seed) {
        return scores[seed.Index] + seed.BeginPositionH() + seed.BeginPositionV();
    };

    for (auto it = seeds->begin(); it != seeds->end();) {
        const size_t row = (*it).BeginPositionV();
        const auto start = it;

        for (; it != seeds->end() && row == (*it).BeginPositionV(); ++it) {
            long bestScore = -std::numeric_limits<long>::max();
            std::optional<SDPHit> bestSeed;

            // find the previous column and best fragment from it
            {
                SDPColumn col((*it).BeginPositionH());
                auto pred = colSet.lower_bound(col);

                if (pred != colSet.begin()) {
                    pred = prev(pred);
                    long s = scores[pred->Seed->Index] + LinkScore(*it, *(pred->Seed), config);

                    if (s > bestScore) {
                        bestScore = s;
                        bestSeed = pred->Seed;
                    }
                }
            }

            // search visible fragments (above)
            {
                auto visa = sweepSet.lower_bound(*it);

                if (visa != sweepSet.begin()) {
                    visa = prev(visa);
                    long s = scores[visa->Index] + LinkScore(*it, *visa, config);

                    if (s > bestScore) {
                        bestScore = s;
                        bestSeed = *visa;
                    }
                }
            }

            // search visible fragments (left)
            if (auto visl = visible[it->Index]) {
                long s = scores[visl->Index] + LinkScore(*it, *visl, config);

                if (s > bestScore) {
                    bestScore = s;
                    bestSeed = visl;
                }
            }

            if (bestSeed && bestScore >= config.minScore) {
                scores[it->Index] = bestScore;
                (*chainPred)[it->Index] = bestSeed->Index;

                if (chainHits->size() < config.numCandidates) {
                    chainHits->push({seedSetIdx, it->Index, bestScore});
                } else if (bestScore > chainHits->top().score) {
                    chainHits->pop();
                    chainHits->push({seedSetIdx, it->Index, bestScore});
                }
            } else if (scores[it->Index] >= config.minScore) {
                // PLEASE NOTE: these have already been done at creation time
                // scores[it->Index] = seedSize(*it);
                // chainPred[it->Index] = std::nullopt;
                //

                if (chainHits->size() < config.numCandidates) {
                    chainHits->push({seedSetIdx, it->Index, bestScore});
                } else if (bestScore > chainHits->top().score) {
                    chainHits->pop();
                    chainHits->push({seedSetIdx, it->Index, bestScore});
                }
            }
        }

        sweepSet.insert(start, it);

        // remove all seeds from the sweepSet with end position less than the current row,
        // and ensure the colSet invariant is kept:
        //   that all columns greater than our current
        for (; toRemove != seeds->end() && (*toRemove).EndPositionV() < row; ++toRemove) {
            SDPColumn col((*toRemove).EndPositionH(), std::make_optional(*toRemove));

            auto myIt = colSet.find(col);

            // update the column if it doesn't exist
            // or if its score is less than the fragment we're removing from consideration
            if (myIt == colSet.end() || zScore(*(myIt->Seed)) < zScore(*toRemove)) {
                // insert the updated column, get successor
                myIt = std::next(colSet.insert(col).first);

                // keep removing columns long as the scores are less than
                while (myIt != colSet.end() && zScore(*(myIt->Seed)) < zScore(*toRemove)) {
                    myIt = colSet.erase(myIt);
                }
            }

            sweepSet.erase(*toRemove);
        }
    }

    // seeds need to be sorted by Index to ... index into it properly
    std::sort(seeds->begin(), seeds->end(), IndexCompare);
}

std::vector<std::vector<Seed>> ChainSeeds(const Seeds& seedSet, const ChainSeedsConfig& config)
{
    // Initialize the work-horse vectors we will actually work with
    std::priority_queue<ChainHit, std::vector<ChainHit>, ChainHitCompare> chainHits;
    std::vector<std::optional<size_t>> chainPred(seedSet.size());
    std::vector<SDPHit> seeds;
    std::vector<long> scores(seedSet.size(), 0L);
    InitializeSeedsAndScores(seedSet, &seeds, &scores);

    // Call the main function
    ChainSeedsImpl(&chainHits, &chainPred, &seeds, scores, 0, config);

    // Empty and resize the result vector
    std::vector<std::vector<Seed>> chains;
    chains.resize(chainHits.size());

    // Pop our results from our queue and convert them into Seed Chains / Sets
    int i = chainHits.size() - 1;
    while (!chainHits.empty()) {
        const auto hit = chainHits.top();
        //std::cout << "b(" << hit.reference << ", " << hit.endIndex << ", " << hit.score << ')' << std::endl;

        // While there are additional links in the chain, append them
        std::optional<size_t> chainEnd = hit.endIndex;
        while (chainEnd) {
            chains[i].push_back(seeds[*chainEnd]);
            chainEnd = chainPred[*chainEnd];
        }

        // We appended seeds back-to-front, so reverse the current order in place
        std::reverse(chains[i].begin(), chains[i].end());

        chainHits.pop();
        --i;
    }

    return chains;
}

std::vector<Seeds> ChainedSeedSets(const Seeds& seedSet, const ChainSeedsConfig& config)
{
    // Initialize the work-horse vectors we will actually work with
    std::priority_queue<ChainHit, std::vector<ChainHit>, ChainHitCompare> chainHits;
    std::vector<std::optional<size_t>> chainPred(seedSet.size());
    std::vector<SDPHit> seeds;
    std::vector<long> scores(seedSet.size(), 0L);
    InitializeSeedsAndScores(seedSet, &seeds, &scores);

    // Call the main function
    ChainSeedsImpl(&chainHits, &chainPred, &seeds, scores, 0, config);

    // Empty and resize the result vector
    std::vector<Seeds> chains;
    chains.resize(chainHits.size());

    // Pop our results from our queue and convert them into Seed Chains / Sets
    int i = chainHits.size() - 1;
    while (!chainHits.empty()) {
        const auto hit = chainHits.top();

        // While there are additional links in the chain, append them
        std::optional<size_t> chainEnd = hit.endIndex;
        while (chainEnd) {
            auto seed = seeds[*chainEnd];
            chains[i].AddSeed(seed);
            chainEnd = chainPred[*chainEnd];
        }

        chainHits.pop();
        --i;
    }
    return chains;
}

std::vector<std::pair<size_t, Seeds>> ChainSeeds(const std::map<size_t, Seeds> seedSets,
                                                 const ChainSeedsConfig config)
{
    // The queue will accumulat results across SeedSets
    std::priority_queue<ChainHit, std::vector<ChainHit>, ChainHitCompare> chainHits;

    // Our vectors of seeds and chain-links need to persist for eventual
    //  use reconstructing our chains, so we initialize them as
    //  vectors-of-vectors here, 1 per seedSet we will analyze
    size_t numSeedSets = seedSets.size();
    std::vector<std::vector<std::optional<size_t>>> chainPred(numSeedSets);
    std::vector<std::vector<SDPHit>> seeds(numSeedSets);

    // We also need to record which seedSet came from which reference
    std::vector<size_t> references(numSeedSets);

    // Iterate over the multiple SeedSets once to search for chains
    int i = 0;
    for (auto it = seedSets.begin(); it != seedSets.end(); ++it, ++i) {
        // Extract the contents of our K/V pair
        references[i] = it->first;
        const auto& seedSet = it->second;

        // Initialize the work-horse vectors we will actually work with
        chainPred[i] = std::vector<std::optional<size_t>>(seedSet.size());
        std::vector<long> scores(seedSet.size(), 0L);
        InitializeSeedsAndScores(seedSet, &seeds[i], &scores);

        // Call the main function on the current seedSet
        ChainSeedsImpl(&chainHits, &chainPred[i], &seeds[i], scores, i, config);
    }

    // Empty and resize the result vector
    std::vector<std::pair<size_t, Seeds>> chains;
    chains.resize(chainHits.size());

    // Pop our results from our queue and convert them into Seed Chains / Sets
    int j = chainHits.size() - 1;
    while (!chainHits.empty()) {
        // Take our next hit and extract the relevant indices ...
        const auto hit = chainHits.top();
        chains[j].first = references[hit.seedSetIdx];

        // While there are additional links in the chain, append them
        std::optional<size_t> chainEnd = hit.endIndex;
        while (chainEnd) {
            auto seed = seeds[hit.seedSetIdx][*chainEnd];
            chains[j].second.AddSeed(seed);
            chainEnd = chainPred[hit.seedSetIdx][*chainEnd];
        }

        chainHits.pop();
        --j;
    }

    return chains;
}

}  // namespace Align
}  // namespace PacBio
