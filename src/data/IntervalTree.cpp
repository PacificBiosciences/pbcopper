#include <pbcopper/data/IntervalTree.h>

#include <algorithm>
#include <vector>

#include <boost/algorithm/string.hpp>

namespace PacBio {
namespace Data {

bool IntervalTree::WeakIntervalOrdering::operator()(const Interval& lhs, const Interval& rhs) const
    noexcept
{
    return lhs.Start() < rhs.Start();
}

void IntervalTree::Insert(const Interval& interval)
{
    auto it = storage.insert(interval);

    // check to see if we overlap the previous element,
    // if we do, start our merge loop from there
    if (it != begin()) {
        const_iterator prev = std::prev(it);

        if (prev->Overlaps(*it)) {
            it = prev;
        }
    }

    while (it != end()) {
        const_iterator nx = std::next(it);

        if (nx != end() && it->Overlaps(*nx)) {
            const auto u = it->Union(*nx);
            it = storage.erase(it);
            it = storage.erase(it);
            it = storage.insert(it, u);
        } else {
            break;
        }
    }
}

IntervalTree IntervalTree::Gaps() const
{
    IntervalTree gaps;

    for (auto it = begin(); it != end(); ++it) {
        const_iterator nx = std::next(it);

        if (nx == end()) {
            break;
        }

        gaps.Insert(Interval(it->End(), nx->Start()));
    }

    return gaps;
}

IntervalTree IntervalTree::Gaps(const Interval& interval) const
{
    auto left = begin();
    auto right = end();

    // if we're empty (left == right == end()) or we don't overlap the interval
    // return just the provided interval
    if (left == right || !interval.Overlaps(Interval(left->Start(), (--right)->End()))) {
        IntervalTree gaps;
        gaps.Insert(interval);
        return gaps;
    }

    IntervalTree gaps = Gaps();

    if (interval.Start() < left->Start()) {
        gaps.Insert(Interval(interval.Start(), left->Start()));
    }

    if (right->End() < interval.End()) {
        gaps.Insert(Interval(right->End(), interval.End()));
    }

    return gaps;
}

bool IntervalTree::Contains(const Position value) const
{
    const_iterator it = std::lower_bound(begin(), end(), Interval(value, value + 1));

    if (it != begin()) {
        it = std::prev(it);
    }

    for (; it != end() && it->Start() <= value; ++it) {
        if (it->Contains(value)) {
            return true;
        }
    }

    return false;
}

IntervalTree::iterator IntervalTree::begin() { return storage.begin(); }
IntervalTree::const_iterator IntervalTree::begin() const { return storage.begin(); }
IntervalTree::iterator IntervalTree::end() { return storage.end(); }
IntervalTree::const_iterator IntervalTree::end() const { return storage.end(); }

size_t IntervalTree::size() const { return storage.size(); }

IntervalTree IntervalTree::FromString(const std::string& str)
{
    std::vector<std::string> components;
    boost::split(components, str, boost::is_any_of(","));
    IntervalTree tree;
    for (const auto& component : components) {
        tree.Insert(Interval::FromString(component));
    }
    return tree;
}

}  // namespace Data
}  // namespace PacBio
