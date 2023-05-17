#include <pbcopper/pbmer/KFGraph.h>

#include <pbcopper/utility/MoveAppend.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <unordered_set>

#include <cassert>

namespace PacBio {
namespace Pbmer {

KFG::KFG(uint8_t k, std::size_t nr) : kmerSize_{k}, nReads_{nr} {}

bool KFG::HasNode(const DnaBit& bit) const { return (kfg_.find(bit.HashedKmer()) != kfg_.cend()); }

bool KFG::MoreRCHits(const std::vector<DnaBit>& bits) const
{

    uint32_t fs = 0;
    uint32_t rs = 0;

    for (const auto& k : bits) {
        auto rc = k;
        rc.ReverseComp();
        if (HasNode(k)) {
            ++fs;
        }
        if (HasNode(rc)) {
            ++rs;
        }
    }
    if (rs > fs) {
        return true;
    }
    return false;
}

uint64_t KFG::Knock(const DnaBit& b, uint64_t hashedKmer) const
{
    auto node = kfg_.find(hashedKmer);
    if (node == kfg_.end()) {
        return 0;
    }
    // if the keys match so too must the kmers;
    if (node->second.Kmer() != b.mer) {
        throw std::runtime_error{"[pbmer] knock graph ERROR: kmer.a != kmer.b, but same hash."};
    }
    return hashedKmer;
}

uint64_t KFG::OpenDoor(const std::vector<DnaBit>& bits, uint64_t hashedKmer, uint32_t rid,
                       std::size_t i, std::size_t j) const
{
    if (j == 0) {
        return hashedKmer;
    }

    auto hit = Knock(bits[i], hashedKmer);
    if (hit == 0) {
        return hashedKmer;
    }
    j = j - 1;
    if (kfg_.at(hit).ContainsSeq(rid)) {
        auto newHashed = (hashedKmer) ^ bits[j].mer;
        if (newHashed == 0) {
            throw std::runtime_error{"[pbmer] knock graph ERROR: zero hash value"};
        }
        return OpenDoor(bits, newHashed, rid, i, j);
    }
    return hashedKmer;
}

void KFG::AddSeq(const std::vector<DnaBit>& bits, const std::size_t rid, const std::string& rn)
{

    nameToId_[rn] = rid;
    idToName_[rid] = rn;

    uint64_t last = 0;
    for (std::size_t i = 0; i < bits.size(); ++i) {
        uint64_t nextHashedKmer = OpenDoor(bits, bits[i].HashedKmer(), rid, i, i);
        if (nextHashedKmer == 0) {
            throw std::runtime_error{"[pbcopper] knock graph ERROR: zero hash value"};
        }
        if (Knock(bits[i], nextHashedKmer) == 0) {
            kfg_.emplace(nextHashedKmer, KFNode{bits[i], nReads_, nextHashedKmer});
            kfg_.at(nextHashedKmer).AddLoad(rid);
        } else {
            kfg_.at(nextHashedKmer).AddLoad(rid);
        }
        if (i > 0) {
            kfg_.at(last).AddOutEdge(nextHashedKmer);
            kfg_.at(nextHashedKmer).AddInEdge(last);
        }
        last = nextHashedKmer;
    }
}

int32_t KFG::NNodes() const { return kfg_.size(); }

int32_t KFG::InEdgeCount() const
{
    std::size_t edgeCount = 0;
    for (const auto& x : kfg_) {
        edgeCount += x.second.inEdges_.size();
    }
    return edgeCount;
}

int32_t KFG::OutEdgeCount() const
{
    std::size_t edgeCount = 0;
    for (const auto& x : kfg_) {
        edgeCount += x.second.outEdges_.size();
    }
    return edgeCount;
}

bool KFG::ValidateLoad() const
{
    for (const auto& x : kfg_) {
        if (x.second.SeqCount() == 0) {
            return false;
        }
    }
    return true;
}

void KFG::CleanUpEdges()
{
    std::vector<uint64_t> toRemove;
    for (auto& node : kfg_) {
        toRemove.clear();
        for (const auto& e : node.second.inEdges_) {
            if (kfg_.find(e) == kfg_.end()) {
                toRemove.push_back(e);
            }
        }
        for (const auto& e : node.second.outEdges_) {
            if (kfg_.find(e) == kfg_.end()) {
                toRemove.push_back(e);
            }
        }

        for (const auto& re : toRemove) {
            node.second.inEdges_.erase(re);
            node.second.outEdges_.erase(re);
        }
    }
}

void KFG::SeqCountFilter(int n, bool gt)
{
    std::vector<uint64_t> toRemove;

    auto filterDirection = [&](const auto count) {
        if (gt) {
            return (count > n);
        } else {
            return (count < n);
        }
    };

    for (const auto& node : kfg_) {
        if (filterDirection(node.second.SeqCount())) {
            toRemove.push_back(node.first);
        }
    }
    for (const auto x : toRemove) {
        kfg_.erase(x);
    }

    CleanUpEdges();
}

std::vector<KFNode> KFG::LinearPath(uint64_t x) const { return LinearPath(kfg_.at(x)); }

std::vector<KFNode> KFG::LinearPath(const KFNode& node) const
{
    std::vector<KFNode> result;

    if (node.InEdgeCount() > 1 || node.OutEdgeCount() > 1) {
        return result;
    }

    // lookup for which nodes we've seen to prevent loops
    std::unordered_set<uint64_t> seen;

    uint64_t past = node.Key();

    while (1) {
        if (seen.find(past) != seen.end()) {
            break;
        }
        seen.insert(past);
        result.push_back(kfg_.at(past));
        for (const auto& y : kfg_.at(past)) {
            if (kfg_.at(y).OutEdgeCount() > 1 || kfg_.at(y).InEdgeCount() > 1) {
                continue;
            }
            if (seen.find(y) == seen.end()) {
                past = y;
            }
        }
    }
    return result;
}

Bubbles KFG::FindBubbles() const
{
    // returned container describing which reads traverse which forks
    Bubbles result;

    // keeping track of read id counts over linear paths
    // these variables are reused
    robin_hood::unordered_map<uint32_t, int> left_reads;
    robin_hood::unordered_map<uint32_t, int> right_reads;

    // keep track of the used head/tails of bubbles.
    std::unordered_set<uint64_t> used_branch_node;

    for (const auto& node : kfg_) {

        // this node is already part of a bubble and should be ignored.
        if (used_branch_node.find(node.second.dna_.HashedKmer()) != used_branch_node.end()) {
            continue;
        }

        // simple bubbles contain 2 out edges
        if (node.second.OutEdgeCount() != 2) {
            continue;
        }

        std::unordered_set<uint64_t>::const_iterator paths = node.second.begin();

        std::vector<KFNode> left = LinearPath(*paths);
        ++paths;
        std::vector<KFNode> right = LinearPath(*paths);

        bool hasBubble = false;

        if (!left.empty() && !right.empty() && !left.back().outEdges_.empty()) {
            if (left.back().outEdges_ == right.back().outEdges_) {
                hasBubble = true;
            }
        }
        if (!hasBubble) {
            continue;
        }

        // reuse the same temp data struct.
        left_reads.clear();
        right_reads.clear();

        for (auto const& l : left) {
            std::size_t i = l.readIds_.find_first();
            while (i != l.readIds_.npos) {
                ++left_reads[i + 1];
                i = l.readIds_.find_next(i);
            }
        }

        for (auto const& r : right) {
            std::size_t i = r.readIds_.find_first();
            while (i != r.readIds_.npos) {
                ++right_reads[i + 1];
                i = r.readIds_.find_next(i);
            }
        }

        BubbleInfo bubble;
        bubble.LSeq = Vec2String(left);
        bubble.RSeq = Vec2String(right);
        bubble.LKmerCount = left.size();
        bubble.RKmerCount = right.size();

        for (const auto& kv : left_reads) {
            bubble.LData.push_back(std::make_pair(kv.first, kv.second));
        }

        for (const auto& kv : right_reads) {
            bubble.RData.push_back(std::make_pair(kv.first, kv.second));
        }
        result.emplace_back(std::move(bubble));
    }
    return result;
}

std::string KFG::Graph2StringDot() const
{
    std::ostringstream ss;
    ss << "digraph DBGraph {\n";
    for (const auto& x : kfg_) {
        ss << "    " << x.second.Key();
        ss << " [fillcolor=grey, style=\"rounded,filled\", shape=ellipse, label=\""
           << x.second.Bit().KmerToStr() << "\"]"
           << "\n";
    }
    for (const auto& x : kfg_) {
        for (const auto& y : x.second) {
            ss << "    " << x.second.Key() << " -> " << y << ";\n";
        }
    }
    ss << "}";

    return ss.str();
}

void KFG::WriteDot(const std::filesystem::path& filename) const
{
    std::ofstream outfile{filename};
    outfile << Graph2StringDot();
}

void KFG::WriteUtgsGFA(const std::filesystem::path& filename) const
{
    std::ofstream outfile{filename};
    outfile << DumpGFAUtgs();
}

int32_t KFG::MatchCount(const std::vector<DnaBit>& bits) const
{
    uint32_t nHits = 0;
    for (const auto& b : bits) {
        if (HasNode(b)) {
            ++nHits;
        }
    }

    return nHits;
}

void KFG::DumpHeader() const
{
    for (const auto& i : nameToId_) {
        std::cerr << i.first << " " << i.second << "\n";
    }
}

bool KFG::NextUtg(uint64_t currentNode, std::unordered_set<uint64_t>& seen,
                  std::vector<std::string>& segments, std::vector<std::string>& links) const
{
    if (seen.find(currentNode) != seen.end()) {
        return false;
    }

    std::vector<KFNode> fullPath;

    // first node is a branch node, but we need to include the branch node
    std::vector<KFNode> linearPath = LinearPath(currentNode);

    Utility::MoveAppend(linearPath, fullPath);

    if (fullPath.empty()) {
        fullPath.push_back(kfg_.at(currentNode));
    }

    // We need to get the branch node.
    if (fullPath.back().OutEdgeCount() == 1) {
        uint64_t tail = (*fullPath.back().outEdges_.begin());
        if (tail != fullPath.back().Key() && seen.find(fullPath.back().Key()) == seen.end()) {
            fullPath.push_back(kfg_.at(tail));
        }
    }

    for (const auto& p : fullPath) {
        seen.insert(p.Key());
    }

    auto seq = Vec2String(fullPath);

    if (fullPath.front().InEdgeCount() != 0) {

        auto start = seq.size() - fullPath.size();

        seq = seq.substr(start, fullPath.size());
    }

    double KmerCovSum = 0;
    for (const auto& n : fullPath) {
        KmerCovSum += n.SeqCount();
    }

    std::stringstream segLine;

    segLine << "S\t" << currentNode << "\t" << seq << "\tKC:i:" << fullPath.size()
            << "\tLN:i:" << seq.size() << "\tRC:i:" << KmerCovSum << "\n";

    segments.push_back(segLine.str());

    for (const auto& nn : fullPath.back()) {

        std::stringstream linkLine;
        linkLine << "L\t" << currentNode << "\t+\t" << nn << "\t+\t*\n";
        links.push_back(linkLine.str());

        NextUtg(nn, seen, segments, links);
    }

    return true;
}

std::string KFG::DumpGFAUtgs() const
{
    std::unordered_set<uint64_t> seen;

    std::vector<std::string> segments;
    std::vector<std::string> links;

    std::set<uint64_t> keys;

    //maintain an order for testing and consistency
    for (const auto& node : kfg_) {
        keys.insert(node.first);
    }

    for (const auto& k : keys) {
        if (seen.find(k) != seen.end()) {
            continue;
        }
        auto node = kfg_.at(k);
        if (node.InEdgeCount() != 0) {
            continue;
        }
        NextUtg(node.Key(), seen, segments, links);
    }

    std::ostringstream ss;
    ss << "H\tVN:Z:1.0\n";

    std::sort(segments.begin(), segments.end());
    std::sort(links.begin(), links.end());

    for (const auto& seg : segments) {
        ss << seg;
    }

    for (const auto& lnk : links) {
        ss << lnk;
    }

    return ss.str();
}

std::unordered_map<std::string, uint32_t> KFG::Header() const { return nameToId_; }

}  // namespace Pbmer
}  // namespace PacBio
