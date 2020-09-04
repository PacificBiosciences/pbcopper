// Author: Derek Barnett

#ifndef PBCOPPER_DATA_FRAMES_H
#define PBCOPPER_DATA_FRAMES_H

#include <pbcopper/PbcopperConfig.h>

#include <cstddef>
#include <cstdint>

#include <iosfwd>
#include <vector>

#include <pbcopper/data/FrameCodec.h>

namespace PacBio {
namespace Data {

/// \brief The Frames class represents pulse frame data.
///
/// Frame data may be stored in either their raw, 16-bit values or
/// using a lossy, 8-bit compression scheme.
///
/// This class is used to store the data and convert between the 2 storage types.
///
class Frames : private std::vector<uint16_t>
{
public:
    /// \name Conversion Methods
    /// \{

    /// \brief Constructs a Frames object from encoded (lossy, 8-bit) data.
    ///
    /// \note This method should probably not be needed often by client code
    ///       working with frame data. It exists primarily for (internal)
    ///       parsing & interpretation of the %BAM file contents. The method is
    ///       available, though, should the conversion operation be needed.
    ///
    /// \param[in] codedData    encoded data
    /// \returns Frames object
    ///
    static Frames Decode(const std::vector<uint8_t>& codedData);

    /// \brief Creates encoded, compressed frame data from raw input data.
    ///
    /// \param[in] frames   raw frame data
    /// \returns lossy, 8-bit encoded frame data
    ///
    static std::vector<uint8_t> Encode(const std::vector<uint16_t>& frames);

    /// \}

public:
    /// \name Constructors & Related Methods
    /// \{

    Frames(std::vector<uint16_t> frames) noexcept;

    Frames() = default;

    using std::vector<uint16_t>::vector;

    /// \}

public:
    /// \name Access Data
    /// \{

    /// \returns Frame data in expanded (not encoded) form
    std::vector<uint16_t>& DataRaw();
    const std::vector<uint16_t>& Data() const;

    using std::vector<uint16_t>::at;

    using std::vector<uint16_t>::operator[];

    /// \}

public:
    /// \name Conversion Methods
    /// \{

    /// \returns Frame data in (lossy, 8-bit) encoded form.
    std::vector<uint8_t> Encode(FrameCodec codec = FrameCodec::V1) const;

    /// \}

public:
    /// \name Comparison Operators
    /// \{

    bool operator==(const Frames& other) const noexcept;
    bool operator!=(const Frames& other) const noexcept;

    /// \}

public:
    /// \name STL Compatbility
    /// \{

    /// \returns Type of the underlying const_iterator.
    using std::vector<uint16_t>::const_iterator;

    /// \returns Type of the underlying iterator.
    using std::vector<uint16_t>::iterator;

    /// \returns Type of the actual elements (uint16_t).
    using std::vector<uint16_t>::value_type;

    /// \returns A const_iterator to the beginning of the sequence.
    using std::vector<uint16_t>::cbegin;

    /// \returns A const_iterator to the element past the end of the sequence.
    using std::vector<uint16_t>::cend;

    /// \returns A const_iterator to the beginning of the reverse sequence.
    using std::vector<uint16_t>::crbegin;

    /// \returns A const_iterator to the element past the end of the reverse sequence.
    using std::vector<uint16_t>::crend;

    /// \returns A (const_)iterator to the beginning of the sequence.
    using std::vector<uint16_t>::begin;

    /// \returns A (const_)iterator to the element past the end of the sequence.
    using std::vector<uint16_t>::end;

    /// \returns A (const_)iterator to the beginning of the reverse sequence.
    using std::vector<uint16_t>::rbegin;

    /// \returns A (const_)iterator to the element past the end of the reverse sequence.
    using std::vector<uint16_t>::rend;

    /// \returns The number of frame data points.
    using std::vector<uint16_t>::size;

    /// \returns True if the container is empty, false otherwise.
    using std::vector<uint16_t>::empty;

    /// Clears all frames.
    using std::vector<uint16_t>::clear;

    /// Emulates std::vector::push_back
    using std::vector<uint16_t>::push_back;

    /// Emulates std::vector::emplace_back
    using std::vector<uint16_t>::emplace_back;

    /// Emulates std::vector::reserve
    using std::vector<uint16_t>::reserve;

    /// Emulates std::vector::resize
    using std::vector<uint16_t>::resize;

    /// Emulates std::vector::insert
    using std::vector<uint16_t>::insert;

    /// \}

public:
    /// \name Access Data
    /// \{

    /// Sets this record's data.
    ///
    /// \param[in] frames data in expanded (not encoded) form
    /// \returns reference to this object
    ///
    Frames& Data(std::vector<uint16_t> frames);

    /// \}
};

std::ostream& operator<<(std::ostream& os, const Frames& frames);

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_FRAMES_H
