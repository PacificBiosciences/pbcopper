#ifndef PBCOPPER_DAGCON_ALIGNMENTGRAPH_H
#define PBCOPPER_DAGCON_ALIGNMENTGRAPH_H

#include <pbcopper/PbcopperConfig.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_selectors.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>

#include <string>
#include <vector>

namespace PacBio {
namespace Dagcon {

/// Alignment graph representation and consensus caller.  Based on the original
/// Python implementation, pbdagcon.  This class is modelled after its
/// aligngraph.py component, which accumulates alignment information into a
/// partial-order graph and then calls consensus.  Used to error-correct pacbio
/// on pacbio reads.
///
/// Implemented using the boost graph library.

struct Alignment;

using graphTraits = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS>;

///
/// Alignment node, representing one base position in the alignment graph.
///
struct AlignmentNode
{
    /// DNA base: [ACTG]
    char Base = 'N';

    /// Number of reads align to this position, but not necessarily match
    int Coverage = 0;

    /// Number of reads that align to this node *with the same base*, but not
    /// necessarily represented in the target.
    int Weight = 0;

    /// Is this node based on the reference
    bool Backbone = false;

    /// Mark for removed as part of the merging process
    bool Deleted = false;

    /// Best scoring in edge
    graphTraits::edge_descriptor BestInEdge;

    /// Best scoring out edge
    graphTraits::edge_descriptor BestOutEdge;
};

///
/// Edge between alignment nodes.
///
struct AlignmentEdge
{
    /// Number of times this edge was confirmed by an alignment
    int Count = 0;

    // Tracks a visit during algorithm processing
    bool Visited = false;
};

// clang-format off
using GraphType = boost::adjacency_list<boost::vecS, boost::vecS,
                                        boost::bidirectionalS,
                                        AlignmentNode, AlignmentEdge>;
using VertexIndex     = boost::graph_traits<GraphType>::vertex_descriptor;
using VertexIterator  = boost::graph_traits<GraphType>::vertex_iterator;
using EdgeIndex       = boost::graph_traits<GraphType>::edge_descriptor;
using EdgeIterator    = boost::graph_traits<GraphType>::edge_iterator;
using InEdgeIterator  = boost::graph_traits<GraphType>::in_edge_iterator;
using OutEdgeIterator = boost::graph_traits<GraphType>::out_edge_iterator;
using IndexMap        = boost::property_map<GraphType, boost::vertex_index_t>::type;
// clang-format on

///
/// Simple consensus interface data structure
///
struct ConsensusResult
{
    /// Range on the target
    int Range[2];

    /// Consensus fragment
    std::string Seq;
};

///
/// Core alignments into consensus algorithm, implemented using the boost graph
/// library.  Takes a set of alignments to a reference and builds a higher
/// accuracy (~ 99.9) consensus sequence from it.  Designed for use in the HGAP
/// pipeline as a long read error correction step.
///
class AlignmentGraph
{
public:
    /// Initialize graph based on the given sequence. Graph is annotated with
    /// the bases from the backbone.
    ///
    /// \param backbone the reference sequence.
    ///
    explicit AlignmentGraph(const std::string& backbone);

    /// Initialize graph to a given backbone length, filled with 'N'. Base
    /// information is filled in as alignments are added.
    ///
    /// \param blen length of the reference sequence.
    ///
    explicit AlignmentGraph(std::size_t backboneLength);

    /// Add alignment to the graph.
    ///
    /// \param Alignment an alignment record (see Alignment.hpp)
    ///
    void AddAlignment(Alignment& alignment, bool useLocalMerge = false);

    /// Adds a new or increments an existing edge between two aligned bases.
    ///
    /// \param u the 'from' vertex descriptor
    /// \param v the 'to' vertex descriptor
    ///
    void AddEdge(VertexIndex u, VertexIndex v);

    /// Collapses degenerate nodes (vertices).  Must be called before
    /// consensus(). Calls mergeInNodes() followed by mergeOutNodes().
    ///
    void MergeNodes();

    /// Recursive merge of 'in' nodes.
    ///
    /// \param n the base node to merge around.
    ///
    void MergeInNodes(VertexIndex n);

    /// Non-recursive merge of 'out' nodes.
    ///
    /// \param n the base node to merge around.
    ///
    void MergeOutNodes(VertexIndex n);

    /// Mark a given node for removal from graph. Doesn't not modify graph.
    ///
    /// \param n the node to remove.
    ///
    void MarkForReaper(VertexIndex n);

    /// Removes the set of nodes that have been marked.  Modifies graph.
    /// Prohibitively expensive when using vecS as the vertex container.
    ///
    void ReapNodes();

    /// Generates the consensus from the graph.  Must be called after
    /// mergeNodes(). Returns the longest contiguous consensus sequence where
    /// each base meets the minimum weight requirement.
    ///
    /// \param minWeight sets the minimum weight for each base in the consensus.
    ///        default = 0
    ///
    std::string Consensus(int minWeight = 0);

    /// Generates all consensus sequences from a target that meet the minimum
    /// weight requirement.
    ///
    void Consensus(std::vector<ConsensusResult>& seqs, int minWeight = 0,
                   std::size_t minLength = 500);

    /// Generates all consensus sequences from a target that meet the minimum
    /// weight requirement and minimum coverage requirement.
    ///
    void ConsensusWithMinFlankCoverage(std::vector<ConsensusResult>& seqs, int minWeight = 0,
                                       int minFlankCoverage = 0, std::size_t minLen = 500);

    /// Locates the optimal path through the graph.  Called by consensus()
    ///
    std::vector<AlignmentNode> BestPath();

    /// Locate nodes that are missing either in or out edges.
    ///
    bool DanglingNodes();

private:
    GraphType graph_;
    VertexIndex enterVertex_;
    VertexIndex exitVertex_;
    std::map<VertexIndex, VertexIndex> bbMap_;
    std::vector<VertexIndex> reaperBag_;
    std::uint64_t backboneLength_;
};

}  // namespace Dagcon
}  // namespace PacBio

#endif  // PBCOPPER_DAGCON_ALIGNMENTGRAPH_H
