#include <pbcopper/align/FindSeeds.h>

#include <pbcopper/qgram/Index.h>

#include "FilterHomopolymers.h"

namespace PacBio {
namespace Align {

std::map<size_t, Seeds> FindSeeds(const PacBio::QGram::Index& index, const std::string& seq,
                                  const std::optional<size_t> qIdx, const bool filterHomopolymers)
{
    std::map<size_t, Seeds> seeds;

    for (const auto& hits : index.Hits(seq, filterHomopolymers)) {
        const auto queryPos = hits.QueryPosition();
        for (const auto& hit : hits) {
            const auto rIdx = hit.Id();
            if (qIdx && rIdx == *qIdx) {
                continue;
            }

            const auto seed = Seed{queryPos, hit.Position(), index.Size()};
            auto& rIdxSeeds = seeds[rIdx];
#ifdef MERGESEEDS
            if (!rIdxSeeds.TryMerge(seed))
#endif
            {
                rIdxSeeds.AddSeed(seed);
            }
        }
    }

    return seeds;
}

std::map<size_t, Seeds> FindSeeds(const PacBio::QGram::Index& index, const std::string& seq,
                                  const std::optional<size_t> qIdx)
{
    return FindSeeds(index, seq, qIdx, Default::FilterHomopolymers);
}

std::map<size_t, Seeds> FindSeeds(const PacBio::QGram::Index& index, const std::string& seq,
                                  const bool filterHomopolymers)
{
    return FindSeeds(index, seq, std::nullopt, filterHomopolymers);
}

std::map<size_t, Seeds> FindSeeds(const PacBio::QGram::Index& index, const std::string& seq)
{
    return FindSeeds(index, seq, std::nullopt, Default::FilterHomopolymers);
}

Seeds FindSeeds(const size_t qGramSize, const std::string& seq1, const std::string& seq2,
                const bool filterHomopolymers)
{
    if (seq2.length() < qGramSize) {
        return Seeds{};
    }
    const auto index = PacBio::QGram::Index{qGramSize, seq2};
    const auto multiSeeds = FindSeeds(index, seq1, filterHomopolymers);
    return (multiSeeds.empty() ? Seeds{} : multiSeeds.cbegin()->second);
}

Seeds FindSeeds(const size_t qGramSize, const std::string& seq1, const std::string& seq2)
{
    return FindSeeds(qGramSize, seq1, seq2, Default::FilterHomopolymers);
}

}  // namespace Align
}  // namespace PacBio
