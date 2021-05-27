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
    /// \param[in] codedData    encoded data
    /// \returns Frames object
    ///
    static Frames Decode(const std::vector<uint8_t>& codedData);

    /// \brief Constructs a Frames object from encoded (lossy, 8-bit) data, using
    ///        decoder provided.
    ///
    /// \param[in] codedData    encoded data
    /// \param[in] decoder      frame codec
    /// \returns Frames object
    ///
    template <typename T>
    static Frames Decode(const std::vector<uint8_t>& codedData, const T& decoder);

    /// \brief Creates encoded, compressed frame data from raw input data.
    ///
    /// \param[in] frames   raw frame data
    /// \returns lossy, 8-bit encoded frame data
    ///
    static std::vector<uint8_t> Encode(const std::vector<uint16_t>& frames);

    /// \brief Creates encoded, compressed frame data from raw input data, using
    ///        encoder provided
    ///
    /// \param[in] frames   raw frame data
    /// \param[in] encoder  frame codec
    /// \returns lossy, 8-bit encoded frame data
    ///
    template <typename T>
    static std::vector<uint8_t> Encode(const std::vector<uint16_t>& frames, const T& encoder);

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
    const std::vector<uint16_t>& Data() const;
    std::vector<uint16_t>& Data();

    PBCOPPER_DEPRECATED std::vector<uint16_t>& DataRaw();

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

    using std::vector<uint16_t>::const_iterator;
    using std::vector<uint16_t>::iterator;
    using std::vector<uint16_t>::value_type;
    using std::vector<uint16_t>::at;
    using std::vector<uint16_t>::operator[];
    using std::vector<uint16_t>::cbegin;
    using std::vector<uint16_t>::cend;
    using std::vector<uint16_t>::crbegin;
    using std::vector<uint16_t>::crend;
    using std::vector<uint16_t>::begin;
    using std::vector<uint16_t>::end;
    using std::vector<uint16_t>::rbegin;
    using std::vector<uint16_t>::rend;
    using std::vector<uint16_t>::size;
    using std::vector<uint16_t>::empty;
    using std::vector<uint16_t>::clear;
    using std::vector<uint16_t>::push_back;
    using std::vector<uint16_t>::emplace_back;
    using std::vector<uint16_t>::reserve;
    using std::vector<uint16_t>::resize;
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

template <typename T>
Frames Frames::Decode(const std::vector<uint8_t>& encodedData, const T& decoder)
{
    return decoder.Decode(encodedData);
}

template <typename T>
std::vector<uint8_t> Frames::Encode(const std::vector<uint16_t>& rawFrames, const T& encoder)
{
    return encoder.Encode(rawFrames);
}

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_FRAMES_H
