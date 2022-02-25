#include <pbcopper/poa/SparsePoa.h>

#include <pbcopper/align/AlignConfig.h>
#include <pbcopper/align/ChainSeeds.h>
#include <pbcopper/align/ChainSeedsConfig.h>
#include <pbcopper/align/FindSeeds.h>
#include <pbcopper/align/Seed.h>
#include <pbcopper/align/Seeds.h>
#include <pbcopper/align/SparseAlignment.h>
#include <pbcopper/logging/Logging.h>
#include <pbcopper/poa/PoaGraph.h>
#include <pbcopper/utility/SequenceUtils.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <cstddef>

namespace PacBio {
namespace Poa {
namespace {
std::vector<Align::Seed> SparseAlignSeedsUnySpecific(const size_t qGramSize,
                                                     const std::string& seq1,
                                                     const std::string& seq2)
{
    const auto config = Align::ChainSeedsConfig{1, 1, 3, -1, -1, -1, INT_MAX};
    const auto seeds = Align::FindSeeds(qGramSize, seq1, seq2, true);
    const auto chains = Align::ChainSeeds(seeds, config);
    if (chains.empty()) {
        return std::vector<Align::Seed>{};
    }
    return chains[0];
}

std::vector<std::pair<size_t, size_t>> SparseAlignUnySpecific(const size_t qGramSize,
                                                              const std::string& seq1,
                                                              const std::string& seq2)
{
    std::vector<std::pair<size_t, size_t>> result;
    const auto chain = SparseAlignSeedsUnySpecific(qGramSize, seq1, seq2);
    for (const auto& s : chain) {
        result.emplace_back(s.BeginPositionH(), s.BeginPositionV());
    }
    return result;
}
}  // namespace

PoaAlignmentOptions::PoaAlignmentOptions()
    : alignConfig(Poa::DefaultPoaConfig(Align::AlignMode::LOCAL)), minScoreToAdd(0.0f)
{}

PoaAlignmentOptions::PoaAlignmentOptions(Align::AlignMode alignMode, float newMinScoreToAdd)
    : alignConfig(Poa::DefaultPoaConfig(alignMode)), minScoreToAdd(newMinScoreToAdd)
{}

PoaAlignmentOptions::PoaAlignmentOptions(const Align::AlignConfig& newAlignConfig,
                                         float newMinScoreToAdd)
    : alignConfig(newAlignConfig), minScoreToAdd(newMinScoreToAdd)
{}

detail::SdpAnchorVector SdpRangeFinder::FindAnchors(const std::string& consensusSequence,
                                                    const std::string& readSequence) const
{
    return SparseAlignUnySpecific(6, consensusSequence, readSequence);
}

SparsePoa::SparsePoa()
    : graph_(std::make_unique<PoaGraph>())
    , readPaths_()
    , reverseComplemented_()
    , rangeFinder_(std::make_unique<SdpRangeFinder>())
{}

SparsePoa::~SparsePoa() = default;

SparsePoa::ReadKey SparsePoa::AddRead(const std::string& readSequence,
                                      const PoaAlignmentOptions& alnOptions)
{
    Path outputPath;
    ReadKey key = -1;

    if (graph_->NumReads() == 0) {
        graph_->AddFirstRead(readSequence, &outputPath);
        readPaths_.push_back(outputPath);
        reverseComplemented_.push_back(false);
        key = graph_->NumReads() - 1;
    } else {
        std::unique_ptr<Poa::PoaAlignmentMatrix> c{
            graph_->TryAddRead(readSequence, alnOptions.alignConfig, rangeFinder_.get())};

        if (c->Score() >= alnOptions.minScoreToAdd) {
            graph_->CommitAdd(c.get(), &outputPath);
            readPaths_.push_back(outputPath);
            reverseComplemented_.push_back(false);
            key = graph_->NumReads() - 1;
        }
    }

    return key;
}

namespace TEST {

char Complement(const char base)
{
    constexpr const std::array<char, 256> lookupTable{
        {/*   0 -   7: */ 0,   0,   0,   0,   0,   0,   0,   0,
         /*   8 -  15: */ 0,   0,   0,   0,   0,   0,   0,   0,
         /*  16 -  23: */ 0,   0,   0,   0,   0,   0,   0,   0,
         /*  24 -  31: */ 0,   0,   0,   0,   0,   0,   0,   0,
         /*  32 -  39: */ 0,   0,   0,   0,   0,   0,   0,   0,
         /*  40 -  47: */ 0,   0,   0,   0,   0,   '-', 0,   0,
         /*  48 -  55: */ 0,   0,   0,   0,   0,   0,   0,   0,
         /*  56 -  63: */ 0,   0,   0,   0,   0,   0,   0,   0,

         /*  64 -  71: */ 0,   'T', 'V', 'G', 'H', 0,   0,   'C',
         /*  72 -  79: */ 'D', 0,   0,   'M', 0,   'K', 'N', 0,
         /*  80 -  87: */ 0,   0,   'Y', 'S', 'A', 0,   'B', 'W',
         /*  88 -  95: */ 0,   'R', 0,   0,   0,   0,   0,   0,

         /*  96 - 103: */ 0,   't', 'v', 'g', 'h', 0,   0,   'c',
         /* 104 - 111: */ 'd', 0,   0,   'm', 0,   'k', 'n', 0,
         /* 112 - 119: */ 0,   0,   'y', 's', 'a', 0,   'b', 'w',
         /* 120 - 127: */ 0,   'r', 0,   0,   0,   0,   0,   0,

         /* 128 - 135: */ 0,   0,   0,   0,   0,   0,   0,   0,
         /* 136 - 143: */ 0,   0,   0,   0,   0,   0,   0,   0,
         /* 144 - 151: */ 0,   0,   0,   0,   0,   0,   0,   0,
         /* 152 - 159: */ 0,   0,   0,   0,   0,   0,   0,   0,
         /* 160 - 167: */ 0,   0,   0,   0,   0,   0,   0,   0,
         /* 168 - 175: */ 0,   0,   0,   0,   0,   0,   0,   0,
         /* 176 - 183: */ 0,   0,   0,   0,   0,   0,   0,   0,
         /* 184 - 191: */ 0,   0,   0,   0,   0,   0,   0,   0,
         /* 192 - 199: */ 0,   0,   0,   0,   0,   0,   0,   0,
         /* 200 - 207: */ 0,   0,   0,   0,   0,   0,   0,   0,
         /* 208 - 215: */ 0,   0,   0,   0,   0,   0,   0,   0,
         /* 216 - 223: */ 0,   0,   0,   0,   0,   0,   0,   0,
         /* 224 - 231: */ 0,   0,   0,   0,   0,   0,   0,   0,
         /* 232 - 239: */ 0,   0,   0,   0,   0,   0,   0,   0,
         /* 240 - 247: */ 0,   0,   0,   0,   0,   0,   0,   0,
         /* 248 - 255: */ 0,   0,   0,   0,   0,   0,   0,   0}};

    const char result = lookupTable[static_cast<int>(base)];

    if (result == 0) {
        throw std::invalid_argument(base + std::string{" is an invalid base!"});
    }

    return result;
}

std::string Complement(const std::string& input)
{
    std::string output;
    output.reserve(input.length());
    for (const char b : input) {
        output.push_back(Complement(b));
    }
    return output;
}

std::string Reverse(const std::string& input)
{
    std::string output;
    output.reserve(input.length());
    for (auto it = input.crbegin(); it != input.crend(); ++it) {
        output.push_back(*it);
    }
    return output;
}

std::string ReverseComplement(const std::string& input)
{
    std::string output;
    output.reserve(input.length());
    for (auto it = input.crbegin(); it != input.crend(); ++it) {
        output.push_back(Complement(*it));
    }
    return output;
}
}  // namespace TEST

SparsePoa::ReadKey SparsePoa::OrientAndAddRead(const std::string& readSequence,
                                               const PoaAlignmentOptions& alnOptions)
{
    Path outputPath;
    ReadKey key;

    if (graph_->NumReads() == 0) {
        graph_->AddFirstRead(readSequence, &outputPath);
        readPaths_.push_back(outputPath);
        reverseComplemented_.push_back(false);
        key = graph_->NumReads() - 1;
    } else {
        std::unique_ptr<Poa::PoaAlignmentMatrix> c1{
            graph_->TryAddRead(readSequence, alnOptions.alignConfig, rangeFinder_.get())};
        std::unique_ptr<Poa::PoaAlignmentMatrix> c2{graph_->TryAddRead(
            TEST::ReverseComplement(readSequence), alnOptions.alignConfig, rangeFinder_.get())};

        if (c1->Score() >= c2->Score() && c1->Score() >= alnOptions.minScoreToAdd) {
            graph_->CommitAdd(c1.get(), &outputPath);
            readPaths_.push_back(outputPath);
            reverseComplemented_.push_back(false);
            key = graph_->NumReads() - 1;
        } else if (c2->Score() >= c1->Score() && c2->Score() >= alnOptions.minScoreToAdd) {
            graph_->CommitAdd(c2.get(), &outputPath);
            readPaths_.push_back(outputPath);
            reverseComplemented_.push_back(true);
            key = graph_->NumReads() - 1;
        } else {
            key = -1;
        }
    }
    return key;
}

std::shared_ptr<const PoaConsensus> SparsePoa::FindConsensus(
    int minCoverage, std::vector<PoaAlignmentSummary>* summaries) const
{
    Align::AlignConfig config = DefaultPoaConfig(Align::AlignMode::LOCAL);
    return FindConsensus(minCoverage, summaries, config);
}

std::shared_ptr<const PoaConsensus> SparsePoa::FindConsensus(
    int minCoverage, std::vector<PoaAlignmentSummary>* summaries,
    const Align::AlignConfig& config) const
{
    std::shared_ptr<const PoaConsensus> pc(graph_->FindConsensus(config, minCoverage));

    if (summaries != nullptr) {
        summaries->clear();

        // digest the consensus path consensus into map(vtx, pos)
        // the fold over the readPaths
        std::map<PoaGraph::Vertex, size_t> cssPosition;

        int i = 0;
        for (PoaGraph::Vertex v : pc->Path) {
            cssPosition[v] = i;
            i++;
        }

        for (size_t readId = 0; readId < graph_->NumReads(); readId++) {
            size_t readS = 0;
            size_t readE = 0;
            size_t cssS = 0;
            size_t cssE = 0;
            bool foundStart = false;
            size_t nErr = 0;

            const std::vector<PoaGraph::Vertex>& readPath = readPaths_[readId];

            for (size_t readPos = 0; readPos < readPath.size(); readPos++) {
                PoaGraph::Vertex v = readPath[readPos];
                if (cssPosition.find(v) != cssPosition.end()) {
                    if (!foundStart) {
                        cssS = cssPosition[v];
                        readS = readPos;
                        foundStart = true;
                    }

                    cssE = cssPosition[v] + 1;
                    readE = readPos + 1;
                } else {
                    nErr += 1;
                }
            }

            Interval readExtent(readS, readE);
            Interval cssExtent(cssS, cssE);

            PoaAlignmentSummary summary;
            summary.ReverseComplementedRead = reverseComplemented_[readId];
            summary.ExtentOnRead = readExtent;
            summary.ExtentOnConsensus = cssExtent;
            summary.AlignmentIdentity = std::max(0.0F, 1.0F - 1.0F * nErr / cssPosition.size());

            (*summaries).push_back(summary);
        }
    }

    return pc;
}

std::string SparsePoa::ToGraphViz(int flags, const PoaConsensus* pc) const
{
    return graph_->ToGraphViz(flags, pc);
}

void SparsePoa::WriteGraphVizFile(const std::string& filename, int flags,
                                  const PoaConsensus* pc) const
{
    graph_->WriteGraphVizFile(filename, flags, pc);
}

void SparsePoa::WriteGraphCsvFile(const std::string& filename) const
{
    graph_->WriteGraphCsvFile(filename);
}

void SparsePoa::PruneGraph(const int minCoverage) { graph_->PruneGraph(minCoverage); }

void SparsePoa::repCheck()
{
    assert(graph_->NumReads() == readPaths_.size());
    assert(graph_->NumReads() == reverseComplemented_.size());
}

}  // namespace Poa
}  // namespace PacBio
