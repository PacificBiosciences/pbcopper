#ifndef PBCOPPER_PBMER_MERS_H
#define PBCOPPER_PBMER_MERS_H

#include <pbcopper/PbcopperConfig.h>

#include <cstdint>

#include <vector>

#include <pbcopper/pbmer/Kmer.h>

namespace PacBio {
namespace Pbmer {

class Mers
{
public:
    ///
    /// A special hash function `cuz u64 kmers requires a mix_64_masked
    /// BADBOY: https://gist.github.com/badboy/6267743#64-bit-mix-functions
    ///
    static uint64_t Mix64Masked(uint64_t key, const uint64_t mask);

    explicit Mers(uint8_t /*kmerSize*/ kSize);

    ///
    /// Insert a new kmer into the mers lists
    ///
    void AddKmer(const Kmer& kmer);

    ///
    /// Applies the mix_64_masked hash to forward and reverse kmers in place,
    /// and loads the minimizers vector
    ///
    void HashKmers();

    /// Minimizes the hashed kmers. The canonical minimizers are generated later
    /// by calling WindowMin. Can be called an arbitrary number of times.
    ///
    void WindowMin(unsigned int winSize);

    ///
    /// Builds neighboring minimizer pairs
    ///
    std::vector<Kmer> BuildNMPs() const;

    uint8_t kmerSize;  // width of the kmers - must be smaller than 32 DNA bases
    std::vector<Kmer> forward;
    std::vector<Kmer> reverse;
    std::vector<Kmer> minimizers;

private:
    bool hashed_ = false;  // have the kmers been hashed to increase bit diversity
    int level_ = -1;       // how many levels of minimizers have been calculated
};

}  // namespace Pbmer
}  // namespace PacBio

#endif  // PBCOPPER_PBMER_MERS_H
