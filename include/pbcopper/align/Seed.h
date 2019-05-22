// Author: Derek Barnett

#ifndef PBCOPPER_ALIGN_SEED_H
#define PBCOPPER_ALIGN_SEED_H

#include <cstdint>

#include <pbcopper/PbcopperConfig.h>

namespace PacBio {
namespace Align {

///
/// \brief The Seed class
///
class Seed
{
public:
    ///
    /// \brief Seed
    ///
    Seed();

    ///
    /// \brief Seed
    /// \param beginPosH
    /// \param beginPosV
    /// \param seedLength
    ///
    Seed(const uint64_t beginPosH, const uint64_t beginPosV, const uint64_t seedLength);

    ///
    /// \brief Seed
    /// \param beginPosH
    /// \param beginPosV
    /// \param endPosH
    /// \param endPosV
    ///
    Seed(const uint64_t beginPosH, const uint64_t beginPosV, const uint64_t endPosH,
         const uint64_t endPosV);

    Seed(const Seed&) = default;
    Seed(Seed&&) noexcept = default;
    Seed& operator=(const Seed&) = default;
    Seed& operator=(Seed&&) noexcept = default;
    ~Seed() = default;

public:
    ///
    /// \brief BeginDiagonal
    /// \return
    ///
    int64_t BeginDiagonal() const;

    ///
    /// \brief BeginPositionH
    /// \return
    ///
    uint64_t BeginPositionH() const;

    ///
    /// \brief BeginPositionV
    /// \return
    ///
    uint64_t BeginPositionV() const;

    ///
    /// \brief Diagonal
    ///
    /// Calculate number of bases between a seed and the diagonal
    /// axis of the matrix it's in as extended outward from the
    /// upper-left-hand corner of the matrix toward the lower right.
    /// High numbers are closer to the upper-right, negative numbers
    /// closer to the lower-left.
    ///
    /// \return int64_t The distance from the seed's upper-left corner and
    ///                 the diagonal axis of the matrix.
    ///
    int64_t Diagonal() const;

    ///
    /// \brief EndDiagonal
    /// \return
    ///
    int64_t EndDiagonal() const;

    ///
    /// \brief EndPositionH
    /// \return
    ///
    uint64_t EndPositionH() const;

    ///
    /// \brief EndPositionV
    /// \return
    ///
    uint64_t EndPositionV() const;

    ///
    /// \brief LowerDiagonal
    /// \return
    ///
    int64_t LowerDiagonal() const;

    ///
    /// \brief UpperDiagonal
    /// \return
    ///
    int64_t UpperDiagonal() const;

    ///
    /// \brief Score
    /// \return
    ///
    int32_t Score() const;

    ///
    /// \brief Size
    /// \return
    ///
    uint64_t Size() const;

public:
    ///
    /// \brief BeginPositionH
    /// \param beginPosH
    /// \return
    ///
    Seed& BeginPositionH(const uint64_t beginPosH);

    ///
    /// \brief BeginPositionV
    /// \param beginPosV
    /// \return
    ///
    Seed& BeginPositionV(const uint64_t beginPosV);

    ///
    /// \brief EndPositionH
    /// \param endPosH
    /// \return
    ///
    Seed& EndPositionH(const uint64_t endPosH);

    ///
    /// \brief EndPositionV
    /// \param endPosV
    /// \return
    ///
    Seed& EndPositionV(const uint64_t endPosV);

    ///
    /// \brief LowerDiagonal
    /// \param lowerDiagonal
    /// \return
    ///
    Seed& LowerDiagonal(const int64_t lowerDiagonal);

    ///
    /// \brief UpperDiagonal
    /// \param upperDiagonal
    /// \return
    ///
    Seed& UpperDiagonal(const int64_t upperDiagonal);

    ///
    /// \brief Score
    /// \param score
    /// \return
    ///
    Seed& Score(const int32_t score);

public:
    ///
    /// \brief operator +=
    ///
    /// Combines \p other Seed into this one.
    ///
    /// \param other
    /// \return
    ///
    Seed& operator+=(const Seed& other);

    ///
    /// \brief operator +
    ///
    /// Creates a newly-merged Seed, by combining this seed with \p other
    ///
    /// \param other
    /// \return
    ///
    Seed operator+(const Seed& other) const;

private:
    uint64_t beginPositionH_;
    uint64_t beginPositionV_;
    uint64_t endPositionH_;
    uint64_t endPositionV_;
    int64_t lowerDiagonal_;
    int64_t upperDiagonal_;
    int32_t score_;
};

///
/// \brief Default less-than comparison.
///
/// Compares on Seed::BeginDiagonal.
///
/// \param lhs seed
/// \param rhs seed
/// \return true if \p lhs compares less-than \p rhs
///
bool operator<(const Seed& lhs, const Seed& rhs);

///
/// \brief Default equality comparison.
///
/// Compares on all member fields.
///
/// \param lhs seed
/// \param rhs seed
/// \return true if seeds are equal
///
bool operator==(const Seed& lhs, const Seed& rhs);

}  // namespace Align
}  // namespace PacBio

#include <pbcopper/align/internal/Seed-inl.h>

#endif  // PBCOPPER_ALIGN_SEED_H
