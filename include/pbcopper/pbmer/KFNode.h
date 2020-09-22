/*!
   \file KFNode.h
   \brief Node for knock first graph (KFG)
   \author Zev Kronenberg
   \date 15/05/2020
*/
#ifndef PBCOPPER_PBMER_KFNODE_H
#define PBCOPPER_PBMER_KFNODE_H

#include <pbcopper/PbcopperConfig.h>

#include <cstddef>
#include <cstdint>

#include <iterator>
#include <unordered_set>

#include <boost/dynamic_bitset.hpp>

#include <pbcopper/pbmer/DnaBit.h>

namespace PacBio {
namespace Pbmer {

class KFNode
{
public:
    /*!
       \brief Construct DbgNode
       \param d    dna class
       \param n    number of seq/read ids;
       \param k    node key
    */
    KFNode(const DnaBit& d, size_t n, uint64_t k);

    /*!
       \brief Adds an in edge, encoded as a uint64_t - see KFGraph AddSeq
       \param edge - previous node key
    */
    void AddInEdge(uint64_t e);

    /*!
       \brief Adds an in edge, encoded as a uint64_t - see KFGraph AddSeq
       \param edge - next node key
    */
    void AddOutEdge(uint64_t e);

    /*!
       \brief Adds a read/seq id to the readIds_ variable. Read/Seq ids are one based.
       \param rid  read/seq id
    */
    bool AddLoad(uint32_t rid);

public:
    /*!
       \brief  Is the read/seq id (one based) set
       \return true is the read/seq is represented in the node
    */
    bool ContainsSeq(uint32_t rid) const;

    /*!
       \brief Finds the first read/seq id in the set.
       \return The index of the first on bit, read/seq id
    */
    size_t FirstRId() const;

    /*!
       \return number of in-edges for the node.
    */
    int InEdgeCount() const;

    /*!
       \return The node key - a hashed uint64_t
    */
    uint64_t Key() const;

    /*!
       \return "the uint64_t packed kmer"
    */
    uint64_t Kmer() const;

    /*!
       \return number of out edges
    */
    int OutEdgeCount() const;

    /*!
       \return number of reads/sequences represented in the node.
    */
    int SeqCount() const;

    /*!
       \return "Return the underlying DNAbit kmer object.
    */
    DnaBit Bit() const;

private:
    uint64_t key_;
    DnaBit dna_;
    // Read/Seq ids must be one based - internally they are converted.
    boost::dynamic_bitset<> readIds_;
    // edges are stored in sets
    // TODO sets are are not ideal here, slow?
    using oe = std::unordered_set<uint64_t>;
    oe outEdges_;
    oe inEdges_;

    friend class KFG;

public:
    using iterator = oe::iterator;
    using const_iterator = oe::const_iterator;

    iterator begin() { return outEdges_.begin(); }
    iterator end() { return outEdges_.end(); }
    const_iterator begin() const { return outEdges_.begin(); }
    const_iterator end() const { return outEdges_.end(); }
    const_iterator cbegin() const { return outEdges_.cbegin(); }
    const_iterator cend() const { return outEdges_.cend(); }
};

std::string Vec2String(const std::vector<KFNode>& nodes);

}  // namespace Pbmer
}  // namespace PacBio

#endif  // PBCOPPER_PBMER_KFNODE_H
