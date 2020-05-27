// Authors: Chris Dunn, Zev Kronenberg, Derek Barnett

#include <pbcopper/pbmer/SparseHaplotype.h>

#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>

namespace PacBio {
namespace Pbmer {

SparseHaplotype::SparseHaplotype(std::string name, uint32_t id, int32_t nvars)
    : name_{std::move(name)}, haplotypeId_{id}, nVariants_{nvars}
{
    variants_.resize(nvars);
}

SparseHaplotype::SparseHaplotype(std::string name, uint32_t id, const std::string& seq)
    : name_{std::move(name)}, haplotypeId_{id}, nVariants_{static_cast<int32_t>(seq.size())}
{
    variants_.resize(seq.size());

    fractionalMembership_.push_back(0.0);
    cluster_ = 0;

    int idx = 0;
    for (const auto i : seq) {
        if (i != '1' && i != '0') {
            throw std::runtime_error("Invalid binary sequence. String must only contain [0,1]");
        }

        if (i == '1') {
            variants_[idx] = 1;
        }
        ++idx;
    }
}

void SparseHaplotype::InitializeMembership(int32_t nClusters)
{
    cluster_ = 0;
    fractionalMembership_.resize(0);
    fractionalMembership_.clear();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.001, 0.999);

    // stick breaking model to generate random variables that sum to (or nearly) one.
    double stick = 1.0;
    for (auto i = 0; i < nClusters; ++i) {

        auto temp = stick * dis(gen);
        fractionalMembership_.push_back(temp);
        stick = temp;
    }
}

void SparseHaplotype::InitializeMembership(int32_t nClusters, int prior)
{
    cluster_ = 0;
    fractionalMembership_.resize(0);
    fractionalMembership_.clear();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    // stick breaking model to generate random variables that sum to (or nearly) one.
    double stick = 0.5;
    for (auto i = 0; i < nClusters; ++i) {
        if (i != prior) {
            auto temp = stick * dis(gen);
            fractionalMembership_.push_back(temp);
            stick = temp;
        } else {
            fractionalMembership_.push_back(0.5);
        }
    }
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

std::string SparseHaplotype::HapStr() const
{
    std::ostringstream result;
    for (boost::dynamic_bitset<>::size_type i = 0; i < variants_.size(); ++i) {
        result << variants_[i];
    }
    return result.str();
}

std::string SparseHaplotype::ToStr() const
{
    std::ostringstream result;

    result << name_ << " " << haplotypeId_ << " ";

    for (boost::dynamic_bitset<>::size_type i = 0; i < variants_.size(); ++i) {
        result << variants_[i];
    }

    if (!fractionalMembership_.empty()) {
        result << " nclusters:" << fractionalMembership_.size() << " ";

        for (auto m : fractionalMembership_) {
            result << m << " ";
        }

        result << "clusterAssignment: " << cluster_ << " ";
        result << "clusterProb:" << fractionalMembership_[cluster_];
    }

    return result.str();
}

int32_t SparseHaplotype::ClusterId() const { return cluster_; }

const std::string& SparseHaplotype::Name() const { return name_; }

int32_t SparseHaplotype::NVariants() const { return nVariants_; }

bool SparseHaplotype::HasVar(int index) const
{
    assert(index < static_cast<int>(variants_.size()) && index >= 0);
    return (variants_[index] == 1);
}

double SparseHaplotype::MembershipFraction(int index) const { return fractionalMembership_[index]; }

void SparseHaplotype::SetMembershipProb(int index, double val)
{
    assert(index < static_cast<int>(fractionalMembership_.size()) && index >= 0);
    fractionalMembership_[index] = val;
}

void SparseHaplotype::SetCluster(int32_t index) { cluster_ = index; }

}  // namespace Pbmer
}  // namespace PacBio
