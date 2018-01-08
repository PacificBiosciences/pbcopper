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

#ifndef PBCOPPER_DATA_RSMOVIENAME_H
#define PBCOPPER_DATA_RSMOVIENAME_H

#include <iosfwd>
#include <memory>
#include <string>

#include <boost/utility/string_ref.hpp>

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Data {

/// \brief The RSMovieName class provides methods for working with PacBio RSII
///        movie names.
///
class RSMovieName
{
public:
    /// \name Constructors & Related Methods
    /// \{

    // create RSMovieName from string
    explicit RSMovieName(const std::string& name);
    explicit RSMovieName(std::string&& name);

    // create MovieName from name parts
    RSMovieName(const std::string& runStartTime,
                const std::string& instrumentSerialNumber,
                const std::string& smrtCellBarcode,
                const std::string& setNumber,
                const std::string& partNumber);

    RSMovieName(void);
    RSMovieName(const RSMovieName& other);
    RSMovieName(RSMovieName&& other) noexcept;
    RSMovieName& operator=(const RSMovieName& other);
    RSMovieName& operator=(RSMovieName&& other) noexcept;
    ~RSMovieName(void);

    /// \}

public:
    /// \name Name Parts
    /// \{

    boost::string_ref InstrumentSerialNumber(void) const;
    boost::string_ref PartNumber(void) const;
    boost::string_ref RunStartTime(void) const;
    boost::string_ref SetNumber(void) const;
    boost::string_ref SMRTCellBarcode(void) const;

    /// \}

public:
    /// \name Additional Methods & Operators
    /// \{

    bool IsReagentExpired(void) const;
    std::string ToStdString(void) const;

    /// \}

private:
    std::string movieName_;

    struct PartsCache;
    mutable std::unique_ptr<PartsCache> partsCache_;

private:
    void UpdatePartsCache(void) const;
};

/// \name Related Non-members
/// \{


// comparison operators
inline bool operator==(const RSMovieName& lhs, const RSMovieName& rhs);
inline bool operator!=(const RSMovieName& lhs, const RSMovieName& rhs);
inline bool operator<(const RSMovieName& lhs, const RSMovieName& rhs);

// I/O
inline std::ostream& operator<<(std::ostream& os, const RSMovieName& movieName);
inline std::istream& operator>>(std::istream& is, RSMovieName& movieName);

/// \}

} // namespace Data
} // namespace PacBio

#include <pbcopper/data/internal/RSMovieName-inl.h>

#endif // PBCOPPER_DATA_RSMOVIENAME_H
