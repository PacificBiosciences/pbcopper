#ifndef PBCOPPER_POA_POACONSENSUS_H
#define PBCOPPER_POA_POACONSENSUS_H

#include <pbcopper/PbcopperConfig.h>

#include <climits>

#include <string>
#include <utility>
#include <vector>

#include <boost/utility.hpp>

#include <pbcopper/align/AlignConfig.h>

#include <pbcopper/poa/PoaGraph.h>

namespace PacBio {
namespace Poa {

class PoaGraph;
class PoaGraphPath;
class ScoredMutation;

Align::AlignConfig DefaultPoaConfig(Align::AlignMode mode = Align::AlignMode::GLOBAL);

/// \brief A multi-sequence consensus obtained from a partial-order alignment
struct PoaConsensus : private boost::noncopyable
{
    const std::string Sequence;
    PoaGraph Graph;
    std::vector<PoaGraph::Vertex> Path;

    PoaConsensus(std::string css, const PoaGraph& g, std::vector<PoaGraph::Vertex> ConsensusPath);

    // NB: this constructor exists to provide a means to avoid an unnecessary
    // copy of the
    // boost graph.  If we had move semantics (C++11) we would be able to get by
    // without
    // this.
    PoaConsensus(std::string css, const detail::PoaGraphImpl& g,
                 std::vector<PoaGraph::Vertex> ConsensusPath);

    static const PoaConsensus* FindConsensus(const std::vector<std::string>& reads);

    static const PoaConsensus* FindConsensus(const std::vector<std::string>& reads,
                                             const Align::AlignConfig& config,
                                             int minCoverage = -INT_MAX);

    static const PoaConsensus* FindConsensus(const std::vector<std::string>& reads,
                                             Align::AlignMode mode, int minCoverage = -INT_MAX);

public:
    // Additional accessors, which do things on the graph/graphImpl
    // LikelyVariants

public:
    std::string ToGraphViz(int flags = 0) const;

    void WriteGraphVizFile(std::string filename, int flags = 0) const;
};

}  // namespace Poa
}  // namespace PacBio

#endif  // PBCOPPER_POA_POACONSENSUS_H
