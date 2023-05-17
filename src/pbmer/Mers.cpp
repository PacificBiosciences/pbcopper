#include <pbcopper/pbmer/Mers.h>

#include <deque>
#include <limits>
#include <stdexcept>

#include <cassert>

namespace PacBio {
namespace Pbmer {

Mers::Mers(const uint8_t kSize) : kmerSize{kSize} {}

void Mers::AddKmer(const Kmer& k)
{
    if (k.strand == Data::Strand::FORWARD) {
        forward.emplace_back(k);
    } else {
        reverse.emplace_back(k);
    }
}

#if 0
//  DO NOT remove, this function generates correct minimizers and is easier to debug.
//  It will remain here in case another edge case arises, at which point we'll use
//  this function. Currently, the deque method is passing all the tests and is faster.

void Mers::AWWindowMin(unsigned int winSize)
{
    // Make sure that the kmers have been hashed
    if (!hashed_) HashKmers();

    /*
    if (winSize > minimizers.size()) {
        throw std::runtime_error{
            "[pbmer] minimizer ERROR: windowSize must be smaller than the number of kmers."};
    }
    */

    // bump level
    ++level_;

    std::vector<Kmer> newMinimizers;  // updated list of minimizers

    // Compare kmers by mer, breaking ties by position (in reverse order)
    struct KmerLessThan
    {
        bool operator()(const Kmer& lhs, const Kmer& rhs) const
        {
            if (lhs.mer < rhs.mer) {
                return true;
            } else if (lhs.mer > rhs.mer) {
                return false;
            } else {
                return lhs.pos > rhs.pos;
            }
        }
    };
    std::multiset<Kmer, KmerLessThan> window;  // kmers in current window
    std::vector<Kmer> windowMinimizers;        // minimizers in current window

    // Find the new set of minimizers
    for (uint32_t i = 0; i < winSize && i < minimizers.size();
         ++i) {  // add kmers from first window
        window.emplace(minimizers[i]);
    }
    // Add all minimizers (mers equal to the smallest mer) from the first window, in position order.
    windowMinimizers.clear();
    for (auto it = window.cbegin(); it != window.cend() && it->mer == window.cbegin()->mer; it++) {
        windowMinimizers.emplace_back(*it);  // added in reverse order by position
    }
    for (auto it = windowMinimizers.crbegin(); it != windowMinimizers.crend(); it++) {
        newMinimizers.emplace_back(*it);  // added in proper order by position
    }

    for (uint32_t i = winSize; i < minimizers.size();
         ++i) {  // minimizer for window ending at kmer `i`, inclusive
        // Advance window to include kmer i
        window.erase(minimizers[i - winSize]);
        window.emplace(minimizers[i]);

        // Add the minimizer(s) from this window if different than minimizer from the previous window.
        windowMinimizers.clear();
        for (auto it = window.cbegin();
             *it != *newMinimizers.crbegin() && it->mer == window.cbegin()->mer; it++) {
            windowMinimizers.emplace_back(*it);  // added in reverse order by position
        }
        for (auto it = windowMinimizers.crbegin(); it != windowMinimizers.crend(); it++) {
            newMinimizers.emplace_back(*it);  // added in proper order by position
        }
    }

    minimizers = std::move(newMinimizers);
}

#endif

std::vector<Kmer> Mers::BuildNMPs() const
{

    if (kmerSize > 16) {
        throw std::runtime_error{"[pbmer] Mers ERROR: Kmer must be <= 16 bp."};
    }

    // no mask because we are merging to minimizers up to 32 bits.
    const uint64_t mask = ~uint64_t(0);

    std::vector<Kmer> nmps;

    uint64_t minA;
    uint64_t minB;

    for (std::size_t i = 1; i < minimizers.size(); ++i) {
        Kmer nm{minimizers[i].mer, 0, Data::Strand::FORWARD};
        minA = minimizers[i - 1].mer;
        minB = nm.mer;
        if (minA <= minB) {
            nm.mer = (minA << 32) | minB;
        } else {
            nm.mer = (minB << 32) | minA;
        }
        nm.mer = Mix64Masked(nm.mer, mask);

        nmps.push_back(nm);
    }

    return nmps;
}

void Mers::HashKmers()
{
    hashed_ = true;
    level_ = 0;
    const uint64_t mask = (1ull << 2 * kmerSize) - 1;

    for (auto& f : forward) {
        f.mer = Mix64Masked(f.mer, mask);
    }

    for (auto& r : reverse) {
        r.mer = Mix64Masked(r.mer, mask);
    }

    // clear minimizers in case a user wants to recompute
    minimizers.clear();

    // Minimizing by position meaning we to traverse the reverse sequence backwards.
    auto itf = forward.cbegin();
    auto itr = reverse.cbegin();

    for (; itf != forward.cend();) {
        if (itf->mer == itr->mer) {
            ++itf;
            ++itr;
            continue;
        }
        if (itf->mer < itr->mer) {
            minimizers.emplace_back(*itf);
        } else {
            minimizers.emplace_back(*itr);
        }
        ++itf;
        ++itr;
    }
}

uint64_t Mers::Mix64Masked(uint64_t key, const uint64_t mask)
{
    key = (~key + (key << 21)) & mask;  // key = (key << 21) - key - 1;
    key = key ^ (key >> 24);
    key = ((key + (key << 3)) + (key << 8)) & mask;  // key * 265
    key = key ^ (key >> 14);
    key = ((key + (key << 2)) + (key << 4)) & mask;  // key * 21
    key = key ^ (key >> 28);
    key = (key + (key << 31)) & mask;
    return key;
}

void Mers::WindowMin(unsigned int winSize)
{
    // Make sure that the kmers have been hashed
    if (!hashed_) {
        HashKmers();
    }

    /* For minimizer lists shorter than the window size, return the global minima
    if (winSize > minimizers.size()) {
        throw std::runtime_error{
            "[pbmer] minimizer ERROR: windowSize must be smaller than the number of kmers."};
    }
    */

    // bump level
    ++level_;

    // the return vector
    std::vector<Kmer> newMins;

    // a deque for keeping track of minimum values
    // https://www.geeksforgeeks.org/sliding-window-maximum-maximum-of-all-subarrays-of-size-k/
    std::deque<int> window;

    // next two for loops intentionally use i without reseting it
    unsigned int i = 0;

    // Load first window into deque.
    for (; i < winSize && i < minimizers.size(); i++) {
        // if the i^th element is less than the back of the deque, pop the back, it's a new low.
        // ties are not resolved, meaning the first lowest value is the starting point
        while ((!window.empty()) && minimizers.at(i).mer < minimizers.at(window.back()).mer) {
            window.pop_back();  // Remove from rear
        }
        window.push_back(i);
    }

    // Keeps track of ties so that there are not repeats elements in newMins
    int lastOutputIx = -1;
    int lowerWinBound = 0;
    // Process rest of the elements [winSize, minimizers.size()).
    for (; i < minimizers.size(); ++i) {
        // The element at the front of the queue is the smallest element of
        // previous window, so add it if it's greater than last.

        if (window.front() > lastOutputIx) {
            newMins.emplace_back(minimizers.at(window.front()));
            lastOutputIx = window.front();
        }
        lowerWinBound = i - winSize;
        // Remove the elements which are out of this window
        while ((!window.empty()) && window.front() <= lowerWinBound) {
            // Duplicate minimizer, might need to add
            if (newMins.back().mer == minimizers.at(window.front()).mer) {
                if (window.front() > lastOutputIx) {
                    newMins.emplace_back(minimizers.at(window.front()));
                    lastOutputIx = window.front();
                }
            }
            window.pop_front();  // Remove from front of queue
        }
        // Remove all elements in the deque greater than the currently
        // being added element (remove useless elements)
        while ((!window.empty()) && minimizers.at(i).mer < minimizers.at(window.back()).mer) {
            // possible tie
            if (newMins.back().mer == minimizers.at(window.back()).mer) {
                // `lastOutputIx` keeps track of the last index, and prevents duplicate minimizers
                if (window.back() > lastOutputIx) {
                    newMins.emplace_back(minimizers.at(window.back()));
                    lastOutputIx = window.back();
                }
            }
            // actual removal of the elements in the deque
            window.pop_back();
        }

        // Add current element at the rear of `window`
        window.push_back(i);
    }

    // Add minimizers from last window
    for (unsigned int l = 0; l < window.size(); ++l) {
        if (minimizers.at(window.at(l)).mer == minimizers.at(window.front()).mer) {
            if (window.at(l) > lastOutputIx) {
                newMins.emplace_back(minimizers.at(window.at(l)));
                lastOutputIx = window.at(l);
            }
        }
    }

    // loading up minimizers to return;
    minimizers = std::move(newMins);
}

}  // namespace Pbmer
}  // namespace PacBio
