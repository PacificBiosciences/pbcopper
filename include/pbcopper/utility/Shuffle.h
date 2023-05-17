#ifndef PBCOPPER_UTILITY_SHUFFLE_H
#define PBCOPPER_UTILITY_SHUFFLE_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/utility/Ssize.h>

#include <iterator>
#include <random>

namespace PacBio {
namespace Utility {
namespace internal {

template <typename Iter>
void ShuffleImpl(Iter begin, std::int64_t seed, std::int64_t size, std::mt19937_64& eng)
{
    const std::uint64_t usingSeed =
        (seed < 0) ? std::mt19937_64::default_seed : static_cast<std::uint64_t>(seed);
    eng.seed(usingSeed);

    for (int i = 0; i < size; ++i) {
        const int j = eng() % size;
        using std::swap;
        swap(*(begin + i), *(begin + j));
    }
}

template <typename Iter>
void ShuffleImpl(Iter begin, std::int64_t seed, std::int64_t size)
{
    std::mt19937_64 eng;
    ShuffleImpl(begin, seed, size, eng);
}

}  // namespace internal

///
/// \brief Portable alternative to std::shuffle, which reorders the elements in
///        a container.
///
/// The behavior of std::shuffle is implementation-defined (not by the standard).
/// This can cause the results to vary in different environments (e.g. CI vs local),
/// even when provided the same URNG/seed, making testing difficult.
///
/// \sa https://en.cppreference.com/w/cpp/algorithm/random_shuffle
///
/// \param container    container to be sorted, in-place
/// \param seed         seed for random number generation. A value of -1 indicates
///                     default seeding, zero or greater for user-defined value.
///
template <typename T>
void Shuffle(T& container, std::int64_t seed = -1)
{
    internal::ShuffleImpl(container.begin(), seed, Utility::Ssize(container));
}

///
/// \brief Portable alternative to std::shuffle, which reorders the elements in
///        a container.
///
/// The behavior of std::shuffle is implementation-defined (not by the standard).
/// This can cause the results to vary in different environments (e.g. CI vs local),
/// even when provided the same URNG/seed, making testing difficult.
///
/// \sa https://en.cppreference.com/w/cpp/algorithm/random_shuffle
///
/// \param container    container to be sorted, in-place
/// \param engine       reusable std::mersenne_twister_engine
/// \param seed         seed for random number generation. A value of -1 indicates
///                     default seeding, zero or greater for user-defined value.
///
template <typename T>
void Shuffle(T& container, std::mt19937_64& eng, std::int64_t seed = -1)
{
    internal::ShuffleImpl(container.begin(), seed, Utility::Ssize(container), eng);
}

///
/// \brief Portable alternative to std::shuffle, which reorders the elements in
///        a container.
///
/// The behavior of std::shuffle is implementation-defined (not by the standard).
/// This can cause the results to vary in different environments (e.g. CI vs local),
/// even when provided the same URNG/seed, making testing difficult.
///
/// \sa https://en.cppreference.com/w/cpp/algorithm/random_shuffle
///
/// \param begin    iterator pointing to the first element
/// \param end      iterator pointing to the end of the range
/// \param seed     seed for random number generation. A value of -1 indicates
///                 default seeding, zero or greater for user-defined value.
///
template <typename Iter>
void Shuffle(Iter begin, Iter end, std::int64_t seed = -1)
{
    internal::ShuffleImpl(begin, seed, std::distance(begin, end));
}

///
/// \brief Portable alternative to std::shuffle, which reorders the elements in
///        a container.
///
/// The behavior of std::shuffle is implementation-defined (not by the standard).
/// This can cause the results to vary in different environments (e.g. CI vs local),
/// even when provided the same URNG/seed, making testing difficult.
///
/// \sa https://en.cppreference.com/w/cpp/algorithm/random_shuffle
///
/// \param begin    iterator pointing to the first element
/// \param end      iterator pointing to the end of the range
/// \param engine   reusable std::mersenne_twister_engine
/// \param seed     seed for random number generation. A value of -1 indicates
///                 default seeding, zero or greater for user-defined value.
///
template <typename Iter>
void Shuffle(Iter begin, Iter end, std::mt19937_64& eng, std::int64_t seed = -1)
{
    internal::ShuffleImpl(begin, seed, std::distance(begin, end), eng);
}

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_SHUFFLE_H
