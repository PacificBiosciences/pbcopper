// Copyright (c) 2016-2018, Pacific Biosciences of California, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//  * Neither the name of Pacific Biosciences nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY PACIFIC
// BIOSCIENCES AND ITS CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL PACIFIC BIOSCIENCES OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
// USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

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
