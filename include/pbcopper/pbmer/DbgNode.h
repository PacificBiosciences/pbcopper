#ifndef PBCOPPER_PBMER_DBGNODE_H
#define PBCOPPER_PBMER_DBGNODE_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/pbmer/DnaBit.h>

#include <boost/dynamic_bitset.hpp>

#include <iterator>

#include <cstddef>
#include <cstdint>

namespace PacBio {
namespace Pbmer {

class DbgNode
{
public:
    ///
    /// Construct DbgNode
    ///
    /// \param d    dna class
    /// \param o    out edges
    /// \param n    number of read ids;
    ///
    DbgNode(const DnaBit& d, uint8_t o, uint32_t n);

    ///
    /// \brief Adds a read id to the readIds_ variable. Read ids are one based.
    ///
    /// \param rid  read id
    ///
    bool AddLoad(uint32_t rid);

    ///
    /// \brief Finds the first read Id set.
    ///
    /// \returns zero if no read ids are set. Read ids are one based.

    std::size_t FirstRId() const;

    ///
    /// \returns Returns the uint64_t packed kmer;
    ///
    uint64_t Kmer() const;

    ///
    /// \brief Uses a bit field to set out edges, possibilities {bit0:A, bit2:C,
    ///        bit3:G, bit4:T}
    ///
    /// \param o    out edges to set: `outEdges_ |= o`
    ///
    void SetEdges(uint8_t o);

    ///
    /// \return number of left edges
    ///
    int LeftEdgeCount() const;

    ///
    /// \return number of right edges
    ///
    int RightEdgeCount() const;

    ///
    /// \return number of edges
    ///
    int TotalEdgeCount() const;

public:
    /// \name Iterable
    /// \{

    class iterator_base
    {
    public:
        using value_type = DnaBit;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        ~iterator_base();
        bool operator==(const iterator_base& other) const noexcept;
        bool operator!=(const iterator_base& other) const noexcept;

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
        using pointer = DnaBit*;
        using reference = DnaBit&;

        iterator();
        iterator(DbgNode& node);
        DnaBit& operator*() noexcept;
        DnaBit* operator->() noexcept;
        iterator& operator++();
        iterator operator++(int);
    };

    struct const_iterator : public iterator_base
    {
    public:
        using pointer = const DnaBit*;
        using reference = const DnaBit&;

        const_iterator();
        const_iterator(const DbgNode& node);
        const DnaBit& operator*() const noexcept;
        const DnaBit* operator->() const noexcept;
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

private:
    DnaBit dna_;
    uint8_t edges_;
    // ReadIds must be one based - internally they are converted.
    boost::dynamic_bitset<> readIds2_;
    friend class Dbg;
};

}  // namespace Pbmer
}  // namespace PacBio

#endif  // PBCOPPER_PBMER_DBGNODE_H
