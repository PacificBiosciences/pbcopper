#ifndef PBCOPPER_ALIGN_SEED_H
#define PBCOPPER_ALIGN_SEED_H

#include <pbcopper/PbcopperConfig.h>

#include <cstdint>

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
    Seed() = default;

    ///
    /// \brief Seed
    /// \param beginPosH
    /// \param beginPosV
    /// \param seedLength
    ///
    Seed(std::uint64_t beginPosH, std::uint64_t beginPosV, std::uint64_t seedLength);

    ///
    /// \brief Seed
    /// \param beginPosH
    /// \param beginPosV
    /// \param endPosH
    /// \param endPosV
    ///
    Seed(std::uint64_t beginPosH, std::uint64_t beginPosV, std::uint64_t endPosH,
         std::uint64_t endPosV);

public:
    ///
    /// \brief BeginDiagonal
    /// \return
    ///
    std::int64_t BeginDiagonal() const;

    ///
    /// \brief BeginPositionH
    /// \return
    ///
    std::uint64_t BeginPositionH() const;

    ///
    /// \brief BeginPositionV
    /// \return
    ///
    std::uint64_t BeginPositionV() const;

    ///
    /// \brief Diagonal
    ///
    /// Calculate number of bases between a seed and the diagonal
    /// axis of the matrix it's in as extended outward from the
    /// upper-left-hand corner of the matrix toward the lower right.
    /// High numbers are closer to the upper-right, negative numbers
    /// closer to the lower-left.
    ///
    /// \return std::int64_t The distance from the seed's upper-left corner and
    ///                 the diagonal axis of the matrix.
    ///
    std::int64_t Diagonal() const;

    ///
    /// \brief EndDiagonal
    /// \return
    ///
    std::int64_t EndDiagonal() const;

    ///
    /// \brief EndPositionH
    /// \return
    ///
    std::uint64_t EndPositionH() const;

    ///
    /// \brief EndPositionV
    /// \return
    ///
    std::uint64_t EndPositionV() const;

    ///
    /// \brief LowerDiagonal
    /// \return
    ///
    std::int64_t LowerDiagonal() const;

    ///
    /// \brief UpperDiagonal
    /// \return
    ///
    std::int64_t UpperDiagonal() const;

    ///
    /// \brief Score
    /// \return
    ///
    std::int32_t Score() const;

    ///
    /// \brief Size
    /// \return
    ///
    std::uint64_t Size() const;

public:
    ///
    /// \brief BeginPositionH
    /// \param beginPosH
    /// \return
    ///
    Seed& BeginPositionH(std::uint64_t beginPosH);

    ///
    /// \brief BeginPositionV
    /// \param beginPosV
    /// \return
    ///
    Seed& BeginPositionV(std::uint64_t beginPosV);

    ///
    /// \brief EndPositionH
    /// \param endPosH
    /// \return
    ///
    Seed& EndPositionH(std::uint64_t endPosH);

    ///
    /// \brief EndPositionV
    /// \param endPosV
    /// \return
    ///
    Seed& EndPositionV(std::uint64_t endPosV);

    ///
    /// \brief LowerDiagonal
    /// \param lowerDiagonal
    /// \return
    ///
    Seed& LowerDiagonal(std::int64_t lowerDiagonal);

    ///
    /// \brief UpperDiagonal
    /// \param upperDiagonal
    /// \return
    ///
    Seed& UpperDiagonal(std::int64_t upperDiagonal);

    ///
    /// \brief Score
    /// \param score
    /// \return
    ///
    Seed& Score(std::int32_t score);

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
    std::uint64_t beginPositionH_ = 0;
    std::uint64_t beginPositionV_ = 0;
    std::uint64_t endPositionH_ = 0;
    std::uint64_t endPositionV_ = 0;
    std::int64_t lowerDiagonal_ = 0;
    std::int64_t upperDiagonal_ = 0;
    std::int32_t score_ = 0;
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
bool operator<(const Seed& lhs, const Seed& rhs) noexcept;

///
/// \brief Default equality comparison.
///
/// Compares on all member fields.
///
/// \param lhs seed
/// \param rhs seed
/// \return true if seeds are equal
///
bool operator==(const Seed& lhs, const Seed& rhs) noexcept;

}  // namespace Align
}  // namespace PacBio

#endif  // PBCOPPER_ALIGN_SEED_H
