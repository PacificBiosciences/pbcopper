#ifndef PBCOPPER_POA_SPARSEPOA_H
#define PBCOPPER_POA_SPARSEPOA_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/data/Interval.h>
#include <pbcopper/poa/PoaConsensus.h>
#include <pbcopper/poa/RangeFinder.h>

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace PacBio {

// fwd decls
namespace Poa {
// fwd decls
class PoaGraph;

using Interval = Data::Interval;
using PoaGraph = Poa::PoaGraph;

class SdpRangeFinder : public Poa::detail::SdpRangeFinder
{
protected:
    virtual Poa::detail::SdpAnchorVector FindAnchors(const std::string& consensusSequence,
                                                     const std::string& readSequence) const final;
};

//
// Details of how a read aligns to the POA graph
//
struct PoaAlignmentSummary
{
    bool ReverseComplementedRead;
    Interval ExtentOnRead;
    Interval ExtentOnConsensus;
    float AlignmentScore;
    float AlignmentIdentity;

    PoaAlignmentSummary()
        : ReverseComplementedRead{false}
        , ExtentOnRead(0, 0)
        , ExtentOnConsensus(0, 0)
        , AlignmentScore{0}
        , AlignmentIdentity{0}
    {}
};

struct PoaAlignmentOptions
{
    PoaAlignmentOptions();
    PoaAlignmentOptions(Align::AlignMode alignMode, float newMinScoreToAdd);
    PoaAlignmentOptions(const Align::AlignConfig& newAlignConfig, float newMinScoreToAdd);

    const Align::AlignConfig alignConfig;
    const float minScoreToAdd;
};

//
// Partial order aligner with parsimonious memory usage
//
class SparsePoa
{
public:
    // ReadKey: Integer key representing
    //  >= 0: a read in the POA graph
    //    -1: read could not be inserted into POA graph
    using ReadKey = int;

public:
    SparsePoa();
    ~SparsePoa();

    //
    // Add read, which must already have been oriented to be in the
    // "forward" convention
    //
    ReadKey AddRead(const std::string& readSequence,
                    const PoaAlignmentOptions& alnOptions = PoaAlignmentOptions());

    //
    // Find better orientation, (fwd or RC) and add as such
    //
    ReadKey OrientAndAddRead(const std::string& readSequence,
                             const PoaAlignmentOptions& alnOptions = PoaAlignmentOptions());

    //
    // Walk the POA and get the optimal consensus path
    //
    std::shared_ptr<const Poa::PoaConsensus> FindConsensus(
        int minCoverage, std::vector<PoaAlignmentSummary>* summaries = NULL) const;

    std::shared_ptr<const PoaConsensus> FindConsensus(int minCoverage,
                                                      std::vector<PoaAlignmentSummary>* summaries,
                                                      const Align::AlignConfig& config) const;

    //
    // Serialize the POA graph to std::string
    //
    std::string ToGraphViz(int flags = 0, const Poa::PoaConsensus* pc = nullptr) const;

    //
    // Serialize the POA graph to a file
    //
    void WriteGraphVizFile(const std::filesystem::path& filename, int flags = 0,
                           const Poa::PoaConsensus* pc = nullptr) const;

    void WriteGraphCsvFile(const std::filesystem::path& filename) const;

    //
    // Clean up the POA graph, pruning minority paths, to speed up
    // successive AddRead operations.
    //
    void PruneGraph(int minCoverage);

private:
    void repCheck();

private:
    using Path = std::vector<PoaGraph::Vertex>;

    std::unique_ptr<PoaGraph> graph_;
    std::vector<Path> readPaths_;
    std::vector<bool> reverseComplemented_;
    std::unique_ptr<SdpRangeFinder> rangeFinder_;
};

}  // namespace Poa
}  // namespace PacBio

#endif  // PBCOPPER_POA_SPARSEPOA_H
