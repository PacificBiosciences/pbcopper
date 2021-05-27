#include <pbcopper/pbmer/KFNode.h>

#include <array>

namespace PacBio {
namespace Pbmer {

KFNode::KFNode(const DnaBit& d, size_t n, uint64_t k) : key_{k}, dna_{d} { readIds_.resize(n); }

bool KFNode::ContainsSeq(uint32_t rid) const { return readIds_[rid - 1] == 1; }

bool KFNode::AddLoad(uint32_t rid)
{
    readIds_[rid - 1] = 1;
    return true;
}
size_t KFNode::FirstRId() const
{
    if (readIds_.none()) {
        return 0;
    }
    return readIds_.find_first() + 1;
}

int KFNode::SeqCount() const { return readIds_.count(); }

void KFNode::AddOutEdge(uint64_t e) { outEdges_.insert(e); }

void KFNode::AddInEdge(uint64_t e) { inEdges_.insert(e); }

uint64_t KFNode::Kmer() const { return dna_.mer; }

int KFNode::InEdgeCount() const { return inEdges_.size(); }
int KFNode::OutEdgeCount() const { return outEdges_.size(); }

uint64_t KFNode::Key() const { return key_; }

DnaBit KFNode::Bit() const { return dna_; }

std::string Vec2String(const std::vector<KFNode>& nodes)
{
    std::string rv;
    if (nodes.empty()) {
        return rv;
    }
    rv += nodes.front().Bit().KmerToStr();
    if (nodes.size() == 1) {
        return rv;
    }

    for (size_t i = 1; i < nodes.size(); ++i) {
        DnaBit d = nodes[i].Bit();
        if (!d.KmerToStr().empty()) {
            rv += d.KmerToStr().back();
        }
    }
    return rv;
}

}  // namespace Pbmer
}  // namespace PacBio
