#ifndef PBCOPPER_POA_RANGEFINDER_H
#define PBCOPPER_POA_RANGEFINDER_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/poa/PoaGraph.h>

#include <map>
#include <string>
#include <utility>
#include <vector>

#include <cstddef>

namespace PacBio {
namespace Poa {
namespace detail {

// an Anchor represents a point (cssPos, readPos)
using SdpAnchor = std::pair<std::size_t, std::size_t>;
using SdpAnchorVector = std::vector<SdpAnchor>;

class PoaGraphImpl;

//
// SdpRangeFinder objects are responsible for identifying the range
// of read positions that we should seek to align to a POA vertex;
// this implementation uses SDP to identify fairly narrow bands,
// enabling sparse memory usage.
//
// This is an abstract class that will be inherited in a client
// library that has access to an SDP method.
//
// RangeFinder state goes away on next call to InitRangeFinder.  We could
// have dealt with this using a factory pattern but bleh.
//
class SdpRangeFinder
{
private:
    std::map<PoaGraph::Vertex, std::pair<int, int>> alignableReadIntervalByVertex_;

public:
    virtual ~SdpRangeFinder();

    void InitRangeFinder(const PoaGraphImpl& poaGraph,
                         const std::vector<PoaGraph::Vertex>& consensusPath,
                         const std::string& consensusSequence, const std::string& readSequence);

    // TODO: write contract
    std::pair<int, int> FindAlignableRange(PoaGraph::Vertex v);

protected:
    // TODO: write contract
    virtual SdpAnchorVector FindAnchors(const std::string& consensusSequence,
                                        const std::string& readSequence) const = 0;
};

}  // namespace detail
}  // namespace Poa
}  // namespace PacBio

#endif  // PBCOPPER_POA_RANGEFINDER_H
