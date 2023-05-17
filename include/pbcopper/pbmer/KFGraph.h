/*!
   \file KFGraph.h
   \brief A Kmer sequence graph that a attempts to avoid repeat collapse.
   \author Zev Kronenberg
   \date 15/05/2020
*/

#ifndef PBCOPPER_PBMER_KFGRAPH_H
#define PBCOPPER_PBMER_KFGRAPH_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/pbmer/Bubble.h>
#include <pbcopper/pbmer/DnaBit.h>
#include <pbcopper/pbmer/KFNode.h>
#include <pbcopper/pbmer/Mers.h>
#include <pbcopper/pbmer/Parser.h>
#include <pbcopper/third-party/robin_hood/robin_hood.h>

#include <filesystem>
#include <map>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <cstddef>
#include <cstdint>

namespace PacBio {
namespace Pbmer {

class KFG
{
public:
    /*!
       \brief Construct a new knock first graph
       \param kmerSize     kmer size in bp
       \param nr           number of sequences/reads
    */
    KFG(uint8_t kmerSize, std::size_t nr);

public:
    /*!
       \brief Add a DNA string to the knock first graph
       \param bits Dnabit (kmer) vector
       \param rid Read Id / index, one based.
    */
    void AddSeq(const std::vector<PacBio::Pbmer::DnaBit>& bits, std::size_t rid,
                const std::string& rn);

    /*!
       \brief After nodes are removed this method removes edges that link to a deleted node.
    */
    void CleanUpEdges();

    /*!
       \brief Remove nodes with fewer or more than n sequences supporting it.
       \param n   number of sequences
       \param gt  true:filter nodes with >n , false: filter nodes with <n"
    */
    void SeqCountFilter(int n, bool gt);

public:
    /*!
    \brief Prints, a mapping between seqIds and names, the header to stderr.
    */
    void DumpHeader() const;

    /*!
      \brief dump unitigs to GFA string.
    */
    std::string DumpGFAUtgs() const;

    /*!
       \brief Get a list of bubbles (a struct defined in Bubble.h)
       \return simple bubbles
    */
    Bubbles FindBubbles() const;

    /*!
       \brief Get the linear path vector
       \param knNode  starting point of the search
       \return nodes in the path including the starting node
    */
    std::vector<KFNode> LinearPath(const KFNode& node) const;

    /*!
       \brief Get the linear path vector
       \param x  the starting point of the search (lex smaller kmer) - uint64_t
       \return nodes in the path including the starting node
    */
    std::vector<KFNode> LinearPath(uint64_t x) const;

    /*!
       \brief write the graph to a dot file.
    */
    std::string Graph2StringDot() const;

    /*!
       \brief Does a node (kmer/DNAbit) exist in the graph
       \param bit DNAbit of interest
       \return true if node exists
    */
    bool HasNode(const DnaBit& bit) const;

    /*!
       \brief Get the number of in-edges across entire graph
       \return number (sum) of in edges across the graph
    */
    int32_t InEdgeCount() const;

    /*!
       \brief Checks to see if a node is present and then checks the kmers match. It's a nuisance function needed for resolving repeats.
       \param DNAbit node to check
       \param hashedKmer the hashed kmer value, used as the key in the graph rather than the kmer sequence
       \return The node key
    */
    uint64_t Knock(const DnaBit& b, uint64_t hashedKmer) const;

    /*!
       \brief Get the number of matching kmers in the KFG
       \param bits Dnabit (kmer) vector
       \return count of matches
    */
    int32_t MatchCount(const std::vector<DnaBit>& bits) const;

    /*!
       \brief Checks the number of forward and reverse matching nodes in the graph
       \return bool true if there are more reverse complement
    */
    bool MoreRCHits(const std::vector<DnaBit>& bits) const;

    /*!
       \brief Get node count in the graph (DNABit/Kmer count)
       \return number of nodes in the graph
    */
    int32_t NNodes() const;

    /*!
       \brief Resolve repeat function. This function should only be used by advanced users.
    */
    uint64_t OpenDoor(const std::vector<DnaBit>& bits, uint64_t hashedKmer, uint32_t rid,
                      std::size_t i, std::size_t j) const;

    /*!
       \brief Get number of out-edges across the graph
       \return number (sum) of out edges across the graph
    */
    int32_t OutEdgeCount() const;

    /*!
       \brief Checks the the edges describe links to valid nodes (i.e. in the robin_map). Only useful for development.
       \return true if all edges are okay
    */
    bool ValidateEdges() const;

    /*!
       \brief Checks that each node has at least one read id. Only useful for development.
       \return true if all nodes are okay
    */
    bool ValidateLoad() const;

    std::unordered_map<std::string, uint32_t> Header() const;

    /*!
       \brief write the graph to a dot format file.
    */
    void WriteDot(const std::filesystem::path& filename) const;

    /*!
       \brief write the unitigs to a GFA, there are kmer length overlaps on the unitigs.
    */
    void WriteUtgsGFA(const std::filesystem::path& filename) const;

private:
    /*!
       \brief the recursive function to generate unitigs
    */
    bool NextUtg(uint64_t currentNode, std::unordered_set<uint64_t>& seen,
                 std::vector<std::string>& segments, std::vector<std::string>& links) const;

private:
    // the whole graph structure and colors are stored here.
    using rh = robin_hood::unordered_map<uint64_t, KFNode>;
    rh kfg_;
    // kmer size up to 32
    uint8_t kmerSize_;
    std::size_t nReads_;
    // header information
    std::unordered_map<std::string, uint32_t> nameToId_;
    std::unordered_map<uint32_t, std::string> idToName_;

public:
    using iterator = rh::iterator;
    using const_iterator = rh::const_iterator;

    iterator begin() { return kfg_.begin(); }
    iterator end() { return kfg_.end(); }
    const_iterator begin() const { return kfg_.begin(); }
    const_iterator end() const { return kfg_.end(); }
    const_iterator cbegin() const { return kfg_.cbegin(); }
    const_iterator cend() const { return kfg_.cend(); }
};

}  // namespace Pbmer
}  // namespace PacBio

#endif  // PBCOPPER_PBMER_KFGRAPH_H
