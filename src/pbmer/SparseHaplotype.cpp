// Authors: Chris Dunn, Zev Kronenberg, Derek Barnett

#include <pbcopper/pbmer/SparseHaplotype.h>

#include <sstream>

namespace PacBio {
namespace Pbmer {

SparseHaplotype::SparseHaplotype(const std::string& name, uint32_t id, int32_t nvars)
    : name_{name}, haplotypeId_{id}, nVariants_{nvars}
{
    variants_.resize(nvars);
}

void SparseHaplotype::LoadVariants(const Bubbles& bubs)
{
    uint32_t bubIdx = 0;
    for (const auto& b : bubs) {
        for (const auto& l : b.LData) {
            auto id = std::get<0>(l);
            if (id == haplotypeId_) {
                variants_[bubIdx] = 1;
            }
        }
        ++bubIdx;
    }
}

std::string SparseHaplotype::HapToStr() const
{
    std::ostringstream result;
    for (boost::dynamic_bitset<>::size_type i = 0; i < variants_.size(); ++i) {
        result << variants_[i];
    }
    return result.str();
}

/*!
   \brief Get cluster ID
   \return cluster id
*/
int32_t SparseHaplotype::ClusterId() const { return cluster_; }

/*!
   \brief Get number of variants
   \return number of variants
*/
int32_t SparseHaplotype::NVariants() const { return nVariants_; }

}  // namespace Pbmer
}  // namespace PacBio
