#ifndef PBCOPPER_ALIGN_SEEDS_H
#define PBCOPPER_ALIGN_SEEDS_H

#include <functional>
#include <set>
#include <vector>

#include "pbcopper/align/Seed.h"

namespace PacBio {
namespace Align {

///
/// \brief The Seeds class represents a container of Seed objects.
///
class Seeds
{
public:
    Seeds(void);

    Seeds(const Seeds&) = default;
    Seeds(Seeds&&) = default;
    Seeds& operator=(const Seeds&) = default;
    Seeds& operator=(Seeds&&) = default;
    ~Seeds(void) = default;

public:
    ///
    /// \brief AddSeed
    ///
    /// Inserts seed into container.
    ///
    /// \param s
    /// \return
    ///
    bool AddSeed(const Seed& s);

    ///
    /// \brief TryMergeSeed
    ///
    /// Attempts to merge seed \p s with a suitable seed in the container. Seeds
    /// must overlap to be merge-able.
    ///
    /// \param s
    /// \return true if seed was merged into existing seed or not, false if no
    ///         suitable candidate could be found
    ///
    bool TryMergeSeed(const Seed& s);

public:
    // STL compatibility
    using container_t     = std::multiset<Seed>;

    using iterator        = container_t::iterator;
    using const_iterator  = container_t::const_iterator;
    using reference       = container_t::reference;
    using const_reference = container_t::const_reference;
    using size_type       = container_t::size_type;

    iterator       begin(void)        { return data_.begin(); }
    const_iterator begin(void) const  { return data_.begin(); }
    const_iterator cbegin(void) const { return data_.cbegin(); }

    iterator       end(void)          { return data_.end(); }
    const_iterator end(void) const    { return data_.end(); }
    const_iterator cend(void) const   { return data_.cend(); }

    const_reference back(void) const  { return *data_.crbegin(); }
    const_reference front(void) const { return *data_.cbegin(); }

    bool      empty(void) const { return data_.empty(); }
    size_type size(void) const  { return data_.size(); }

private:
    container_t data_;
};

} // namespace Align
} // namespace PacBio

#include "pbcopper/align/internal/Seeds-inl.h"

#endif // PBCOPPER_ALIGN_SEEDS_H
