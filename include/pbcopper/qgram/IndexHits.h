#ifndef PBCOPPER_QGRAM_INDEXHITS_H
#define PBCOPPER_QGRAM_INDEXHITS_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/qgram/IndexHit.h>

#include <vector>

#include <cassert>
#include <cstddef>

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
    /// \param seqPos
    ///
    IndexHits(const std::vector<IndexHit>* source, const size_t queryPos)
        : IndexHits(source, 0, source->size(), queryPos)
    {}

    ///
    /// \brief IndexHits
    /// \param source
    /// \param beginPos
    /// \param endPos
    ///
    IndexHits(const std::vector<IndexHit>* source, const size_t beginPos, const size_t endPos,
              const size_t queryPos)
        : source_(source), begin_(beginPos), end_(endPos), queryPos_(queryPos)
    {
        assert(source_);
        assert(begin_ <= end_);
    }

public:
    size_t QueryPosition() const { return queryPos_; }

public:
    /// \name STL compatibility
    /// \{

    using iterator = std::vector<IndexHit>::iterator;
    using const_iterator = std::vector<IndexHit>::const_iterator;
    using reference = std::vector<IndexHit>::reference;
    using const_reference = std::vector<IndexHit>::const_reference;
    using size_type = std::vector<IndexHit>::size_type;

    const_iterator begin() const { return source_->begin() + begin_; }
    const_iterator cbegin() const { return source_->cbegin() + begin_; }
    const_iterator end() const { return begin() + size(); }
    const_iterator cend() const { return cbegin() + size(); }
    size_type size() const { return end_ - begin_; }

    const_reference operator[](size_type pos) const noexcept
    {
        return *(source_->begin() + begin_ + pos);
    }

    const_reference at(size_type pos) const
    {
        assert(pos < size());
        return *(source_->begin() + begin_ + pos);
    }

    /// \}

private:
    const std::vector<IndexHit>* source_;
    const size_t begin_;
    const size_t end_;
    const size_t queryPos_;
};

}  // namespace QGram
}  // namespace PacBio

#endif  // PBCOPPER_QGRAM_INDEXHITS_H
