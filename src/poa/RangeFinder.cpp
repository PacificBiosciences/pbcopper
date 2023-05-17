#include <pbcopper/poa/RangeFinder.h>

#include "PoaGraphImpl.h"

#include <boost/foreach.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/range/adaptor/reversed.hpp>

#include <algorithm>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <cstddef>

#define WIDTH 30
#define DEBUG_RANGE_FINDER 0

#if DEBUG_RANGE_FINDER
#include <iostream>
#endif  // DEBUG_RANGE_FINDER

namespace PacBio {
namespace Poa {
namespace detail {
namespace {

bool compareAnchorsOnCssPos(const SdpAnchor& a1, const SdpAnchor& a2)
{
    return a1.first < a2.first;
}

const SdpAnchor* binarySearchAnchors(const SdpAnchorVector& anchors, std::size_t cssPosition)
{
    auto found = std::lower_bound(anchors.begin(), anchors.end(), std::make_pair(cssPosition, -1),
                                  compareAnchorsOnCssPos);
    if (found != anchors.end() && (*found).first == cssPosition) {
        return &(*found);
    } else {
        return nullptr;
    }
}

}  // namespace

using IntervalPair = std::pair<int, int>;

std::string formatIntervalEndpoint(int i)
{
    if (i == INT_MAX / 2) {
        return "inf";
    } else if (i == -INT_MAX / 2) {
        return "-inf";
    } else {
        return std::to_string(i);
    }
}

std::string formatInterval(const IntervalPair& ival)
{
    return '[' + formatIntervalEndpoint(ival.first) + ", " + formatIntervalEndpoint(ival.second) +
           ')';
}

// Canonical empty interval....
const IntervalPair emptyInterval = IntervalPair(INT_MAX / 2, -INT_MAX / 2);

IntervalPair RangeUnion(const IntervalPair& range1, const IntervalPair& range2)
{
    return IntervalPair(std::min(range1.first, range2.first),
                        std::max(range1.second, range2.second));
}

IntervalPair RangeUnion(const std::vector<IntervalPair>& ranges)
{
    IntervalPair result = emptyInterval;
    for (const IntervalPair& r : ranges) {
        result = RangeUnion(result, r);
    }
    return result;
}

IntervalPair next(const IntervalPair& v, int upperBound)
{
    if (v == emptyInterval) {
        return emptyInterval;
    } else {
        return IntervalPair(std::min(v.first + 1, upperBound), std::min(v.second + 1, upperBound));
    }
}

IntervalPair prev(const IntervalPair& v, int lowerBound = 0)
{
    if (v == emptyInterval) {
        return emptyInterval;
    } else {
        return IntervalPair(std::max(v.first - 1, lowerBound), std::max(v.second - 1, lowerBound));
    }
}

SdpRangeFinder::~SdpRangeFinder() = default;
void SdpRangeFinder::InitRangeFinder(const PoaGraphImpl& poaGraph,
                                     const std::vector<Vertex>& consensusPath,
                                     const std::string& consensusSequence,
                                     const std::string& readSequence)
{
#if DEBUG_RANGE_FINDER
    poaGraph.WriteGraphVizFile("debug-graph.dot", PoaGraph::VERBOSE_NODES, NULL);
    std::map<Vertex, const SdpAnchor*> anchorByVertex;
#endif
    // Clear prexisting state first!
    alignableReadIntervalByVertex_.clear();

    const int readLength = readSequence.size();
    SdpAnchorVector anchors = FindAnchors(consensusSequence, readSequence);
#if DEBUG_RANGE_FINDER
    std::cout << "Consensus: " << consensusSequence << std::endl;
    std::cout << "Read     : " << readSequence << std::endl;
    std::cout << "RawAnchors length: " << anchors.size() << std::endl;
#endif

    std::map<VD, std::optional<IntervalPair>> directRanges;
    std::map<VD, IntervalPair> fwdMarks, revMarks;

    std::vector<VD> sortedVertices(num_vertices(poaGraph.g_));
    topological_sort(poaGraph.g_, sortedVertices.rbegin());
    for (const VD v : sortedVertices) {
        directRanges[v] = std::nullopt;
    }

    // Find the "direct ranges" implied by the anchors between the
    // css and this read.  Possibly null.
    for (std::size_t cssPos = 0; cssPos < consensusPath.size(); cssPos++) {
        Vertex vExt = consensusPath[cssPos];
        VD v = poaGraph.internalize(vExt);
        const SdpAnchor* anchor = binarySearchAnchors(anchors, cssPos);
        if (anchor != nullptr) {
#if DEBUG_RANGE_FINDER
            anchorByVertex[vExt] = anchor;
#endif
            directRanges[v] = IntervalPair(std::max(int(anchor->second) - WIDTH, 0),
                                           std::min(int(anchor->second) + WIDTH, readLength));
        } else {
            directRanges[v] = std::nullopt;
        }
    }

    // Use the direct ranges as a seed and perform a forward recursion,
    // letting a node with null direct range have a range that is the
    // union of the "forward stepped" ranges of its predecessors
    for (const VD v : sortedVertices) {
        /* Vertex vExt = */ poaGraph.externalize(v);  // DEBUGGING

        std::optional<IntervalPair> directRange = directRanges.at(v);
        if (directRange) {
            fwdMarks[v] = *directRange;
        } else {
            std::vector<IntervalPair> predRangesStepped;
            for (const ED& e : inEdges(v, poaGraph.g_)) {
                VD pred = source(e, poaGraph.g_);
                /* Vertex predExt = */ poaGraph.externalize(pred);  // DEBUGGING
                IntervalPair predRangeStepped = next(fwdMarks.at(pred), readLength);
                predRangesStepped.push_back(predRangeStepped);
            }
            IntervalPair fwdInterval = RangeUnion(predRangesStepped);
            fwdMarks[v] = fwdInterval;
        }
    }

    // Do the same thing, but as a backwards recursion
    for (const VD v : boost::adaptors::reverse(sortedVertices)) {
        /* Vertex vExt = */ poaGraph.externalize(v);  // DEBUGGING

        std::optional<IntervalPair> directRange = directRanges.at(v);
        if (directRange) {
            revMarks[v] = *directRange;
        } else {
            std::vector<IntervalPair> succRangesStepped;
            BOOST_FOREACH (const ED& e, out_edges(v, poaGraph.g_)) {
                VD succ = target(e, poaGraph.g_);
                /* Vertex succExt = */ poaGraph.externalize(succ);  // DEBUGGING
                IntervalPair succRangeStepped = prev(revMarks.at(succ), 0);
                succRangesStepped.push_back(succRangeStepped);
            }
            IntervalPair revInterval = RangeUnion(succRangesStepped);
            revMarks[v] = revInterval;
        }
    }

    // take hulls of extents from forward and reverse recursions
    for (const VD v : sortedVertices) {
        Vertex vExt = poaGraph.externalize(v);
        alignableReadIntervalByVertex_[vExt] = RangeUnion(fwdMarks.at(v), revMarks.at(v));
#if DEBUG_RANGE_FINDER
        cout << vExt << "\t";
        if (anchorByVertex.find(vExt) != anchorByVertex.end()) {
            cout << " @  " << anchorByVertex.at(vExt)->second << "\t";
        } else {
            cout << "\t";
        }
        cout << "Fwd mark: " << formatInterval(fwdMarks[v]) << "\t"
             << "Rev mark: " << formatInterval(revMarks[v]) << "\t"
             << "Range: " << formatInterval(alignableReadIntervalByVertex_[vExt]) << endl;
#endif
    }
}

IntervalPair SdpRangeFinder::FindAlignableRange(Vertex v)
{
    return alignableReadIntervalByVertex_.at(v);
}

}  // namespace detail
}  // namespace Poa
}  // namespace PacBio
