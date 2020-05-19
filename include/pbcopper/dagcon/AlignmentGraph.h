#ifndef PBCOPPER_DAGCON_ALIGNMENTGRAPH_H
#define PBCOPPER_DAGCON_ALIGNMENTGRAPH_H

#include <pbcopper/PbcopperConfig.h>

#include <string>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_selectors.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>

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
/// Graph vertex property. An alignment node, which represents one base position
/// in the alignment graph.
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
/// Graph edge property. Represents an edge between alignment nodes.
///
struct AlignmentEdge
{
    /// Number of times this edge was confirmed by an alignment
    int Count = 0;

    // Tracks a visit during algorithm processing
    bool Visited = false;
};

// Boost-related typedefs
// XXX: listS, listS?
using G = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, AlignmentNode,
                                AlignmentEdge>;
using VtxDesc = boost::graph_traits<G>::vertex_descriptor;
using VtxIter = boost::graph_traits<G>::vertex_iterator;
using EdgeDesc = boost::graph_traits<G>::edge_descriptor;
using EdgeIter = boost::graph_traits<G>::edge_iterator;
using InEdgeIter = boost::graph_traits<G>::in_edge_iterator;
using OutEdgeIter = boost::graph_traits<G>::out_edge_iterator;
using IndexMap = boost::property_map<G, boost::vertex_index_t>::type;

///
/// Simple consensus interface datastructure
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
    AlignmentGraph(const std::string& backbone);

    /// Initialize graph to a given backbone length. Base information is filled
    /// in as alignments are added.
    ///
    /// \param blen length of the reference sequence.
    ///
    AlignmentGraph(const size_t backboneLength);

    /// Add alignment to the graph.
    ///
    /// \param Alignment an alignment record (see Alignment.hpp)
    ///
    void AddAln(Alignment& alignmnet, bool useLocalMerge = false);

    /// Adds a new or increments an existing edge between two aligned bases.
    ///
    /// \param u the 'from' vertex descriptor
    /// \param v the 'to' vertex descriptor
    ///
    void AddEdge(VtxDesc u, VtxDesc v);

    /// Collapses degenerate nodes (vertices).  Must be called before
    /// consensus(). Calls mergeInNodes() followed by mergeOutNodes().
    ///
    void MergeNodes();

    /// Recursive merge of 'in' nodes.
    ///
    /// \param n the base node to merge around.
    ///
    void MergeInNodes(VtxDesc node);

    /// Non-recursive merge of 'out' nodes.
    ///
    /// \param n the base node to merge around.
    ///
    void MergeOutNodes(VtxDesc node);

    /// Mark a given node for removal from graph. Doesn't not modify graph.
    ///
    /// \param n the node to remove.
    ///
    void MarkForReaper(VtxDesc node);

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
    void Consensus(std::vector<ConsensusResult>& seqs, int minWeight = 0, size_t minLength = 500);

    /// Generates all consensus sequences from a target that meet the minimum
    /// weight requirement and minimum coverage requirement.
    ///
    void ConsensusWithMinFlankCoverage(std::vector<ConsensusResult>& seqs, int minWeight = 0,
                                       int minFlankCoverage = 0, size_t minLen = 500);

    /// Locates the optimal path through the graph.  Called by consensus()
    ///
    std::vector<AlignmentNode> BestPath();

    /// Emits the current graph, in dot format, to out stream.
    ///
    void PrintGraph(std::ostream& out);

    /// Locate nodes that are missing either in or out edges.
    ///
    bool DanglingNodes();

private:
    G graph_;
    VtxDesc enterVtx_;
    VtxDesc exitVtx_;
    std::map<VtxDesc, VtxDesc> bbMap_;
    std::vector<VtxDesc> reaperBag_;
    int64_t backboneLength_;
};

}  // namespace Dagcon
}  // namespace PacBio

#endif  // PBCOPPER_DAGCON_ALIGNMENTGRAPH_H
