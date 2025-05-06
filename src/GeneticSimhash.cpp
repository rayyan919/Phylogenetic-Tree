#include "../include/GeneticSimhash.h"
#include <array>
#include <cmath>
#include <vector>
#include <tuple>

// --------------------------------------------------------------------
// INTERNAL: polynomial hash of one segment taken from a BitEncodedSeq
// hash =  ((((b1*5 + b2)*5 + b3)*5 … ) * MIX)  (exactly as before)
// --------------------------------------------------------------------
std::uint64_t GeneticSimhash::custom_nucleotide_hash(const BitEncodedSeq &seq,
                                                     std::uint32_t start, std::uint32_t length)
{
    std::uint64_t hash = 0;

    for (std::uint32_t i = 0; i < length; ++i)
    {
        char nucleotide = seq.at(start + i); // O(1) packed read
        std::uint64_t val = 0;               // A->1, C->2, T->3, G->4
        switch (nucleotide)
        {
        case 'A':
            val = 1;
            break;
        case 'C':
            val = 2;
            break;
        case 'T':
            val = 3;
            break;
        case 'G':
            val = 4;
            break;
        default:
            val = 0; // shouldn’t happen
        }
        hash = hash * 5 + val;
    }
    // Bit-mix to spread to full 64-bit range
    return hash * 0x9e377f5ULL;
}

// --------------------------------------------------------------------
// PUBLIC: Simhash over an encoded sequence
// Splits the DNA into ≤ numSegments equal-length chunks,
// hashes each chunk with the function above, then feeds the
// list of 64-bit hashes to Simhash::compute().
// --------------------------------------------------------------------
Simhash::hash_t GeneticSimhash::computeSimhash(const BitEncodedSeq &seq, std::size_t numSegments)
{
    if (seq.empty())
        throw std::invalid_argument("Genetic sequence is empty.");

    const std::uint32_t n = seq.size();

    // never create more segments than bases
    numSegments = std::min<std::size_t>(numSegments, n);

    std::uint32_t segLen = n / numSegments;
    if (segLen == 0)
    {
        segLen = 1;
        numSegments = n;
    } // very short seq

    std::vector<Simhash::hash_t> segHashes;
    segHashes.reserve(numSegments);

    std::uint32_t cursor = 0;
    for (std::size_t seg = 0; seg < numSegments; ++seg)
    {
        // last segment takes the remainder
        std::uint32_t len =
            (seg == numSegments - 1) ? (n - cursor) : segLen;

        segHashes.push_back(custom_nucleotide_hash(seq, cursor, len));
        cursor += len;
    }
    return Simhash::compute(segHashes);
}

std::tuple<std::uint32_t, std::uint32_t, std::uint32_t> GeneticSimhash::makeCoords(Simhash::hash_t simhash)
{
    std::tuple<std::uint32_t, std::uint32_t, std::uint32_t> coords;
    if (simhash >> 32)
    {
        uint32_t raw_x = static_cast<uint32_t>((simhash >> 43) & ((1ULL << 21) - 1));
        uint32_t raw_y = static_cast<uint32_t>((simhash >> 22) & ((1ULL << 21) - 1));
        uint32_t raw_z = static_cast<uint32_t>(simhash & ((1ULL << 22) - 1));

        // Scale down to [0,4095]:
        std::get<0>(coords) = raw_x / 512;
        std::get<1>(coords) = raw_y / 512;
        std::get<2>(coords) = raw_z / 1024;
    }
    else
    {
        // If simhash is only 32-bit.
        uint32_t raw_x = static_cast<uint32_t>((simhash >> 22) & ((1U << 10) - 1));
        uint32_t raw_y = static_cast<uint32_t>((simhash >> 11) & ((1U << 11) - 1));
        uint32_t raw_z = static_cast<uint32_t>(simhash & ((1U << 11) - 1));
        std::get<0>(coords) = raw_x;
        std::get<1>(coords) = raw_y;
        std::get<2>(coords) = raw_z;
    }
    return coords;
}
