// Authors: Chris Dunn, Zev Kronenberg, Derek Barnett

#ifndef PBCOPPER_PBMER_SPARSE_HAPLOTYPE_H
#define PBCOPPER_PBMER_SPARSE_HAPLOTYPE_H

#include <pbcopper/PbcopperConfig.h>

#include <cstdint>

#include <string>
#include <vector>

#include <boost/dynamic_bitset.hpp>

#include <pbcopper/pbmer/Bubble.h>

namespace PacBio {
namespace Pbmer {

class SparseHaplotype
{

public:
    /*!
       \brief Constructor of SparseHaplotype
       \param name seq/read name
       \param id seq/read id
       \param nvars the number of variant sites
    */
    SparseHaplotype(const std::string& name, uint32_t id, int32_t nvars);

public:
    /*!
       \brief Haplotypes are a rotation of variants. This function converts a vector of variant sites to haplotypes.
       \param Bubbles from KFG.
    */
    void LoadVariants(const Bubbles& bubs);

public:
    /*!
       \brief Get cluster ID
       \return cluster id
    */
    int32_t ClusterId() const;

    /*!
       \brief Convert the binary sequence of variant sites to a string e.g.: 0101010
       \return string of binary characters.
    */
    std::string HapToStr() const;

    /*!
       \brief Get number of variants
       \return number of variants
    */
    int32_t NVariants() const;

private:
    std::string name_;
    int32_t cluster_{0};
    uint32_t haplotypeId_;
    int32_t nVariants_;
    std::vector<double> fractionalMembership_;
    boost::dynamic_bitset<> variants_;
};

}  // namespace Pbmer
}  // namespace PacBio

#endif  // PBCOPPER_PBMER_SPARSE_HAPLOTYPE_H
