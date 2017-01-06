#ifndef PBCOPPER_QGRAM_INDEXHITS_H
#define PBCOPPER_QGRAM_INDEXHITS_H

#include "pbcopper/qgram/IndexHit.h"
#include <vector>
#include <cassert>

namespace PacBio {
namespace QGram {

///
/// \brief The IndexHits class provides a "view" onto a contiguous run of
///        IndexHit objects for a given q-gram.
///
/// \note An instance of this class must not outlive the Index that generated it.
///
class IndexHits
{
public:

    ///
    /// \brief IndexHits
    /// \param source
    ///
    IndexHits(const std::vector<IndexHit>* source)
        : IndexHits(source, 0, source->size())
    { }

    ///
    /// \brief IndexHits
    /// \param source
    /// \param begin
    /// \param end
    ///
    IndexHits(const std::vector<IndexHit>* source, size_t begin, size_t end)
        : source_(source)
        , begin_(begin)
        , end_(end)
    {
        assert(source);
        assert(begin <= end);
    }

    IndexHits(const IndexHits&) = default;
    IndexHits(IndexHits&&) = default;
    ~IndexHits(void) = default;

public:
    /// \name STL compatibility
    /// \{

    using iterator        = std::vector<IndexHit>::iterator;
    using const_iterator  = std::vector<IndexHit>::const_iterator;
    using reference       = std::vector<IndexHit>::reference;
    using const_reference = std::vector<IndexHit>::const_reference;
    using size_type       = std::vector<IndexHit>::size_type;

    const_iterator begin(void) const  { return source_->begin() + begin_; }
    const_iterator cbegin(void) const { return source_->cbegin() + begin_; }
    const_iterator end(void) const    { return begin() + size(); }
    const_iterator cend(void) const   { return cbegin() + size(); }
    size_type size(void) const        { return end_ - begin_; }

    const_reference operator[]( size_type pos ) const
    {
        return *(source_->begin()+begin_+pos);
    }

    const_reference at( size_type pos ) const
    {
        assert(pos < size());
        return *(source_->begin()+begin_+pos);
    }

    /// \}

private:
    const std::vector<IndexHit>* source_;
    const size_t begin_;
    const size_t end_;
};

} // namespace QGram
} // namespace PacBio

#endif // PBCOPPER_QGRAM_INDEXHITS_H
