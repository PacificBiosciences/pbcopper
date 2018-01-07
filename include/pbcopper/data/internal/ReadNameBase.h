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

#ifndef PBCOPPER_DATA_READNAMEBASE_H
#define PBCOPPER_DATA_READNAMEBASE_H

#include <pbcopper/data/CCSTag.h>
#include <pbcopper/data/Interval.h>
#include <pbcopper/data/MovieName.h>
#include <pbcopper/data/Position.h>
#include <pbcopper/data/Zmw.h>

namespace PacBio {
namespace Data {
namespace internal {

template<typename MovieNameType>
class ReadNameBase
{
public:
    /// \name Constructors & Related Methods
    /// \{

    // create ReadNameBase from string
    explicit ReadNameBase(const std::string& name);
    explicit ReadNameBase(std::string&& name);

    // create ReadName from name parts
    ReadNameBase(const MovieNameType& movieName,
                 const PacBio::Data::Zmw& zmw,
                 const Interval<Position>& queryInterval);

    ReadNameBase(const MovieNameType& movieName,
                 const PacBio::Data::Zmw& zmw,
                 const Position& queryStart,
                 const Position& queryEnd);

    ReadNameBase(const MovieNameType& movieName,
                 const PacBio::Data::Zmw& zmw,
                 const CCSTag ccs);

    ReadNameBase(void);
    ReadNameBase(const ReadNameBase<MovieNameType>& other) = default;
    ReadNameBase(ReadNameBase<MovieNameType>&& other) = default;
    ReadNameBase<MovieNameType>& operator=(const ReadNameBase<MovieNameType>& other) = default;
    ReadNameBase<MovieNameType>& operator=(ReadNameBase<MovieNameType>&& other) = default;
    ~ReadNameBase(void) = default;

    /// \}

public:
    /// \name Name Parts
    /// \{

    MovieNameType MovieName(void) const;
    PacBio::Data::Zmw Zmw(void) const;
    Interval<Position> QueryInterval(void) const;
    Position QueryStart(void) const;
    Position QueryEnd(void) const;

    /// \}

public:
    /// \name Additional Methods & Operators
    /// \{

    bool IsCCS(void) const;
    std::string ToString(void) const;

    bool operator==(const ReadNameBase<MovieNameType>& other) const;
    bool operator<(const ReadNameBase<MovieNameType>& other) const;

    /// \}

private:
    MovieNameType movieName_;
    PacBio::Data::Zmw zmw_;
    std::unique_ptr<Interval<Position>> queryInterval_;

private:
    void Check(void) const;
    void FromString(std::string&& name);
};

// add'l comparison operators
template<typename MovieNameType>
inline bool operator!=(const ReadNameBase<MovieNameType>& lhs,
                       const ReadNameBase<MovieNameType>& rhs);

// I/O
template<typename MovieNameType>
inline std::ostream& operator<<(std::ostream& os,
                                const ReadNameBase<MovieNameType>& readName);

template<typename MovieNameType>
inline std::istream& operator>>(std::istream& is,
                                ReadNameBase<MovieNameType>& readName);

} // namespace internal
} // namespace Data
} // namespace PacBio

#include <pbcopper/data/internal/ReadNameBase-inl.h>

#endif // PBCOPPER_DATA_READNAMEBASE_H
