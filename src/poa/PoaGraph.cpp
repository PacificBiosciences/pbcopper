// (Based on the original "Partial Order Aligner" by Lee, Grasso, and Sharlow,
//  and an implementation in C# by Patrick Marks)

#include <pbcopper/poa/PoaGraph.h>

#include <pbcopper/poa/PoaConsensus.h>
#include "PoaGraphImpl.h"

namespace PacBio {
namespace Poa {

//
// PIMPL idiom delegation
//
void PoaGraph::AddRead(const std::string& sequence, const Align::AlignConfig& config,
                       detail::SdpRangeFinder* rangeFinder, std::vector<Vertex>* readPathOutput)
{
    impl_->AddRead(sequence, config, rangeFinder, readPathOutput);
}

void PoaGraph::AddFirstRead(const std::string& sequence, std::vector<Vertex>* readPathOutput)
{
    impl_->AddFirstRead(sequence, readPathOutput);
}

PoaAlignmentMatrix* PoaGraph::TryAddRead(const std::string& sequence,
                                         const Align::AlignConfig& config,
                                         detail::SdpRangeFinder* const rangeFinder) const
{
    return impl_->TryAddRead(sequence, config, rangeFinder).release();
}

void PoaGraph::CommitAdd(PoaAlignmentMatrix* mat, std::vector<Vertex>* readPathOutput)
{
    impl_->CommitAdd(mat, readPathOutput);
}

void PoaGraph::PruneGraph(const int minCoverage) { impl_->PruneGraph(minCoverage); }

std::size_t PoaGraph::NumReads() const { return impl_->NumReads(); }

const PoaConsensus* PoaGraph::FindConsensus(const Align::AlignConfig& config, int minCoverage) const
{
    return impl_->FindConsensus(config, minCoverage).release();
}

std::string PoaGraph::ToGraphViz(int flags, const PoaConsensus* pc) const
{
    return impl_->ToGraphViz(flags, pc);
}

void PoaGraph::WriteGraphVizFile(const std::filesystem::path& filename, int flags,
                                 const PoaConsensus* pc) const
{
    impl_->WriteGraphVizFile(filename, flags, pc);
}

void PoaGraph::WriteGraphCsvFile(const std::filesystem::path& filename) const
{
    impl_->WriteGraphCsvFile(filename);
}

PoaGraph::PoaGraph() : impl_{std::make_unique<detail::PoaGraphImpl>()} {}

PoaGraph::PoaGraph(const PoaGraph& other)
    : impl_{std::make_unique<detail::PoaGraphImpl>(*other.impl_)}
{}

PoaGraph& PoaGraph::operator=(const PoaGraph& other)
{
    // check for self-assignment
    if (&other != this) {
        impl_ = std::make_unique<detail::PoaGraphImpl>(*other.impl_);
    }

    return *this;
}

PoaGraph::PoaGraph(PoaGraph&&) noexcept = default;

PoaGraph& PoaGraph::operator=(PoaGraph&&) noexcept = default;

PoaGraph::PoaGraph(const detail::PoaGraphImpl& o) : impl_{std::make_unique<detail::PoaGraphImpl>(o)}
{}

PoaGraph::~PoaGraph() = default;

}  // namespace Poa
}  // namespace PacBio
