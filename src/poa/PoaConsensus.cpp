#include <pbcopper/poa/PoaConsensus.h>

#include <pbcopper/align/AlignConfig.h>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

#include <string>
#include <utility>
#include <vector>

namespace PacBio {
namespace Poa {

Align::AlignConfig DefaultPoaConfig(Align::AlignMode mode)
{
    Align::AlignParams params{3, -5, -4, -4};
    Align::AlignConfig config{params, mode};
    return config;
}

PoaConsensus::PoaConsensus(std::string css, const PoaGraph& g, std::vector<size_t> cssPath)
    : Sequence(std::move(css)), Graph(g), Path(std::move(cssPath))
{}

PoaConsensus::PoaConsensus(std::string css, const detail::PoaGraphImpl& gi,
                           std::vector<size_t> cssPath)
    : Sequence(std::move(css)), Graph(gi), Path(std::move(cssPath))
{}

const PoaConsensus* PoaConsensus::FindConsensus(const std::vector<std::string>& reads)
{
    return FindConsensus(reads, DefaultPoaConfig(Align::AlignMode::GLOBAL), -INT_MAX);
}

const PoaConsensus* PoaConsensus::FindConsensus(const std::vector<std::string>& reads,
                                                const Align::AlignConfig& config, int minCoverage)
{
    PoaGraph pg;
    for (const std::string& read : reads) {
        if (read.length() == 0) {
            throw std::invalid_argument("input sequences must have nonzero length.");
        }
        pg.AddRead(read, config);
    }
    return pg.FindConsensus(config, minCoverage);
}

const PoaConsensus* PoaConsensus::FindConsensus(const std::vector<std::string>& reads,
                                                Align::AlignMode mode, int minCoverage)
{
    return FindConsensus(reads, DefaultPoaConfig(mode), minCoverage);
}

std::string PoaConsensus::ToGraphViz(int flags) const { return Graph.ToGraphViz(flags, this); }

void PoaConsensus::WriteGraphVizFile(std::string filename, int flags) const
{
    Graph.WriteGraphVizFile(filename, flags, this);
}

}  // namespace Poa
}  // namespace PacBio
