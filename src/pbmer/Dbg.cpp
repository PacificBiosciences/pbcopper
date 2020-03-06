// Authors: Chris Dunn, Zev Kronenberg, Derek Barnett

#include <pbcopper/pbmer/Dbg.h>

#include <cassert>

#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>

#include <pbcopper/third-party/kxsort/kxsort.h>

namespace PacBio {
namespace Pbmer {
namespace {

struct RadixTraits_128
{
    static const int nBytes = 8;  // only do 8 rounds
    int kth_byte(const BI& x, int k) { return (x >> 64) >> (k * 8) & 0x0FF; }
    bool compare(const BI& x, const BI& y) { return (x >> 64) < (y >> 64); }
};

}  // namespace

Dbg::Dbg(uint8_t k, uint32_t nr) : kmerSize_{k}, nReads_{nr} {}

void Dbg::AddKmers(std::vector<BI>& kmers, uint32_t minFreqCutoff)
{
    kx::radix_sort(kmers.begin(), kmers.end(), RadixTraits_128());
    size_t start_i = 0;
    size_t end_i = 0;

    while (end_i < kmers.size()) {
        ++end_i;
        if ((kmers[start_i] >> 64) != (kmers[end_i] >> 64)) {
            --end_i;

            if ((end_i - start_i) >= minFreqCutoff) {
                DnaBit db;
                db.Bin2DnaBit(kmers[start_i]);
                DbgNode eg{db, 0};
                eg.readIds2_.resize(nReads_);

                for (auto i = start_i; i <= end_i; ++i) {
                    uint32_t v = static_cast<uint32_t>(kmers[i]);
                    // converting from one base index to zero
                    eg.readIds2_[v - 1] = 1;
                }
                dbg_.emplace(db.mer, std::move(eg));
            }

            start_i = end_i + 1;
            end_i = start_i;
        }
    }
}

int Dbg::AddKmers(const PacBio::Pbmer::Mers& m, const uint32_t rid)
{
    // cover the cases where the kmers are not suitable for the Dbg.
    if ((m.kmerSize > 31)) return -1;

    if ((m.kmerSize % 2) == 0) return -2;

    for (auto const& x : m.forward) {
        DnaBit niby{x.mer, static_cast<uint8_t>(x.strand == Data::Strand::FORWARD ? 0 : 1),
                    kmerSize_};

        niby = niby.LexSmallerEq();

        if (dbg_.find(niby.mer) != dbg_.end()) {
            dbg_.at(niby.mer).AddLoad(rid);
        } else {
            DbgNode eg{niby, 0};
            eg.readIds2_.resize(nReads_);
            eg.AddLoad(rid);
            dbg_.emplace(niby.mer, std::move(eg));
        }
    }
    return 1;
}

void Dbg::AddVerifedKmerPairs(std::vector<DnaBit>& bits, const uint32_t rid)
{

    for (size_t i = 0; i < bits.size(); ++i) {
        bits[i].MakeLexSmaller();
    }

    auto edges = BuildVerifiedEdges(bits);

    for (size_t i = 0; i < bits.size(); ++i) {

        if (dbg_.find(bits[i].mer) != dbg_.end()) {
            dbg_.at(bits[i].mer).AddLoad(rid);
        } else {

            DbgNode eg{bits[i], 0};
            eg.readIds2_.resize(nReads_);

            eg.AddLoad(rid);

            dbg_.emplace(bits[i].mer, std::move(eg));
        }
        dbg_.at(bits[i].mer).SetEdges(edges[i]);
    }
}

uint8_t SetRevEdge(const DnaBit& a, const DnaBit& b)
{
    uint8_t c = (a.strand << 1) | (b.strand);
    uint8_t es = 1;

    DnaBit newNib = a;

    switch (c) {
        case 0: {
            es <<= (b.GetFirstBaseIdx() + 0);
            newNib.PrependBase(b.GetFirstBaseIdx());
            break;
        }
        case 1: {
            es <<= (b.GetFirstBaseRCIdx() + 0);
            newNib.PrependBase(b.GetFirstBaseRCIdx());
            newNib = newNib.LexSmallerEq();
            break;
        }
        case 2: {
            es <<= (b.GetLastBaseRCIdx() + 4);
            newNib.AppendBase(b.GetLastBaseRCIdx());
            newNib = newNib.LexSmallerEq();
            break;
        }
        case 3: {
            es <<= (b.GetLastBaseIdx() + 4);
            newNib.AppendBase(b.GetLastBaseIdx());
            break;
        }

        default: {
            break;
        }
    }
    if (a.mer == newNib.mer) return 0;
    return es;
}

uint8_t SetForEdge(const DnaBit& a, const DnaBit& b)
{
    uint8_t c = (a.strand << 1) | (b.strand);
    uint8_t es = 1;

    DnaBit newNib = a;

    switch (c) {
        case 0: {
            es <<= (b.GetLastBaseIdx() + 4);
            newNib.AppendBase(b.GetLastBaseIdx());
            break;
        }
        case 1: {
            es <<= (b.GetLastBaseRCIdx() + 4);
            newNib.AppendBase(b.GetLastBaseRCIdx());
            newNib = newNib.LexSmallerEq();
            break;
        }
        case 2: {
            es <<= (b.GetFirstBaseRCIdx() + 0);
            newNib.PrependBase(b.GetFirstBaseRCIdx());
            newNib = newNib.LexSmallerEq();
            break;
        }
        case 3: {
            es <<= (b.GetFirstBaseIdx() + 0);
            newNib.PrependBase(b.GetFirstBaseIdx());
            break;
        }

        default: {
            break;
        }
    }

    if (a.mer == newNib.mer) return 0;
    return es;
}

std::vector<uint8_t> Dbg::BuildVerifiedEdges(const std::vector<PacBio::Pbmer::DnaBit>& bits)
{

    std::vector<uint8_t> edges;
    edges.reserve(bits.size());
    for (size_t i = 0; i < bits.size(); ++i) {
        edges.push_back(0);
    }

    // set back edges
    for (size_t i = 1; i < bits.size(); ++i) {
        edges[i] |= SetRevEdge(bits[i], bits[i - 1]);
    }

    // set forward edges
    for (size_t i = 0; i < bits.size() - 1; ++i) {
        edges[i] |= SetForEdge(bits[i], bits[i + 1]);
    }

    return edges;
}

void Dbg::BuildEdges()
{
    for (auto x = dbg_.begin(); x != dbg_.end(); ++x) {
        // all 8 possible edges
        for (uint8_t y = 0; y < 8; ++y) {

            DnaBit niby = x->second.dna_;
            // pre-prending base
            if (y <= 3) {
                niby.PrependBase(y);
            }
            // appending base
            else {
                niby.AppendBase(y);
            }

            // generate new lex smallest
            niby = niby.LexSmallerEq();

            // this is a self loop
            // TODO validate this should not be skipped.
            if (x->second.dna_.mer == niby.mer) continue;

            if (dbg_.find(niby.mer) != dbg_.end()) {
                //setting the edges
                x->second.SetEdges((uint8_t(1) << y));
            }
        }
    }
}

void Dbg::DumpNodes() const
{
    for (auto x = dbg_.begin(); x != dbg_.end(); ++x) {
        std::cout << "    " << x->second.dna_.KmerToStr() << " n out:" << x->second.TotalEdgeCount()
                  << " e val: " << static_cast<int>(x->second.edges_)
                  << " n ids: " << x->second.readIds2_.count()
                  << " n left eg: " << x->second.LeftEdgeCount()
                  << " n right eg: " << x->second.RightEdgeCount()
                  << " n total eg: " << x->second.TotalEdgeCount() << "\n";
    }
}

void Dbg::FrequencyFilterNodes(unsigned long n)
{
    this->ResetEdges();

    std::vector<uint64_t> toRemove;

    for (auto x = dbg_.begin(); x != dbg_.end(); ++x) {
        if (x->second.readIds2_.count() < n) {
            toRemove.push_back(x->first);
        }
    }
    for (const auto x : toRemove) {
        dbg_.erase(x);
    }
}

void Dbg::FrequencyFilterNodes2(unsigned long n)
{

    std::vector<uint64_t> toRemove;

    for (auto x = dbg_.begin(); x != dbg_.end(); ++x) {
        if (x->second.readIds2_.count() < n) {
            toRemove.push_back(x->first);
        }
    }
    for (const auto x : toRemove) {
        dbg_.erase(x);
    }

    uint64_t lexSmall = 0;

    for (auto x = dbg_.begin(); x != dbg_.end(); ++x) {
        for (uint8_t y = 0; y < 8; ++y) {
            if (((1 << y) & x->second.edges_) == 0) continue;
            DnaBit niby = x->second.dna_;
            // pre-prending base
            if (y <= 3) {
                niby.PrependBase(y);
            }
            // appending base
            else {
                niby.AppendBase(y);
            }

            // generate new lex smallest
            lexSmall = niby.LexSmallerEq64();

            if (dbg_.find(lexSmall) == dbg_.end()) {
                uint8_t turnOff = ~(uint8_t(1) << y);
                x->second.edges_ &= turnOff;
            }
        }
    }
}

BubbleInfo Dbg::GetBubbles() const
{
    // returned container describing which reads traverse which forks
    BubbleInfo result;

    // keeping track of read id counts over linear paths
    // these variables are reused
    robin_hood::unordered_map<uint32_t, int> left_reads;
    robin_hood::unordered_map<uint32_t, int> right_reads;

    // keep track of the used head/tails of bubbles.
    std::unordered_set<uint64_t> used_branch_node;

    // reused variables
    uint64_t s1, s2, e1, e2, shared;

    for (auto x = dbg_.begin(); x != dbg_.end(); ++x) {

        // this node is already part of a bubble and should be ignored.
        if (used_branch_node.find(x->second.dna_.mer) != used_branch_node.end()) continue;

        // valid bubbles contain 3 or more paths
        if (x->second.TotalEdgeCount() < 3) continue;
        std::vector<std::tuple<uint64_t, uint64_t>> path_info;

        // loop over neighboring nodes collecting the start and end node of
        // linear paths. I.E. looping over all linear paths coming out of a node.
        for (auto& out : x->second) {
            auto linear_path = GetLinearPath(out);
            if (linear_path.empty()) continue;
            path_info.push_back(std::make_tuple(out.mer, linear_path.back().mer));
        }

        // The left and right path of a bubble.
        std::vector<DnaBit> left;
        std::vector<DnaBit> right;

        bool bubble = false;

        // Comparing all linear paths to check if they converge. The first
        // two paths to converge are considered a bubble. subsequent bubbles are
        // ignored.
        for (size_t i = 0; i < path_info.size(); ++i) {
            bubble = false;
            for (size_t j = 0; j < path_info.size(); ++j) {
                s1 = std::get<0>(path_info[i]);
                s2 = std::get<0>(path_info[j]);
                e1 = std::get<1>(path_info[i]);
                e2 = std::get<1>(path_info[j]);

                // check if the paths converge on a common neighboring node.
                if (OneIntermediateNode(e1, e2, &shared)) {
                    // the paths are not stored in the first loop, maybe the should?
                    left = GetLinearPath(s1);
                    right = GetLinearPath(s2);
                    // set the used incoming node so we don't get 2x n bubbles
                    used_branch_node.insert(shared);
                    bubble = true;
                    break;
                }
            }
            // only keep the first valid bubble
            if (bubble) break;
        }

        if (!bubble) {
            continue;
        }
        // reuse the same temp data struct.
        left_reads.clear();
        right_reads.clear();

        for (auto const& l : left) {
            for (size_t i = 0; i < dbg_.at(l.mer).readIds2_.size(); ++i) {
                if (dbg_.at(l.mer).readIds2_[i] != 0) ++left_reads[i + 1];
            }
        }
        for (auto const& r : right) {
            for (size_t i = 0; i < dbg_.at(r.mer).readIds2_.size(); ++i) {
                if (dbg_.at(r.mer).readIds2_[i] != 0) ++right_reads[i + 1];
            }
        }

        std::string lk = x->second.dna_.KmerToStr() + "L";
        std::string rk = x->second.dna_.KmerToStr() + "R";

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

std::vector<DnaBit> Dbg::GetLinearPath(uint64_t x) const { return GetLinearPath(dbg_.at(x).dna_); }

std::vector<DnaBit> Dbg::GetLinearPath(const DnaBit& niby) const
{
    std::vector<DnaBit> result;

    if (dbg_.at(niby.mer).TotalEdgeCount() > 2) {
        return result;
    }

    // lookup for which nodes we've seen to prevent loops
    std::unordered_set<uint64_t> seen;

    uint64_t past = niby.mer;

    while (1) {
        if (seen.find(past) != seen.end()) {
            break;
        }
        seen.insert(past);
        result.push_back(dbg_.at(past).dna_);
        for (const auto& y : dbg_.at(past)) {
            if (dbg_.at(y.mer).TotalEdgeCount() > 2) {
                continue;
            }
            if (seen.find(y.mer) == seen.end()) {
                past = y.mer;
            }
        }
    }
    return result;
}

std::string Dbg::Graph2StringDot()
{
    std::stringstream ss;
    ss << "digraph DBGraph {\n";
    for (auto x = dbg_.begin(); x != dbg_.end(); ++x) {
        ss << "    " << x->second.dna_.KmerToStr();
        if (x->second.dna_.strand) {
            ss << " [fillcolor=red, style=\"rounded,filled\", shape=diamond]\n";
        } else {
            ss << " [fillcolor=grey, style=\"rounded,filled\", shape=ellipse]\n";
        }
    }
    for (auto x = dbg_.begin(); x != dbg_.end(); ++x) {
        for (const auto& y : x->second) {
            const DnaBit niby = y;

            ss << "    " << x->second.dna_.KmerToStr() << " -> " << niby.KmerToStr() << ";\n";
        }
    }
    ss << "}";

    return ss.str();
}

size_t Dbg::NNodes() const { return dbg_.size(); }

size_t Dbg::NEdges() const
{
    size_t edgeCount = 0;

    for (auto x = dbg_.begin(); x != dbg_.end(); ++x) {
        edgeCount += x->second.TotalEdgeCount();
    }
    return edgeCount;
}

bool Dbg::OneIntermediateNode(uint64_t n1, uint64_t n2, uint64_t* shared) const
{
    std::unordered_set<uint64_t> seen;
    if (n1 == n2) {
        return false;
    }
    for (const auto& nout : dbg_.at(n1)) {
        seen.insert(nout.mer);
    }
    for (const auto& nout : dbg_.at(n2)) {
        if (seen.find(nout.mer) != seen.end()) {
            *shared = nout.mer;
            return true;
        }
    }
    return false;
}

int Dbg::RemoveSpurs(unsigned int maxLength)
{
    int nSpurs = 0;

    std::unordered_set<uint64_t> toDelete;

    for (auto nodeIter = dbg_.begin(); nodeIter != dbg_.end(); ++nodeIter) {
        // Starting at tip nodes with a degree of one.
        if (nodeIter->second.TotalEdgeCount() != 1) continue;

        // Including tip node in the linear path.
        auto linear_path = GetLinearPath(nodeIter->second.dna_.mer);

        if (linear_path.size() > maxLength) {
            continue;
        }
        for (const auto x : linear_path) {
            toDelete.insert(x.mer);
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

void Dbg::ResetEdges()
{
    for (auto x = dbg_.begin(); x != dbg_.end(); ++x) {
        x->second.edges_ = 0;
    }
}

bool Dbg::ValidateEdges() const
{
    bool valid = true;
    int count = 0;

    for (auto x = dbg_.begin(); x != dbg_.end(); ++x) {
        for (const auto& y : x->second) {
            auto got = dbg_.find(y.mer);
            if (got == dbg_.end()) {
                ++count;

                valid = false;
            }
        }
    }

    //std::cerr << "N failed: " << count << "\n";
    return valid;
}

bool Dbg::ValidateLoad() const
{
    for (const auto& x : dbg_) {
        if (x.second.readIds2_.count() == 0) return false;
    }
    return true;
}

void Dbg::WriteGraph(std::string fn)
{
    std::ofstream outfile;
    outfile.open(fn);
    outfile << Dbg::Graph2StringDot();

    outfile.close();
}

}  // namespace Pbmer
}  // namespace PacBio
