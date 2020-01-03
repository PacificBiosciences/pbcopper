// Authors: Chris Dunn, Zev Kronenberg, Derek Barnett

#ifndef PBMER_DBG_H
#define PBMER_DBG_H

#include <pbcopper/PbcopperConfig.h>

#include <cstdint>

#include <bitset>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <unordered_set>

#include <pbcopper/pbmer/DnaBit.h>
#include <pbcopper/pbmer/Mers.h>
#include <pbcopper/pbmer/Parser.h>

#include <pbcopper/third-party/robin_map.h>

namespace PacBio {
namespace Pbmer {

/*      BubbleInfo

    std::string:
        KMER + "L" or KMER + "R"

        The KMER is the head of the bubble (e.i `Y` fork).
        The L and R suffix describe which branch of the bubble.

    std::vector<std::tuple<int32_t, int, int>>>
       READID, KMER_COUNT, PATH_LENGHT

       READID documents which reads are in the path.
       KMER_COUNT documents the number of kmers for a given read over the paths
       PATH_LENGHT documents the length of a given path. For example,
       if KMER_COUNT/PATH_LENGHT == 1, then you know the read completely spans
       the path.
 */

using BubbleInfo = std::map<std::string, std::vector<std::tuple<int32_t, int, int>>>;

class DbgNode
{
private:
    DnaBit dna_;
    std::bitset<4> inEdges_;
    std::bitset<4> outEdges_;
    std::set<int32_t> readIds_;
    friend class Dbg;

public:
    /**
     * Constructor
     * @param d - dna class
     * @param o - out edges
     * @param i - in edges
     * @param l - readIds
     */
    DbgNode(const DnaBit& d, uint8_t o, uint8_t i, int32_t l);
    /**
     * Adds a read id to the readIds_ variable.
     * @param rid - int32_t readId
     */
    void AddLoad(int32_t rid);
    /**
     * Uses a bit field to set out edges, possibilities {bit0:A, bit2:C, bit3:G, bit4:T}
     * @param o - uint8_t out edges to set: `outEdges_ |= o`
     */
    void SetOutEdges(uint8_t o);
    /**
     * Uses a bit field to set in edges, possibilities {bit0:A, bit2:C, bit3:G, bit4:T}
     * @param i - uint8_t in edges to set: `inEdges_ |= i`
     */
    void SetInEdges(uint8_t i);

public:
    /// \name Iterable
    /// \{

    class iterator_base
    {
    public:
        ~iterator_base();
        bool operator==(const iterator_base& other) const;
        bool operator!=(const iterator_base& other) const;

    protected:
        iterator_base();
        iterator_base(DbgNode& node);
        void LoadNext();

    protected:
        DbgNode* node_ = nullptr;
        uint8_t index_ = 0;
        DnaBit value_;
    };

    class iterator : public iterator_base
    {
    public:
        iterator();
        iterator(DbgNode& node);
        DnaBit& operator*();
        DnaBit* operator->();
        iterator& operator++();
        iterator operator++(int);
    };

    struct const_iterator : iterator_base
    {
    public:
        const_iterator();
        const_iterator(const DbgNode& node);
        const DnaBit& operator*() const;
        const DnaBit* operator->() const;
        const_iterator& operator++();
        const_iterator operator++(int);
    };

    const_iterator begin() const;
    const_iterator cbegin() const;
    iterator begin();

    const_iterator end() const;
    const_iterator cend() const;
    iterator end();

    /// \}
};

class Dbg
{
public:
    /**
     * Adds a Mers object to dbg
     * @param  m   - The Mers object
     * @param  rid - Read id
     * @return Returns one if everything is okay. Negative one means kmer is too
     * large. Negative two means the kmers length is not odd.
     */
    int AddKmers(const PacBio::Pbmer::Mers& m, const int32_t rid);

    /**
     * Iterates over node kmers and checks for all possible out/in bases
     * {A, C, G, T} and sets the out/in edges. Once this is done edges are set.
     */
    void BuildEdges();

    /**
     * Number of nodes in the graph
     * @return size_t - n. nodes.
     */
    size_t NNodes() const;

    /**
     * Constructor for the de bruijn graph.
     * @param kmerSize - the kmers size in bp.
     */
    Dbg(uint8_t kmerSize);

    /**
     * Resets all the edges to zero. Meaning no outgoing/incoming edges.
     */
    void ResetEdges();

    /**
     * Writes the graph to dot format
     * @param fn - filename
     */
    void WriteGraph(std::string fn);

    /**
     * Remove kmers with fewer than N reads covering it. This resets the edges
     * @param n - read count per kmer to filter on.
     */
    void FrequencyFilterNodes(unsigned long n);

    /**
     * Checks that each node has at least one read id.
     * @return - true if all nodes are okay
     */
    bool ValidateLoad() const;

    /**
     * Checks the the edges describe links to valid nodes
     * (i.e in the robin_map).
     * @return - true if all edges are okay
     */
    bool ValidateEdges() const;

    /**
     * Prints the nodes to std::cerr
     */
    void DumpNodes() const;

    /**
     * [GetLinearPath description]
     * @param  niby - The starting point of the search
     * @param  hit  - This is the merging point, specific for bubble detection
     * @return      - The nodes in the path
     */
    std::vector<uint64_t> GetLinearPath(const DnaBit& niby, uint64_t& hit) const;

    /**
     * Finds and returns simple bubbles - see BubbleInfo
     * @return - BubbleInfo
     */
    BubbleInfo GetBubbles() const;

    /**
     * Removes simple spurs (out edge == 2) from graph. Ties are not resolved.
     * @param  maxLength - only spurs shorter than `maxLength` will be removed.
     * @return     number of spurs trimmed
     */
    int RemoveSpurs(unsigned int maxLength);

private:
    // I'm not sure that this container is going to be efficient, specifically
    // the hash function worries me.
    tsl::robin_map<uint64_t, DbgNode> dbg_;
    uint8_t kmerSize_;
};

}  // namespace Pbmer
}  // namespace PacBio

#endif  // PBMER_DGB_H
