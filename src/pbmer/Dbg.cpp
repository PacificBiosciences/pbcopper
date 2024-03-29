#include <pbcopper/pbmer/Dbg.h>

#include <pbcopper/container/Unordered.h>

#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>

#include <cassert>

namespace PacBio {
namespace Pbmer {

Dbg::Dbg(std::uint8_t k, std::uint32_t nr) : kmerSize_{k}, nReads_{nr} {}

int Dbg::AddKmers(const PacBio::Pbmer::Mers& m, const std::uint32_t rid)
{
    // cover the cases where the kmers are not suitable for the Dbg.
    if ((m.kmerSize > 31)) {
        return -1;
    }

    if ((m.kmerSize % 2) == 0) {
        return -2;
    }

    for (const auto& x : m.forward) {
        DnaBit niby{x.mer, static_cast<std::uint8_t>(x.strand == Data::Strand::FORWARD ? 0 : 1),
                    kmerSize_};

        niby.MakeLexSmaller();

        if (dbg_.find(niby.mer) != dbg_.end()) {
            dbg_.at(niby.mer).AddLoad(rid);
        } else {
            DbgNode eg{niby, 0, nReads_};
            eg.AddLoad(rid);
            dbg_.emplace(niby.mer, std::move(eg));
        }
    }
    return 1;
}

void Dbg::AddVerifedKmerPairs(std::vector<DnaBit>& bits, const std::uint32_t rid)
{

    for (auto& bit : bits) {
        bit.MakeLexSmaller();
    }

    auto edges = BuildVerifiedEdges(bits);

    for (std::size_t i = 0; i < bits.size(); ++i) {

        if (dbg_.find(bits[i].mer) != dbg_.end()) {
            dbg_.at(bits[i].mer).AddLoad(rid);
        } else {

            DbgNode eg{bits[i], 0, nReads_};

            eg.AddLoad(rid);

            dbg_.emplace(bits[i].mer, std::move(eg));
        }
        dbg_.at(bits[i].mer).SetEdges(edges[i]);
    }
}

uint8_t SetRevEdge(const DnaBit& a, const DnaBit& b)
{
    std::uint8_t c = (a.strand << 1) | (b.strand);
    std::uint8_t es = 1;

    DnaBit newNib = a;

    switch (c) {
        case 0: {
            es <<= (b.FirstBaseIdx() + 0);
            newNib.PrependBase(b.FirstBaseIdx());
            break;
        }
        case 1: {
            es <<= (b.FirstBaseRCIdx() + 0);
            newNib.PrependBase(b.FirstBaseRCIdx());
            newNib.MakeLexSmaller();
            break;
        }
        case 2: {
            es <<= (b.LastBaseRCIdx() + 4);
            newNib.AppendBase(b.LastBaseRCIdx());
            newNib.MakeLexSmaller();
            break;
        }
        case 3: {
            es <<= (b.LastBaseIdx() + 4);
            newNib.AppendBase(b.LastBaseIdx());
            break;
        }

        default: {
            break;
        }
    }
    if (a.mer == newNib.mer) {
        return 0;
    }
    return es;
}

uint8_t SetForEdge(const DnaBit& a, const DnaBit& b)
{
    std::uint8_t c = (a.strand << 1) | (b.strand);
    std::uint8_t es = 1;

    DnaBit newNib = a;

    switch (c) {
        case 0: {
            es <<= (b.LastBaseIdx() + 4);
            newNib.AppendBase(b.LastBaseIdx());
            break;
        }
        case 1: {
            es <<= (b.LastBaseRCIdx() + 4);
            newNib.AppendBase(b.LastBaseRCIdx());
            newNib.MakeLexSmaller();
            break;
        }
        case 2: {
            es <<= (b.FirstBaseRCIdx() + 0);
            newNib.PrependBase(b.FirstBaseRCIdx());
            newNib.MakeLexSmaller();
            break;
        }
        case 3: {
            es <<= (b.FirstBaseIdx() + 0);
            newNib.PrependBase(b.FirstBaseIdx());
            break;
        }

        default: {
            break;
        }
    }

    if (a.mer == newNib.mer) {
        return 0;
    }
    return es;
}

std::vector<std::uint8_t> Dbg::BuildVerifiedEdges(const std::vector<PacBio::Pbmer::DnaBit>& bits)
{

    std::vector<std::uint8_t> edges(bits.size(), 0);

    // set back edges
    for (std::size_t i = 1; i < bits.size(); ++i) {
        edges[i] |= SetRevEdge(bits[i], bits[i - 1]);
    }

    // set forward edges
    for (std::size_t i = 0; i < bits.size() - 1; ++i) {
        edges[i] |= SetForEdge(bits[i], bits[i + 1]);
    }

    return edges;
}

void Dbg::BuildEdges()
{
    for (auto& x : dbg_) {
        // all 8 possible edges
        for (std::uint8_t y = 0; y < 8; ++y) {

            DnaBit niby = x.second.dna_;
            // pre-prending base
            if (y <= 3) {
                niby.PrependBase(y);
            }
            // appending base
            else {
                niby.AppendBase(y);
            }

            // generate new lex smallest
            niby.MakeLexSmaller();

            // this is a self loop
            // TODO validate this should not be skipped.
            if (x.second.dna_.mer == niby.mer) {
                continue;
            }

            if (dbg_.find(niby.mer) != dbg_.end()) {
                //setting the edges
                x.second.SetEdges((std::uint8_t(1) << y));
            }
        }
    }
}

void Dbg::DumpNodes() const
{
    for (const auto& x : dbg_) {
        std::cout << "    " << x.second.dna_.KmerToStr() << " n out:" << x.second.TotalEdgeCount()
                  << " e val: " << static_cast<int>(x.second.edges_)
                  << " n ids: " << x.second.readIds2_.count()
                  << " n left eg: " << x.second.LeftEdgeCount()
                  << " n right eg: " << x.second.RightEdgeCount()
                  << " n total eg: " << x.second.TotalEdgeCount() << "\n";
    }
}

void Dbg::FrequencyFilterNodes(unsigned long n)
{
    this->ResetEdges();

    std::vector<std::uint64_t> toRemove;

    for (auto& x : dbg_) {
        if (x.second.readIds2_.count() < n) {
            toRemove.push_back(x.first);
        }
    }
    for (const auto x : toRemove) {
        dbg_.erase(x);
    }
}

void Dbg::FrequencyFilterNodes2(unsigned long n) { Dbg::FrequencyFilterNodes2(n, false); }

void Dbg::FrequencyFilterNodes2(unsigned long n, bool gt)
{

    std::vector<std::uint64_t> toRemove;

    auto filterDirection = [&](const auto count) { return gt ? (count > n) : (count < n); };

    for (auto& x : dbg_) {
        if (filterDirection(x.second.readIds2_.count())) {
            toRemove.push_back(x.first);
        }
    }
    for (const auto x : toRemove) {
        dbg_.erase(x);
    }

    std::uint64_t lexSmall = 0;

    for (auto& x : dbg_) {
        for (std::uint8_t y = 0; y < 8; ++y) {
            if (((1 << y) & x.second.edges_) == 0) {
                continue;
            }
            DnaBit niby = x.second.dna_;
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
                std::uint8_t turnOff = ~(std::uint8_t(1) << y);
                x.second.edges_ &= turnOff;
            }
        }
    }
}

Bubbles Dbg::FindBubbles() const
{
    // returned container describing which reads traverse which forks
    Bubbles result;

    // keeping track of read id counts over linear paths
    // these variables are reused
    Container::UnorderedMap<std::uint32_t, int> leftReads;
    Container::UnorderedMap<std::uint32_t, int> rightReads;

    // keep track of the used head/tails of bubbles.
    Container::UnorderedSet<std::uint64_t> usedBranchNode;

    // reused variables

    for (const auto& x : dbg_) {

        // this node is already part of a bubble and should be ignored.
        if (usedBranchNode.find(x.second.dna_.mer) != usedBranchNode.end()) {
            continue;
        }

        // valid bubbles contain 3 or more paths
        if (x.second.TotalEdgeCount() < 3) {
            continue;
        }
        std::vector<std::tuple<std::uint64_t, std::uint64_t>> pathInfo;

        // loop over neighboring nodes collecting the start and end node of
        // linear paths. I.E. looping over all linear paths coming out of a node.
        for (const auto& out : x.second) {
            const auto linearPath = LinearPath(out);
            if (!linearPath.empty()) {
                pathInfo.emplace_back(out.mer, linearPath.back().mer);
            }
        }

        // The left and right path of a bubble.
        std::vector<DnaBit> left;
        std::vector<DnaBit> right;

        bool hasBubble = false;

        // Comparing all linear paths to check if they converge. The first
        // two paths to converge are considered a bubble. subsequent bubbles are
        // ignored.
        for (const auto& [s1, e1] : pathInfo) {
            hasBubble = false;
            for (const auto& [s2, e2] : pathInfo) {
                // check if the paths converge on a common neighboring node.
                if (std::uint64_t shared; OneIntermediateNode(e1, e2, &shared)) {
                    // the paths are not stored in the first loop, maybe the should?
                    left = LinearPath(s1);
                    right = LinearPath(s2);
                    // set the used incoming node so we don't get 2x n bubbles
                    usedBranchNode.insert(shared);
                    usedBranchNode.insert(x.second.dna_.mer);
                    hasBubble = true;
                    break;
                }
            }
            // only keep the first valid bubble
            if (hasBubble) {
                break;
            }
        }

        if (!hasBubble) {
            continue;
        }
        // reuse the same temp data struct.
        leftReads.clear();
        rightReads.clear();

        for (const auto& l : left) {
            std::size_t i = dbg_.at(l.mer).readIds2_.find_first();
            while (i != dbg_.at(l.mer).readIds2_.npos) {
                ++leftReads[i + 1];
                i = dbg_.at(l.mer).readIds2_.find_next(i);
            }
        }

        for (const auto& r : right) {
            std::size_t i = dbg_.at(r.mer).readIds2_.find_first();
            while (i != dbg_.at(r.mer).readIds2_.npos) {
                ++rightReads[i + 1];
                i = dbg_.at(r.mer).readIds2_.find_next(i);
            }
        }

        BubbleInfo bubble;
        bubble.LSeq = DnaBitVec2String(left);
        bubble.RSeq = DnaBitVec2String(right);
        bubble.LKmerCount = left.size();
        bubble.RKmerCount = right.size();
        bubble.LVec = std::move(left);
        bubble.RVec = std::move(right);

        for (const auto& kv : leftReads) {
            bubble.LData.emplace_back(kv.first, kv.second);
        }

        for (const auto& kv : rightReads) {
            bubble.RData.emplace_back(kv.first, kv.second);
        }
        result.emplace_back(std::move(bubble));
    }
    return result;
}  // namespace Pbmer

std::vector<DnaBit> Dbg::LinearPath(std::uint64_t x) const { return LinearPath(dbg_.at(x).dna_); }

std::vector<DnaBit> Dbg::LinearPath(const DnaBit& niby) const
{
    std::vector<DnaBit> result;

    if (dbg_.at(niby.mer).TotalEdgeCount() > 2) {
        return result;
    }

    // lookup for which nodes we've seen to prevent loops
    Container::UnorderedSet<std::uint64_t> seen;

    std::uint64_t past = niby.mer;

    while (seen.find(past) == seen.end()) {
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
    std::ostringstream ss;
    ss << "digraph DBGraph {\n";
    for (auto& x : dbg_) {
        ss << "    " << x.second.dna_.KmerToStr();
        if (x.second.dna_.strand) {
            ss << " [fillcolor=red, style=\"rounded,filled\", shape=diamond]\n";
        } else {
            ss << " [fillcolor=grey, style=\"rounded,filled\", shape=ellipse]\n";
        }
    }
    for (auto& x : dbg_) {
        for (const auto& y : x.second) {
            const DnaBit niby = y;

            ss << "    " << x.second.dna_.KmerToStr() << " -> " << niby.KmerToStr() << ";\n";
        }
    }
    ss << "}";

    return ss.str();
}

std::size_t Dbg::NNodes() const { return dbg_.size(); }

std::size_t Dbg::NEdges() const
{
    return std::accumulate(
        std::begin(dbg_), std::end(dbg_), std::size_t(0),
        [](std::size_t sum, const auto& x) { return sum + x.second.TotalEdgeCount(); });
}

bool Dbg::OneIntermediateNode(std::uint64_t n1, std::uint64_t n2, std::uint64_t* shared) const
{
    Container::UnorderedSet<std::uint64_t> seen;
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

    Container::UnorderedSet<std::uint64_t> toDelete;

    for (auto& node : dbg_) {
        // Starting at tip nodes with a degree of one.
        if (node.second.TotalEdgeCount() != 1) {
            continue;
        }

        // Including tip node in the linear path.
        const auto linearPath = LinearPath(node.second.dna_.mer);

        if (linearPath.size() > maxLength) {
            continue;
        }
        for (const auto& x : linearPath) {
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
    for (auto& x : dbg_) {
        x.second.edges_ = 0;
    }
}

bool Dbg::ValidateEdges() const
{
    const auto& dbg = dbg_;
    return std::all_of(std::begin(dbg_), std::end(dbg_), [&dbg](const auto& x) {
        return std::all_of(std::begin(x.second), std::end(x.second),
                           [&dbg](const auto& y) { return dbg.find(y.mer) != dbg.end(); });
    });
}

bool Dbg::ValidateLoad() const
{
    return std::all_of(std::begin(dbg_), std::end(dbg_),
                       [](const auto& x) { return x.second.readIds2_.count() != 0; });
}

void Dbg::WriteGraph(const std::filesystem::path& filename)
{
    std::ofstream outfile{filename};
    outfile << Dbg::Graph2StringDot();
}

}  // namespace Pbmer
}  // namespace PacBio
