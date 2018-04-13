// Author: Derek Barnett

#ifndef PBCOPPER_DATA_INTERVAL_INL_H
#define PBCOPPER_DATA_INTERVAL_INL_H

#include <pbcopper/data/Interval.h>

namespace PacBio {
namespace Data {

template <typename T>
inline Interval<T>::Interval(void) : data_(boost::icl::discrete_interval<T>::right_open(0, 0))
{
}

template <typename T>
inline Interval<T>::Interval(const T val)
    : data_(boost::icl::discrete_interval<T>::right_open(val, val + 1))
{
}

template <typename T>
inline Interval<T>::Interval(const T start, const T end)
    : data_(boost::icl::discrete_interval<T>::right_open(start, end))
{
}

template <typename T>
inline Interval<T>::Interval(const Interval<T>& other)
    : data_(boost::icl::discrete_interval<T>::right_open(other.Start(), other.End()))
{
}

template <typename T>
inline Interval<T>::Interval(Interval<T>&& other)
    : data_(boost::icl::discrete_interval<T>::right_open(other.Start(), other.End()))
{
}

template <typename T>
inline bool Interval<T>::operator==(const Interval<T>& other) const
{
    return data_ == other.data_;
}

template <typename T>
inline bool Interval<T>::operator!=(const Interval<T>& other) const
{
    return !(data_ == other.data_);
}

template <typename T>
inline bool Interval<T>::CoveredBy(const Interval<T>& other) const
{
    return boost::icl::within(data_, other.data_);
}

template <typename T>
inline bool Interval<T>::Covers(const Interval<T>& other) const
{
    return boost::icl::contains(data_, other.data_);
}

template <typename T>
inline T Interval<T>::End(void) const
{
    return data_.upper();
}

template <typename T>
inline Interval<T>& Interval<T>::End(const T& end)
{
    data_ = boost::icl::discrete_interval<T>::right_open(data_.lower(), end);
    return *this;
}

template <typename T>
inline bool Interval<T>::Intersects(const Interval<T>& other) const
{
    return boost::icl::intersects(data_, other.data_);
}

template <typename T>
inline bool Interval<T>::IsValid(void) const
{
    return !boost::icl::is_empty(data_);
}

template <typename T>
inline size_t Interval<T>::Length(void) const
{
    return boost::icl::length(data_);
}

template <typename T>
inline T Interval<T>::Start(void) const
{
    return data_.lower();
}

template <typename T>
inline Interval<T>& Interval<T>::Start(const T& start)
{
    data_ = boost::icl::discrete_interval<T>::right_open(start, data_.upper());
    return *this;
}

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_INTERVAL_INL_H
