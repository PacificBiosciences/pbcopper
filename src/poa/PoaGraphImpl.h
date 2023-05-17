#ifndef PBCOPPER_POA_POAGRAPHIMPL_H
#define PBCOPPER_POA_POAGRAPHIMPL_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/align/AlignConfig.h>
#include <pbcopper/poa/PoaGraph.h>
#include "BoostGraph.h"
#include "PoaAlignmentMatrix.h"

#include <boost/config.hpp>
#include <boost/format.hpp>
#include <boost/type_traits.hpp>

#include <filesystem>
#include <string>
#include <vector>

#include <cfloat>
#include <climits>
#include <cstddef>

///
///  Boost graph library typedefs, properties, and graphviz output.
///

namespace PacBio {
namespace Poa {
namespace detail {

// FWD
class SdpRangeFinder;

class PoaNode
{
public:
    PoaGraph::Vertex Id;
    char Base;
    int Reads;
    // move the below out of here?
    int SpanningReads;
    float Score;
    float ReachingScore;

    PoaNode() : PoaNode(0, 'N', 0, 0) {}
    explicit PoaNode(std::size_t id, char base) : PoaNode(id, base, 1, 0) {}
    explicit PoaNode(std::size_t id, char base, int reads) : PoaNode(id, base, reads, 0) {}
    explicit PoaNode(std::size_t id, char base, int reads, int spanning)
        : Id{id}, Base{base}, Reads{reads}, SpanningReads{spanning}, Score{0}, ReachingScore{0}
    {}
};

// External-facing vertex id type
using Vertex = std::size_t;

struct EdgeComparator
{
public:
    explicit EdgeComparator(const BoostGraph& g) : g_(g) {}

    // want lex. comparison... just using pair to get it..
    bool operator()(ED e1, ED e2) const noexcept
    {
        std::pair<int, int> vt1;
        std::pair<int, int> vt2;
        vt1 = std::make_pair(get(boost::vertex_index, g_, source(e1, g_)),
                             get(boost::vertex_index, g_, target(e1, g_)));
        vt2 = std::make_pair(get(boost::vertex_index, g_, source(e2, g_)),
                             get(boost::vertex_index, g_, target(e2, g_)));
        return (vt1 < vt2);
    }

private:
    const BoostGraph& g_;
};

inline std::vector<ED> inEdges(VD v, const BoostGraph& g)
{
    // This is a sad workaround the nondeterministic order of iteration
    // from BGL's in_edges. (see: http://stackoverflow.com/questions/30968690/)

    // Unfortunately, we can't just use the boost::sort range adapter
    // because it requires an underlying random access iterator, which
    // we can't get from the std::set container.
    boost::graph_traits<BoostGraph>::in_edge_iterator begin;
    boost::graph_traits<BoostGraph>::in_edge_iterator end;
    std::tie(begin, end) = in_edges(v, g);
    std::vector<ED> tmp(begin, end);
    std::sort(tmp.begin(), tmp.end(), EdgeComparator(g));
    return tmp;
}

class PoaGraphImpl
{
    friend class SdpRangeFinder;

    BoostGraph g_;
    VertexInfoMap vertexInfoMap_;  // NB: this is a reference type and refers to
                                   // an "internal" property map
    index_map_t indexMap_;
    VD enterVertex_;
    VD exitVertex_;
    std::size_t numReads_;
    std::size_t totalVertices_;          // includes "ex"-vertices which have since been removed
    std::size_t liveVertices_;           // vertices that are in the graph.  this is needed
                                         // for algorithms.
    std::map<Vertex, VD> vertexLookup_;  // external ID -> internal ID

    void repCheck() const;

    VD addVertex(char base, int nReads = 1, int spanningReads = 0)
    {
        VD vd = add_vertex(g_);
        Vertex vExt = totalVertices_;
        ++totalVertices_;
        vertexInfoMap_[vd] = PoaNode(vExt, base, nReads, spanningReads);
        vertexLookup_[vExt] = vd;
        indexMap_[vd] = liveVertices_++;
        return vd;
    }

    //
    // utility routines
    //
    std::unique_ptr<const AlignmentColumn> makeAlignmentColumn(
        VD v, const AlignmentColumnMap& alignmentColumnForVertex, const std::string& sequence,
        const Align::AlignConfig& config, int beginRow, int endRow) const;

    std::unique_ptr<const AlignmentColumn> makeAlignmentColumnForExit(
        VD v, const AlignmentColumnMap& alignmentColumnForVertex, const std::string& sequence,
        const Align::AlignConfig& config) const;

public:
    //
    // Vertex id translation
    //

    Vertex externalize(VD vd) const
    {
        if (vd == null_vertex) {
            return PoaGraph::NullVertex;
        } else {
            return vertexInfoMap_[vd].Id;
        }
    }

    VD internalize(Vertex vertex) const
    {
        if (vertex == PoaGraph::NullVertex) {
            return null_vertex;
        }
        return vertexLookup_.at(vertex);
    }

    std::vector<Vertex> externalizePath(const std::vector<VD>& vds) const
    {
        std::vector<Vertex> out(vds.size(), 0);
        for (std::size_t i = 0; i < vds.size(); i++) {
            out[i] = externalize(vds[i]);
        }
        return out;
    }

    std::vector<VD> internalizePath(const std::vector<Vertex>& vertices) const
    {
        std::vector<VD> out(vertices.size(), null_vertex);
        for (std::size_t i = 0; i < vertices.size(); i++) {
            out[i] = internalize(vertices[i]);
        }
        return out;
    }

    //
    // POA node lookup
    //
    const PoaNode& getPoaNode(VD v) const { return vertexInfoMap_[v]; }

    //
    // Graph traversal functions, defined in PoaGraphTraversals
    //

    std::vector<VD> sortedVertices() const;

    void tagSpan(VD start, VD end);

    std::vector<VD> consensusPath(Align::AlignMode mode, int minCoverage = -INT_MAX) const;

    void threadFirstRead(std::string sequence, std::vector<Vertex>* readPathOutput = NULL);

    void tracebackAndThread(std::string sequence,
                            const AlignmentColumnMap& alignmentColumnForVertex,
                            Align::AlignMode mode, std::vector<Vertex>* readPathOutput = NULL);

    PoaGraphImpl();
    PoaGraphImpl(const PoaGraphImpl& other);

    void AddRead(const std::string& sequence, const Align::AlignConfig& config,
                 SdpRangeFinder* rangeFinder = NULL, std::vector<Vertex>* readPathOutput = NULL);

    void AddFirstRead(const std::string& sequence, std::vector<Vertex>* readPathOutput = NULL);

    std::unique_ptr<PoaAlignmentMatrix> TryAddRead(const std::string& sequence,
                                                   const Align::AlignConfig& config,
                                                   SdpRangeFinder* const rangeFinder = NULL) const;

    void CommitAdd(const PoaAlignmentMatrix* const mat, std::vector<Vertex>* readPathOutput = NULL);

    std::unique_ptr<PoaConsensus> FindConsensus(const Align::AlignConfig& config,
                                                int minCoverage = -INT_MAX);
    void PruneGraph(const int minCoverage);

    std::size_t NumReads() const;
    std::string ToGraphViz(int flags, const PoaConsensus* pc) const;
    void WriteGraphVizFile(const std::filesystem::path& filename, int flags,
                           const PoaConsensus* pc) const;
    void WriteGraphCsvFile(const std::filesystem::path& filename) const;
};

// free functions, we should put these all in traversals
std::string sequenceAlongPath(const BoostGraph& g, const VertexInfoMap& vertexInfoMap,
                              const std::vector<VD>& path);

}  // namespace detail
}  // namespace Poa
}  // namespace PacBio

#endif  // PBCOPPER_POA_POAGRAPHIMPL_H
