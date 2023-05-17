#include <pbcopper/pbmer/DbgNode.h>
#include <pbcopper/utility/Intrinsics.h>

#include <array>

namespace PacBio {
namespace Pbmer {

/**
 * The bases are duplicated for the iterator, avoiding strand checking
 */
constexpr std::array<char, 8> BASES{'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T'};
constexpr std::array<uint8_t, 256> CLZ_LOOKUP{
    8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

DbgNode::DbgNode(const DnaBit& d, uint8_t o, uint32_t n) : dna_{d}, edges_{o}
{
    readIds2_.resize(n);
}

bool DbgNode::AddLoad(uint32_t rid)
{
    readIds2_[rid - 1] = 1;
    return true;
}
std::size_t DbgNode::FirstRId() const
{
    if (readIds2_.none()) {
        return 0;
    }
    return readIds2_.find_first() + 1;
}

uint64_t DbgNode::Kmer() const { return dna_.mer; }

int DbgNode::LeftEdgeCount() const
{
    // 11110000 = 240
    return Utility::PopCount(edges_ & 240);
}
int DbgNode::RightEdgeCount() const
{
    // 00001111 = 15
    return Utility::PopCount(edges_ & 15);
}

void DbgNode::SetEdges(uint8_t o) { edges_ |= o; }

int DbgNode::TotalEdgeCount() const { return Utility::PopCount(edges_ & 255); }

// -------------------------------------------
// iteration of DnaBits over node

DbgNode::iterator_base::iterator_base() = default;

DbgNode::iterator_base::iterator_base(DbgNode& node) : node_{&node} { LoadNext(); }

DbgNode::iterator_base::~iterator_base() = default;

bool DbgNode::iterator_base::operator==(const iterator_base& other) const noexcept
{
    return node_ == other.node_;
}

bool DbgNode::iterator_base::operator!=(const iterator_base& other) const noexcept
{
    return !(*this == other);
}

void DbgNode::iterator_base::LoadNext()
{
    if (index_ < 8) {

        // this is using a lookup table to find the current on bit. The 255
        // shift is a mask. This saves time because we don't loop over all
        // four bases x 2 strands.
        index_ = CLZ_LOOKUP[((255 >> (index_)) & node_->edges_)];

        DnaBit niby = node_->dna_;
        if (index_ > 3) {
            niby.PrependBase(BASES[7 - index_]);
        } else {
            niby.AppendBase(BASES[7 - index_]);
        }
        value_ = niby.LexSmallerEq();

        index_ = CLZ_LOOKUP[((255 >> (index_ + 1)) & node_->edges_)];

    } else {
        node_ = nullptr;
    }
}

DbgNode::iterator::iterator() = default;
DbgNode::iterator::iterator(DbgNode& node) : iterator_base{node} {}
DnaBit& DbgNode::iterator::operator*() noexcept { return iterator_base::value_; }
DnaBit* DbgNode::iterator::operator->() noexcept { return &(operator*()); }
DbgNode::iterator& DbgNode::iterator::operator++()
{
    LoadNext();
    return *this;
}

DbgNode::iterator DbgNode::iterator::operator++(int)
{
    DbgNode::iterator result(*this);
    ++(*this);
    return result;
}

DbgNode::const_iterator::const_iterator() = default;
DbgNode::const_iterator::const_iterator(const DbgNode& node)
    : iterator_base{const_cast<DbgNode&>(node)}
{}
const DnaBit& DbgNode::const_iterator::operator*() const noexcept { return iterator_base::value_; }
const DnaBit* DbgNode::const_iterator::operator->() const noexcept { return &(operator*()); }
DbgNode::const_iterator& DbgNode::const_iterator::operator++()
{
    LoadNext();
    return *this;
}

DbgNode::const_iterator DbgNode::const_iterator::operator++(int)
{
    const_iterator result(*this);
    ++(*this);
    return result;
}

DbgNode::const_iterator DbgNode::begin() const { return const_iterator{*this}; }
DbgNode::const_iterator DbgNode::cbegin() const { return const_iterator{*this}; }
DbgNode::iterator DbgNode::begin() { return iterator{*this}; }

DbgNode::const_iterator DbgNode::end() const { return const_iterator{}; }
DbgNode::const_iterator DbgNode::cend() const { return const_iterator{}; }
DbgNode::iterator DbgNode::end() { return iterator{}; }

// -------------------------------------------

}  // namespace Pbmer
}  // namespace PacBio
