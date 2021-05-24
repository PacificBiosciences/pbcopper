// Authors: Brett Bowman, Derek Barnett

#include <pbcopper/align/SparseAlignment.h>

#include <pbcopper/align/ChainSeeds.h>
#include <pbcopper/align/FindSeeds.h>
#include <pbcopper/utility/SequenceUtils.h>

#include "FilterHomopolymers.h"

namespace PacBio {
namespace Align {

std::vector<Seed> SparseAlignSeeds(const size_t qGramSize, const std::string& seq1,
                                   const std::string& seq2, const bool filterHomopolymers)
{
    const auto seeds = FindSeeds(qGramSize, seq1, seq2, filterHomopolymers);
    const auto chains = ChainSeeds(seeds, ChainSeedsConfig{});
    if (chains.empty()) {
        return std::vector<Seed>{};
    }
    return chains[0];
}

std::vector<Seed> SparseAlignSeeds(const size_t qGramSize, const std::string& seq1,
                                   const std::string& seq2)
{
    return SparseAlignSeeds(qGramSize, seq1, seq2, Default::FilterHomopolymers);
}

std::pair<size_t, std::vector<Seed>> BestSparseAlign(const std::string& seq1,
                                                     const std::string& seq2,
                                                     const bool filterHomopolymers)
{
    const auto seq2rc = PacBio::Utility::ReverseComplemented(seq2);

    const auto fwd = SparseAlignSeeds(10, seq1, seq2, filterHomopolymers);
    const auto rev = SparseAlignSeeds(10, seq1, seq2rc, filterHomopolymers);

    if (fwd.size() > rev.size()) {
        return std::make_pair(0, fwd);
    }
    return std::make_pair(1, rev);
}

std::pair<size_t, std::vector<Seed>> BestSparseAlign(const std::string& seq1,
                                                     const std::string& seq2)
{
    return BestSparseAlign(seq1, seq2, Default::FilterHomopolymers);
}

std::vector<std::pair<size_t, size_t>> SparseAlign(const size_t qGramSize, const std::string& seq1,
                                                   const std::string& seq2,
                                                   const bool filterHomopolymers)
{
    std::vector<std::pair<size_t, size_t>> result;

    const auto chain = SparseAlignSeeds(qGramSize, seq1, seq2, filterHomopolymers);
    for (const auto& s : chain) {
        result.emplace_back(s.BeginPositionH(), s.BeginPositionV());
    }

    return result;
}

std::vector<std::pair<size_t, size_t>> SparseAlign(const size_t qGramSize, const std::string& seq1,
                                                   const std::string& seq2)
{
    return SparseAlign(qGramSize, seq1, seq2, Default::FilterHomopolymers);
}

}  // namespace Align
}  // namespace PacBio
