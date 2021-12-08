#ifndef PBCOPPER_DATA_INTERVAL_TREE_H
#define PBCOPPER_DATA_INTERVAL_TREE_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/data/Interval.h>
#include <pbcopper/data/Position.h>

#include <set>
#include <string>

#include <cstddef>

namespace PacBio {
namespace Data {

class IntervalTree
{
private:
    struct WeakIntervalOrdering
    {
        bool operator()(const Interval& lhs, const Interval& rhs) const noexcept;
    };

    using StorageType = std::multiset<Interval, WeakIntervalOrdering>;

public:
    using iterator = StorageType::iterator;
    using const_iterator = StorageType::const_iterator;

    void Insert(const Interval& interval);

    IntervalTree Gaps() const;
    IntervalTree Gaps(const Interval& interval) const;

    bool Contains(Position value) const;

    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;

    size_t size() const;

    static IntervalTree FromString(const std::string& str);

private:
    StorageType storage;
};

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_INTERVAL_TREE_H
