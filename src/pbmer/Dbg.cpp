// Authors: Chris Dunn, Zev Kronenberg, Derek Barnett

#include <pbcopper/pbmer/Dbg.h>

#include <cassert>
#include <deque>
#include <fstream>
#include <iostream>
#include <iterator>
#include <unordered_set>

namespace PacBio {
namespace Pbmer {

constexpr const std::array<char, 4> bases{'A', 'C', 'G', 'T'};

DbgNode::DbgNode(const DnaBit& d, uint8_t o, uint8_t i, int32_t l)
    : dna_{d}, inEdges_{i}, outEdges_{o}, readIds_{l}
{
}

void DbgNode::SetOutEdges(uint8_t o) { outEdges_ |= o; }
void DbgNode::SetInEdges(uint8_t i) { inEdges_ |= i; }

void DbgNode::AddLoad(int32_t rid) { readIds_.insert(rid); }

// -------------------------------------------
// iteration of DnaBits over node

DbgNode::iterator_base::iterator_base() = default;

DbgNode::iterator_base::iterator_base(DbgNode& node) : node_{&node} { LoadNext(); }

DbgNode::iterator_base::~iterator_base() = default;

bool DbgNode::iterator_base::operator==(const iterator_base& other) const
{
    return node_ == other.node_;
}

bool DbgNode::iterator_base::operator!=(const iterator_base& other) const
{
    return !(*this == other);
}

void DbgNode::iterator_base::LoadNext()
{
    assert(node_);
    assert(index_ <= 4);

    while (index_ < 4) {
        if (node_->outEdges_.test(index_)) {
            DnaBit niby = node_->dna_;
            if (niby.strand) {
                niby.PrependBase(bases[index_]);
            } else {
                niby.AppendBase(bases[index_]);
            }
            value_ = niby.LexSmallerEq();
            ++index_;
            return;
        }
        ++index_;
    }

    // not found
    if (index_ == 4) node_ = nullptr;
}

DbgNode::iterator::iterator() = default;
DbgNode::iterator::iterator(DbgNode& node) : iterator_base{node} {}
DnaBit& DbgNode::iterator::operator*() { return iterator_base::value_; }
DnaBit* DbgNode::iterator::operator->() { return &(operator*()); }
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
{
}
const DnaBit& DbgNode::const_iterator::operator*() const { return iterator_base::value_; }
const DnaBit* DbgNode::const_iterator::operator->() const { return &(operator*()); }
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

Dbg::Dbg(uint8_t k) : kmerSize_{k} {}

size_t Dbg::NNodes() const { return dbg_.size(); }

int Dbg::AddKmers(const PacBio::Pbmer::Mers& m, const int32_t rid)
{
    // cover the cases where the kmers are not suitable for the Dbg.
    if ((m.kmerSize >= 30)) return -1;

    if ((m.kmerSize % 2) == 0) return -2;

    bool toFlip = false;

    for (auto const& x : m.forward) {
        DnaBit niby{x.mer, static_cast<uint8_t>(x.strand == Data::Strand::FORWARD ? 0 : 1),
                    kmerSize_};

        if (toFlip) {
            niby.strand = !niby.strand;
        }

        niby = niby.LexSmallerEq();

        auto got = dbg_.find(niby.mer);
        if (got != dbg_.end()) {
            if (got.value().dna_.strand != niby.strand) {
                toFlip = !toFlip;
            }

            got.value().AddLoad(rid);
        } else {
            DbgNode eg{niby, 0, 0, rid};
            dbg_.emplace(niby.mer, eg);
        }
    }
    return 1;
}

void Dbg::ResetEdges()
{
    for (auto x = dbg_.begin(); x != dbg_.end(); ++x) {
        x.value().outEdges_.reset();
        x.value().inEdges_.reset();
    }
}

void Dbg::BuildEdges()
{
    for (auto x = dbg_.begin(); x != dbg_.end(); ++x) {
        // outgoing edges
        for (const auto& y : bases) {

            DnaBit niby = x.value().dna_;

            int last_base = 0;
            if (niby.strand) {
                niby.PrependBase(y);
                last_base = AsciiToDna[y];
            } else {
                niby.AppendBase(y);
                last_base = niby.mer & 3;
            }

            niby = niby.LexSmallerEq();

            auto got = dbg_.find(niby.mer);
            if (got != dbg_.end()) {
                //using the last nibble in the mer to set out edges by a shift
                // & 3 gets the last base
                x.value().SetOutEdges((uint8_t(1) << (last_base & 3)));
            }
        }

        for (const auto& y : bases) {
            DnaBit niby = x.value().dna_;

            int last_base = 0;
            if (niby.strand) {

                niby.AppendBase(y);
                last_base = niby.mer & 3;

            } else {
                niby.PrependBase(y);
                last_base = AsciiToDna[y];
            }

            niby = niby.LexSmallerEq();

            auto got = dbg_.find(niby.mer);
            if (got != dbg_.end()) {
                //using the last nibble in the mer to set in edges by a shift
                // 2*(msize-1) gets the first base
                x.value().SetInEdges((uint8_t(1) << (last_base & 3)));
            }
        }
    }
}

void Dbg::WriteGraph(std::string fn)
{
    std::ofstream outfile;
    outfile.open(fn);

    outfile << "digraph DBGraph {\n";
    for (auto x = dbg_.begin(); x != dbg_.end(); ++x) {
        outfile << "    " << x.value().dna_.KmerToStr();
        if (x.value().dna_.strand) {
            outfile << " [fillcolor=red, style=\"rounded,filled\", shape=diamond]\n";
        } else {
            outfile << " [fillcolor=grey, style=\"rounded,filled\", shape=ellipse]\n";
        }
    }
    for (auto x = dbg_.begin(); x != dbg_.end(); ++x) {
        for (const auto& y : x.value()) {
            const DnaBit niby = y;

            outfile << "    " << x.value().dna_.KmerToStr() << " -> " << niby.KmerToStr() << ";\n";
        }
    }
    outfile << "}";
    outfile.close();
}

bool Dbg::ValidateLoad() const
{
    for (const auto& x : dbg_) {
        if (x.second.readIds_.size() == 0) return false;
    }
    return true;
}

bool Dbg::ValidateEdges() const
{

    for (auto x = dbg_.begin(); x != dbg_.end(); ++x) {
        for (const auto& y : x.value()) {
            auto got = dbg_.find(y.mer);
            if (got == dbg_.end()) {
                return false;
            }
        }
    }
    return true;
}

void Dbg::FrequencyFilterNodes(unsigned long n)
{
    this->ResetEdges();

    std::vector<uint64_t> toRemove;

    for (auto x = dbg_.begin(); x != dbg_.end(); ++x) {
        if (x.value().readIds_.size() < n) {
            toRemove.push_back(x->first);
        }
    }
    for (const auto x : toRemove) {
        dbg_.erase(x);
    }
}

void Dbg::DumpNodes() const
{
    for (auto x = dbg_.begin(); x != dbg_.end(); ++x) {
        std::cout << "    " << x.value().dna_.KmerToStr() << " n in:" << x.value().inEdges_.count()
                  << " n out:" << x.value().outEdges_.count() << "\n";
    }
}

std::vector<uint64_t> Dbg::GetLinearPath(const DnaBit& niby, uint64_t& hit) const
{
    // deque is used for the breadth first
    std::deque<uint64_t> to_visit;
    // return value - node keys in the path
    std::vector<uint64_t> result;
    // lookup for which nodes we've seen
    std::unordered_set<uint64_t> seen;

    // first node goes into the structures
    to_visit.push_back(niby.mer);
    seen.insert(niby.mer);
    result.push_back(niby.mer);

    // keep going while there are nodes in the deque
    while (!to_visit.empty()) {
        // looping over out edges, looking foward to the next nodes.
        for (const auto& y : dbg_.at(to_visit.front())) {
            // Next node's out degree > 1 : path is no longer linear.
            if (dbg_.at(y.mer).outEdges_.count() > 1) {
                break;
            }
            // Next node's in degree > 1 : path is no longer linear.
            // The merging fork is marked for downstream bubble detection.
            if (dbg_.at(y.mer).inEdges_.count() > 1) {
                hit = dbg_.at(y.mer).dna_.mer;
                break;
            }
            // node passes
            if (seen.find(y.mer) == seen.end()) {
                to_visit.push_back(y.mer);
                seen.insert(y.mer);
                result.push_back(y.mer);
            }
        }
        to_visit.pop_front();
    }
    return result;
}

/* This method only copes with outgoing spurs */
int Dbg::RemoveSpurs(unsigned int maxLength)
{
    int nSpurs = 0;
    uint64_t e1, e2;

    std::unordered_set<uint64_t> toDelete;

    for (auto x = dbg_.begin(); x != dbg_.end(); ++x) {
        // one out edge cannot have a spur.
        if (x.value().outEdges_.count() != 2) continue;
        auto first_edge = x.value().begin();
        auto second_edge = ++(x.value().begin());

        assert(first_edge->mer != second_edge->mer);

        e1 = 0;
        e2 = 0;

        auto left = GetLinearPath(*first_edge, e1);
        auto right = GetLinearPath(*second_edge, e2);

        // one of the paths ends at an in-edge - not a simple spur
        if ((e1 > 0) && (e2 > 0)) {
            continue;
        }
        // putative spur is too long and shouldn't be filtered.
        if (left.size() > maxLength && right.size() > maxLength) {
            continue;
        }

        if (left.size() <= right.size()) {
            for (const auto l : left) {
                toDelete.insert(l);
            }
        } else {
            for (const auto r : right) {
                toDelete.insert(r);
            }
        }
        ++nSpurs;
    }
    for (const auto x : toDelete) {
        dbg_.erase(x);
    }

    this->ResetEdges();
    this->BuildEdges();

    return nSpurs;
}

BubbleInfo Dbg::GetBubbles() const
{
    int count = 0;
    uint64_t e1, e2;

    // returned container describing which reads traverse which forks.
    BubbleInfo result;

    std::map<int32_t, int> left_reads;
    std::map<int32_t, int> right_reads;

    for (auto x = dbg_.begin(); x != dbg_.end(); ++x) {
        // two out edges are required at fork
        if (x.value().outEdges_.count() != 2) continue;
        auto first_edge = x.value().begin();
        auto second_edge = ++(x.value().begin());

        assert(first_edge->mer != second_edge->mer);

        e1 = 0;
        e2 = 0;

        auto left = GetLinearPath(*first_edge, e1);
        auto right = GetLinearPath(*second_edge, e2);

        // both nodes don't have an in-degree > 0
        if ((e1 + e2) == 0) {
            continue;
        }
        // in degree do not agree
        if (e1 != e2) {
            continue;
        }
        // not a simple bubble.
        else {
            ++count;
        }

        // reuse the same temp data struct.
        left_reads.clear();
        right_reads.clear();

        for (auto const& l : left) {
            for (auto const& lid : dbg_.at(l).readIds_) {
                ++left_reads[lid];
            }
        }
        for (auto const& r : right) {
            for (auto const& rid : dbg_.at(r).readIds_) {
                ++right_reads[rid];
            }
        }

        std::string lk = x.value().dna_.KmerToStr() + "L";
        std::string rk = x.value().dna_.KmerToStr() + "R";

        assert(result.find(lk) == result.end());

        for (const auto& kv : left_reads) {
            result[lk].push_back(std::make_tuple(kv.first, kv.second, left.size()));
        }

        for (const auto& kv : right_reads) {
            result[rk].push_back(std::make_tuple(kv.first, kv.second, right.size()));
        }
    }
    return result;
}

}  // namespace Pbmer
}  // namespace PacBio
