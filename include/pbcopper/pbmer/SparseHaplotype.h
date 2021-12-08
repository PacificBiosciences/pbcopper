#ifndef PBCOPPER_PBMER_SPARSE_HAPLOTYPE_H
#define PBCOPPER_PBMER_SPARSE_HAPLOTYPE_H

#include <pbcopper/PbcopperConfig.h>

#include <pbcopper/pbmer/Bubble.h>

#include <boost/dynamic_bitset.hpp>

#include <string>
#include <vector>

#include <cstdint>

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
    SparseHaplotype(std::string name, uint32_t id, int32_t nvars);

    /*!
       \brief Constructor of SparseHaplotype
       \param name seq/read name
       \param id seq/read id
       \param nvars the number of variant sites
       \param seq a binary string "010101..."
    */
    SparseHaplotype(std::string name, uint32_t id, const std::string& seq);

public:
    /*!
       \brief Haplotypes are a rotation of variants. This function converts a vector of variant sites to haplotypes.
       \param Bubbles from KFG.
    */
    void LoadVariants(const Bubbles& bubs);

    void InitializeMembership(int32_t nClusters);

    void InitializeMembership(int32_t nClusters, int prior);

    void SetMembershipProb(int index, double val);

    void SetCluster(int32_t index);

public:
    /*!
       \brief Get cluster ID
       \return cluster id
    */
    int32_t ClusterId() const;

    /*!
       \brief provides a printable string for the haplotype information
    */
    std::string ToStr() const;

    /*!
       \brief Convert the binary sequence of variant sites to a string e.g.: 0101010
    */
    std::string HapStr() const;

    const std::string& Name() const;

    bool HasVar(int index) const;

    /*!
       \brief Get number of variants
       \return number of variants
    */
    int32_t NVariants() const;

    /*!
       \brief Get the membership fraction, the probability of haplotype being part of a cluster.
       \param index the cluster to retrieve
    */
    double MembershipFraction(int index) const;

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
