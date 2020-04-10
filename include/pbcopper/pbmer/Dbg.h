// Authors: Chris Dunn, Zev Kronenberg, Derek Barnett

#ifndef PBCOPPER_PBMER_DBG_H
#define PBCOPPER_PBMER_DBG_H

#include <pbcopper/PbcopperConfig.h>

#include <cstddef>
#include <cstdint>

#include <map>
#include <string>
#include <tuple>
#include <vector>

#include <pbcopper/pbmer/DbgNode.h>
#include <pbcopper/pbmer/DnaBit.h>
#include <pbcopper/pbmer/Mers.h>
#include <pbcopper/pbmer/Parser.h>
#include <pbcopper/third-party/robin_hood/robin_hood.h>

namespace PacBio {
namespace Pbmer {

// simple structure that returns the bubble sequences and readIds across the paths
struct BubbleInfo
{
    // sequence of left path
    std::string LSeq;
    // sequence of right path
    std::string RSeq;
    // count of kmers along left paths
    uint32_t LKmerCount;
    // count of kmers along right path
    uint32_t RKmerCount;

    // READID, KMER_COUNT
    std::vector<std::tuple<uint32_t, int>> LData;
    std::vector<std::tuple<uint32_t, int>> RData;
};

using Bubbles = std::vector<BubbleInfo>;

class Dbg
{
public:
    ///
    /// Construct a new De Bruijn graph
    ///
    /// \param kmerSize     kmer size in bp
    /// \param nr           number of sequences/reads
    ///
    Dbg(uint8_t kmerSize, uint32_t nr);

    ///
    /// Adds a Mers object to dbg
    ///
    /// \param m    Mers object
    /// \param rid  read id
    /// \return      1 : everything is okay.
    ///             -1 : kmer is too large.
    ///             -2 : kmer length is not odd.
    ///
    int AddKmers(const PacBio::Pbmer::Mers& m, const uint32_t rid);

    void AddKmers(std::vector<BI>& kmers, uint32_t minFreqCutoff);

    void AddVerifedKmerPairs(std::vector<PacBio::Pbmer::DnaBit>& bits, const uint32_t rid);

    ///
    /// Iterates over node kmers and checks for all possible out/in bases
    /// {A, C, G, T} and sets the out/in edges. Once this is done edges are set.
    ///
    void BuildEdges();

    ///
    /// Iterates over node kmers and checks for all possible out/in bases
    /// {A, C, G, T} and sets the out/in edges based on neighbors.
    ///
    std::vector<uint8_t> BuildVerifiedEdges(const std::vector<PacBio::Pbmer::DnaBit>& bits);

    ///
    /// \return number of nodes in the graph
    ///
    size_t NNodes() const;

    ///
    /// \return number of edges
    ///
    size_t NEdges() const;

    ///
    /// Resets all the edges to zero, meaning no outgoing/incoming edges.
    ///
    void ResetEdges();

    ///
    /// Writes graph to dot format
    ///
    /// \param fn   filename
    ///
    void WriteGraph(std::string fn);

    ///
    /// Remove kmers with fewer than N reads covering it. This resets the edges
    ///
    /// \param n    read count per kmer to filter on
    ///
    void FrequencyFilterNodes(unsigned long n);

    void FrequencyFilterNodes2(unsigned long n);

    ///
    /// \param gt   greater than filter or less than filter
    ///
    void FrequencyFilterNodes2(unsigned long n, bool gt);

    ///
    /// Checks that each node has at least one read id.
    ///
    /// \return true if all nodes are okay
    ///
    bool ValidateLoad() const;

    ///
    /// Checks the the edges describe links to valid nodes (i.e. in the robin_map).
    ///
    /// \return true if all edges are okay
    ///
    bool ValidateEdges() const;

    ///
    /// Prints the nodes to std::cerr
    ///
    void DumpNodes() const;

    ///
    /// \brief Get the linear path vector
    ///
    /// \param niby  starting point of the search
    ///
    /// \return nodes in the path including the starting node
    ///
    std::vector<DnaBit> GetLinearPath(const DnaBit& niby) const;

    ///
    /// \brief Get the linear path vector
    ///
    /// \param x  the starting point of the search (lex smaller kmer) - uint64_t
    std::vector<DnaBit> GetLinearPath(uint64_t x) const;

    ///
    /// \return simple bubbles
    ///
    Bubbles GetBubbles() const;

    ////
    /// Removes simple spurs (out edge == 2) from graph. Ties are not resolved.
    ///
    /// \param maxLength    only spurs shorter than `maxLength` will be removed
    /// \return number of spurs trimmed
    ///
    int RemoveSpurs(unsigned int maxLength);

    ///
    /// \return dot formatted string from the graph, useful for testing
    ///
    std::string Graph2StringDot();

    ///
    /// Tests if two nodes are connected via a third node
    ///
    /// \param n1       kmer1 - node key (lex smaller kmer) - uint64_t
    /// \param n2       kmer2 - node key (lex smaller kmer) - uint64_t
    /// \param shared   node - uint64_t, the common neighbor
    ///
    /// \return true if two nodes share a neighbor
    ///
    bool OneIntermediateNode(uint64_t n1, uint64_t n2, uint64_t* shared) const;

private:
    // the whole graph structure and colors are stored here.
    robin_hood::unordered_map<uint64_t, DbgNode> dbg_;
    // kmer size up to 32
    uint8_t kmerSize_;
    uint32_t nReads_;
};

}  // namespace Pbmer
}  // namespace PacBio

#endif  // PBCOPPER_PBMER_DBG_H
