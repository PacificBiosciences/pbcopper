// Author: Derek Barnett

#ifndef PBCOPPER_UTILITY_PARTITIONELEMENTS_H
#define PBCOPPER_UTILITY_PARTITIONELEMENTS_H

#include <pbcopper/PbcopperConfig.h>

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace PacBio {
namespace Utility {

///
/// For partition methods below, select if empty buckets should be kept or discarded.
///
enum class PartitionBehavior
{
    KEEP_EMPTY,
    DISCARD_EMPTY,
};

///
/// Often one needs to to distribute values across a maximum number of buckets,
/// e.g. assigning a number of tasks or data chunks per thread. This method
/// will determine a approximately even spread of input data across buckets.
/// The returned vector contains the * counts * of elements per bucket.
///
/// For example, say you have 20 data chunks to spread across 6 threads, this
/// will return the vector{4,4,3,3,3,3}.
///
/// For small data sets, there may be empty buckets - such as 3 elements for 5
/// buckets. The PartitionBehavior can be used to select whether the empty buckets
/// should be kept {1,1,1,0,0} or discarded {1,1,1}.
///
/// \note This utility should not be confused with std::partition(). It is more
///       akin to a set partition.
///
/// \throw if max buckets is not a positive integer, or if input data size is negative
///
/// \param numElements      input data size
/// \param maxNumBuckets    maximum number of buckets
/// \param behavior         keep or discard empty buckets
///
/// \return vector of counts per bucket
///
inline std::vector<int> PartitionElementCounts(
    int numElements, int maxNumBuckets, PartitionBehavior behavior = PartitionBehavior::KEEP_EMPTY)
{
    // parameter check
    if (maxNumBuckets <= 0) {
        throw std::runtime_error{"[pbcopper] partition element ERROR: max buckets (" +
                                 std::to_string(maxNumBuckets) + ") must greater than zero"};
    }
    if (numElements < 0) {
        throw std::runtime_error{"[pbcopper] partition element ERROR: number of elements (" +
                                 std::to_string(numElements) + ") must not be negative"};
    }

    // quick exit on zero input elements, not an error
    const bool shrinkToFit{behavior == PartitionBehavior::DISCARD_EMPTY};
    if (numElements == 0) {
        return (shrinkToFit ? std::vector<int>{} : std::vector<int>(maxNumBuckets, 0));
    }

    // determine partition values
    const int usingNumBuckets =
        (shrinkToFit ? std::min(maxNumBuckets, numElements) : maxNumBuckets);
    const int minimum = numElements / usingNumBuckets;
    const int modulo = numElements % usingNumBuckets;

    // spread element counts over buckets
    std::vector<int> result(usingNumBuckets, minimum);
    for (int i = 0; i < modulo; ++i)
        ++result[i];
    return result;
}

///
/// Often one needs to to distribute values across a maximum number of buckets,
/// e.g. assigning a number of tasks or data chunks per thread. This method
/// will determine a approximately even spread of input data across buckets and
/// place the elements accordingly.
///
/// For example, say you have a vector of integers {10,20,30,40,50,60,70} to
/// distribute across 3 buckets. The resulting buckets will contain the integer values:
/// {10,20,30}, {40,50}, and {60,70}.
///
/// For small data sets, there may be empty buckets - such as 3 elements for 5
/// buckets. The PartitionBehavior can be used to select whether the empty buckets
/// should be kept or discarded.
///
/// This overload copies input elements into the output container.
///
/// \note This utility should not be confused with std::partition(). It is more
///       akin to a set partition.
///
/// \throw if max buckets is not a positive integer
///
/// \param input            input data values
/// \param maxNumBuckets    maximum number of buckets
/// \param behavior         keep or discard empty buckets
///
/// \return buckets containing elements from input data
///
template <typename T>
inline auto PartitionElements(const T& input, int maxNumBuckets,
                              PartitionBehavior behavior = PartitionBehavior::KEEP_EMPTY)
{
    const auto counts = PartitionElementCounts(input.size(), maxNumBuckets, behavior);
    const bool shrinkToFit{behavior == PartitionBehavior::DISCARD_EMPTY};
    const int numCounts = counts.size();
    if ((numCounts == 0) && shrinkToFit) {
        return std::vector<T>{};
    }

    const int resultSize = (shrinkToFit ? std::min(maxNumBuckets, numCounts) : maxNumBuckets);
    std::vector<T> result(resultSize, T{});
    auto inputIterator = std::cbegin(input);
    for (int i = 0; i < resultSize; ++i) {
        const auto count = counts[i];
        result[i] = T(inputIterator, inputIterator + count);
        inputIterator += count;
    }

    return result;
}

///
/// Often one needs to to distribute values across a maximum number of buckets,
/// e.g. assigning a number of tasks or data chunks per thread. This method
/// will determine a approximately even spread of input data across buckets and
/// place the elements accordingly.
///
/// For example, say you have a vector of integers {10,20,30,40,50,60,70} to
/// distribute across 3 buckets. The resulting buckets will contain the integer values:
/// {10,20,30}, {40,50}, and {60,70}.
///
/// For small data sets, there may be empty buckets - such as 3 elements for 5
/// buckets. The PartitionBehavior can be used to select whether the empty buckets
/// should be kept or discarded.
///
/// This overload moves input elements into the output container.
///
/// \note This utility should not be confused with std::partition(). It is more
///       akin to a set partition.
///
/// \throw if max buckets is not a positive integer
///
/// \param input            input data values
/// \param maxNumBuckets    maximum number of buckets
/// \param behavior         keep or discard empty buckets
///
/// \return buckets containing elements from input data
///
template <typename T>
inline auto PartitionElements(T&& input, int maxNumBuckets,
                              PartitionBehavior behavior = PartitionBehavior::KEEP_EMPTY)
{
    const auto counts = PartitionElementCounts(input.size(), maxNumBuckets, behavior);
    const bool shrinkToFit{behavior == PartitionBehavior::DISCARD_EMPTY};
    const int numCounts = counts.size();
    if ((numCounts == 0) && shrinkToFit) {
        return std::vector<T>{};
    }

    const int resultSize = (shrinkToFit ? std::min(maxNumBuckets, numCounts) : maxNumBuckets);
    std::vector<T> result(resultSize, T{});

    auto inputIterator = std::begin(input);
    using iterator_t = decltype(inputIterator);
    for (int i = 0; i < resultSize; ++i) {
        const auto count = counts[i];
        result[i] = T(std::move_iterator<iterator_t>(inputIterator),
                      std::move_iterator<iterator_t>(inputIterator + count));
        inputIterator += count;
    }

    return result;
}

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_PARTITIONELEMENTS_H