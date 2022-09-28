#include <pbcopper/pbmer/Parser.h>

#include <algorithm>
#include <array>
#include <limits>
#include <stdexcept>
#include <vector>

#include <cassert>

namespace PacBio {
namespace Pbmer {

Parser::Parser(uint8_t kmerSize)
    : kmerSize_{kmerSize}
    , mask_{((0 < kmerSize) && (kmerSize <= 32))
                ? std::numeric_limits<decltype(mask_)>::max() >> (64 - 2 * kmerSize)
                : throw std::invalid_argument{"[pbmer] parsing ERROR: kmerSize must be in the "
                                              "range [1, 32]."}}
    , shift1_{2ull * (kmerSize - 1)}
{}

Mers Parser::Parse(const std::string& dna) const
{
    if (dna.size() < kmerSize_) {
        throw std::runtime_error{"[pbmer] parsing ERROR: DNA sequence shorter than kmer size."};
    }

    Mers kms{kmerSize_};
    Kmer forwardKmer{Data::Strand::FORWARD};
    Kmer reverseKmer{Data::Strand::REVERSE};

    uint8_t c;
    // The length of the current kmer buffer, lk needs to be >= to kmer_size
    // before a kmer is complete. Remember the kmer is build in a sliding fashion
    int lk = 0;

    for (const auto d : dna) {
        // check that only ASCII is used
        assert(static_cast<unsigned char>(d) < 128);
        c = ASCII_TO_DNA[static_cast<unsigned char>(d)];
        // If an unknown character is seen (c < 4) advance the window, set lk = 0,
        // and reset the kmer.
        if (c < 4) {
            // building forwardKmer and reverseKmer kmers by adding shiftin onto the kmers.
            forwardKmer.mer = (forwardKmer.mer << 2 | c) & mask_;
            reverseKmer.mer = (reverseKmer.mer >> 2) | (3ull ^ c) << shift1_;
            ++lk;
        } else {
            // unknown base, reset
            lk = 0;
            forwardKmer.mer = 0;
            reverseKmer.mer = 0;
            forwardKmer.pos += kmerSize_;
            reverseKmer.pos += kmerSize_;
        }
        if (lk >= kmerSize_) {
            kms.AddKmer(forwardKmer);
            kms.AddKmer(reverseKmer);
            ++forwardKmer.pos;
            ++reverseKmer.pos;
        }
    }

    return kms;  // placeholder return
}

std::vector<DnaBit> Parser::ParseDnaBit(const std::string& dna) const
{
    if (dna.size() < kmerSize_) {
        throw std::runtime_error{"[pbmer] parsing ERROR: DNA sequence shorter than kmer size."};
    }

    std::vector<DnaBit> kms;
    kms.reserve(dna.size() - kmerSize_ + 1);
    DnaBit forwardKmer;
    forwardKmer.strand = 0;
    forwardKmer.msize = kmerSize_;

    uint8_t c;
    // The length of the current kmer buffer, lk needs to be >= to kmer_size
    // before a kmer is complete. Remember the kmer is build in a sliding fashion
    int lk = 0;

    for (const auto d : dna) {
        // check that only ASCII is used
        assert(static_cast<unsigned char>(d) < 128);
        c = ASCII_TO_DNA[static_cast<unsigned char>(d)];
        // If an unknown chracter is seen (c < 4) advance the window, set lk = 0,
        // and reset the kmer.
        if (c < 4) {
            // building forwardKmer kmer
            forwardKmer.mer = (forwardKmer.mer << 2 | c) & mask_;
            ++lk;
        } else {
            // unknown base, reset
            lk = 0;
            forwardKmer.mer = 0;
        }
        if (lk >= kmerSize_) {
            kms.emplace_back(forwardKmer);
        }
    }

    return kms;
}

void Parser::ParseDnaBit(const std::string& dna, std::vector<DnaBit>& kms) const
{
    if (dna.size() < kmerSize_) {
        throw std::runtime_error{"[pbmer] parsing ERROR: DNA sequence shorter than kmer size."};
    }

    DnaBit forwardKmer;
    forwardKmer.strand = 0;
    forwardKmer.msize = kmerSize_;

    uint8_t c;
    // The length of the current kmer buffer, lk needs to be >= to kmer_size
    // before a kmer is complete. Remember the kmer is build in a sliding fashion
    int lk = 0;

    for (const auto d : dna) {
        // check that only ASCII is used
        assert(static_cast<unsigned char>(d) < 128);
        c = ASCII_TO_DNA[static_cast<unsigned char>(d)];
        // If an unknown chracter is seen (c < 4) advance the window, set lk = 0,
        // and reset the kmer.
        if (c < 4) {
            // building forwardKmer kmer
            forwardKmer.mer = (forwardKmer.mer << 2 | c) & mask_;
            ++lk;
        } else {
            // unknown base, reset
            lk = 0;
            forwardKmer.mer = 0;
        }
        if (lk >= kmerSize_) {
            kms.emplace_back(forwardKmer);
        }
    }
}

std::string Parser::RLE(const std::string& dna) const
{
    std::string res;
    res.reserve(dna.size());
    if (dna.size() == 0) {
        return res;
    }
    res.push_back(dna[0]);

    for (const auto& c : dna) {
        if (res.back() != c) {
            res.push_back(c);
        }
    }
    return res;
}

void Parser::RLE(std::string& dna) const
{
    auto first = dna.begin();
    auto last = dna.end();
    if (first == last) {
        return;
    }
    auto result = first;
    while (++first != last) {
        if (!(*result == *first) && ++result != first) {
            *result = std::move(*first);
        }
    }
    dna.erase(++result, dna.end());
}

}  // namespace Pbmer
}  // namespace PacBio
