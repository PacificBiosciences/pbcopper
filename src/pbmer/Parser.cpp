// Authors: Chris Dunn, Zev Kronenberg, Derek Barnett
#include <pbcopper/pbmer/Parser.h>

#include <cassert>

#include <array>
#include <stdexcept>
#include <vector>

namespace PacBio {
namespace Pbmer {

Parser::Parser(uint8_t kmerSize)
    : kmerSize_{kmerSize}, mask_{(1ull << 2 * kmerSize) - 1}, shift1_{2ull * (kmerSize - 1)}
{
}

Mers Parser::Parse(const std::string& dna) const
{
    if (dna.size() < kmerSize_)
        throw std::runtime_error{"[pbmer] parsing ERROR: DNA sequence shorter than kmer size."};

    Mers kms{kmerSize_};
    Kmer forward{Data::Strand::FORWARD};
    Kmer reverse{Data::Strand::REVERSE};

    uint8_t c;
    // Counters to keep track of position and sizes of things
    int i = 0;
    // The length of the current kmer buffer, lk needs to be >= to kmer_size
    // before a kmer is complete. Remember the kmer is build in a sliding fashion
    int lk = 0;
    // The count of the number of kmers/2. Unknown bases are skipped so the
    // dna.size() != size( kmers )
    int n = 0;

    for (const auto d : dna) {
        // check that only ASCII is used
        assert(static_cast<unsigned char>(d) < 128);
        c = AsciiToDna[static_cast<int>(d)];
        // If an unknown character is seen (c < 4) advance the window, set lk = 0,
        // and reset the kmer.
        if (c < 4) {
            // building forward and reverse kmers by adding shiftin onto the kmers.
            forward.mer = (forward.mer << 2 | c) & mask_;
            reverse.mer = (reverse.mer >> 2) | (3ull ^ c) << shift1_;
            ++lk;
        } else {
            // unknown base, reset
            lk = 0;
            i += kmerSize_;
            forward.mer = 0;
            reverse.mer = 0;
            forward.pos += kmerSize_;
            reverse.pos += kmerSize_;
        }
        if (lk >= kmerSize_) {
            n += 2;
            kms.AddKmer(forward);
            kms.AddKmer(reverse);
            ++forward.pos;
            ++reverse.pos;
        }
        ++i;
    }

    return kms;  // placeholder return
}

std::vector<DnaBit> Parser::ParseDnaBit(const std::string& dna) const
{
    if (dna.size() < kmerSize_)
        throw std::runtime_error{"[pbmer] parsing ERROR: DNA sequence shorter than kmer size."};

    std::vector<DnaBit> kms;
    kms.reserve(dna.size() - kmerSize_ + 1);
    DnaBit forward;
    forward.strand = 0;
    forward.msize = kmerSize_;

    uint8_t c;
    // Counters to keep track of position and sizes of things
    int i = 0;
    // The length of the current kmer buffer, lk needs to be >= to kmer_size
    // before a kmer is complete. Remember the kmer is build in a sliding fashion
    int lk = 0;
    // The count of the number of kmers/2. Unknown bases are skipped so the
    // dna.size() != size( kmers )
    int n = 0;

    for (const auto d : dna) {
        // check that only ASCII is used
        assert(static_cast<unsigned char>(d) < 128);
        c = AsciiToDna[static_cast<int>(d)];
        // If an unknown chracter is seen (c < 4) advance the window, set lk = 0,
        // and reset the kmer.
        if (c < 4) {
            // building forward kmer
            forward.mer = (forward.mer << 2 | c) & mask_;
            ++lk;
        } else {
            // unknown base, reset
            lk = 0;
            i += kmerSize_;
            forward.mer = 0;
        }
        if (lk >= kmerSize_) {
            n += 2;
            kms.emplace_back(forward);
        }
        ++i;
    }

    return kms;
}

void Parser::ParseDnaBit(const std::string& dna, std::vector<DnaBit>& kms) const
{
    if (dna.size() < kmerSize_)
        throw std::runtime_error{"[pbmer] parsing ERROR: DNA sequence shorter than kmer size."};

    DnaBit forward;
    forward.strand = 0;
    forward.msize = kmerSize_;

    uint8_t c;
    // Counters to keep track of position and sizes of things
    int i = 0;
    // The length of the current kmer buffer, lk needs to be >= to kmer_size
    // before a kmer is complete. Remember the kmer is build in a sliding fashion
    int lk = 0;
    // The count of the number of kmers/2. Unknown bases are skipped so the
    // dna.size() != size( kmers )
    int n = 0;

    for (const auto d : dna) {
        // check that only ASCII is used
        assert(static_cast<unsigned char>(d) < 128);
        c = AsciiToDna[static_cast<int>(d)];
        // If an unknown chracter is seen (c < 4) advance the window, set lk = 0,
        // and reset the kmer.
        if (c < 4) {
            // building forward kmer
            forward.mer = (forward.mer << 2 | c) & mask_;
            ++lk;
        } else {
            // unknown base, reset
            lk = 0;
            i += kmerSize_;
            forward.mer = 0;
        }
        if (lk >= kmerSize_) {
            n += 2;
            kms.emplace_back(forward);
        }
        ++i;
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
    if (first == last) return;
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
