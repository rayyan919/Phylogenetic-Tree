
// #ifndef GENETIC_SIMHASH_H
// #define GENETIC_SIMHASH_H

// #include "simhash.h" // The simhash library provided
// #include <string>
// #include <vector>
// #include <stdexcept>
// #include <cstdint>

// namespace GeneticSimhash
// {
//     /**
//      * An improved nucleotide hash function that better preserves similarity.
//      *
//      * We use a larger base (5) to ensure better distribution while still
//      * preserving the similarity between sequences with minor mutations.
//      *
//      * Each nucleotide is assigned a value:
//      *   A -> 1
//      *   T -> 2
//      *   C -> 3
//      *   G -> 4
//      *
//      * The hash is computed with better bit distribution while maintaining
//      * the property that similar sequences have similar hashes.
//      */
//     inline uint64_t custom_nucleotide_hash(const std::string &segment)
//     {
//         uint64_t hash = 0;
//         constexpr uint64_t base = 5; // Better distribution than 3 but still preserves similarity

//         for (char nucleotide : segment)
//         {
//             uint64_t val = 0;
//             switch (nucleotide)
//             {
//             case 'A':
//                 val = 1;
//                 break;
//             case 'T':
//                 val = 2;
//                 break;
//             case 'C':
//                 val = 3;
//                 break;
//             case 'G':
//                 val = 4;
//                 break;
//             default:
//                 // For unknown characters we add 0.
//                 break;
//             }
//             hash = hash * base + val;
//         }

//         // Mix the bits to improve distribution without destroying locality
//         hash ^= hash >> 23;
//         hash ^= hash << 17;
//         hash ^= hash >> 7;

//         return hash;
//     }

//     /**
//      * Compute the simhash for a genetic sequence.
//      *
//      * The sequence is divided into numSegments equal parts (default is 64 segments for better precision).
//      * Each segment is hashed using our custom nucleotide hash function (above) and then
//      * combined using Simhash::compute() to yield a 64-bit simhash.
//      *
//      * This simhash should change moderately if a few nucleotides change, preserving similarity.
//      *
//      * @param geneticSequence The input genetic sequence.
//      * @param numSegments     The number of segments to divide the sequence. Default is 64.
//      * @return                The computed 64-bit simhash.
//      *
//      * @throws std::invalid_argument if the sequence is empty.
//      */
//     inline Simhash::hash_t computeSimhash(const std::string &geneticSequence, size_t numSegments = 64)
//     {
//         if (geneticSequence.empty())
//         {
//             throw std::invalid_argument("Genetic sequence is empty.");
//         }

//         size_t seqLength = geneticSequence.size();
//         size_t segLength = seqLength / numSegments;

//         // Ensure we use at least one character per segment
//         if (segLength == 0)
//         {
//             segLength = 1;
//             numSegments = std::min(seqLength, static_cast<size_t>(64));
//         }

//         std::vector<Simhash::hash_t> segmentHashes;
//         segmentHashes.reserve(numSegments);

//         // Divide the sequence into overlapping segments to better capture context
//         for (size_t i = 0; i < numSegments; ++i)
//         {
//             size_t startPos = (i * seqLength) / numSegments;

//             // Use overlapping segments when possible
//             size_t overlapLength = std::min(segLength + (segLength / 4), seqLength - startPos);

//             std::string segment = geneticSequence.substr(startPos, overlapLength);
//             segmentHashes.push_back(custom_nucleotide_hash(segment));
//         }

//         return Simhash::compute(segmentHashes);
//     }

//     /**
//      * Deviate (mutate) an existing simhash based on a mutation ratio.
//      *
//      * Given an original simhash and a ratio computed as (mutated_bases / total_bases),
//      * this function deterministically flips a number of bits in the simhash proportional
//      * to the ratio. For a 64-bit simhash, it flips roughly round(ratio * 64) bits.
//      *
//      * The method uses the original simhash as a seed to generate pseudo-random bit positions.
//      *
//      * @param original The original 64-bit simhash.
//      * @param ratio    The mutation ratio (mutated_bases divided by total_bases).
//      * @return         The new, deviated simhash.
//      */
//     inline Simhash::hash_t deviateSimhash(Simhash::hash_t original, double ratio)
//     {
//         const size_t totalBits = 64; // For a 64-bit hash.
//         size_t numToFlip = static_cast<size_t>(ratio * totalBits + 0.5);
//         Simhash::hash_t newHash = original;
//         // Use a simple linear congruential generator (LCG) seeded by the original hash.
//         uint64_t seed = original;
//         for (size_t i = 0; i < numToFlip; i++)
//         {
//             seed = seed * 6364136223846793005ULL + 1;
//             size_t bitIndex = seed % totalBits;
//             newHash ^= (1ULL << bitIndex);
//         }
//         return newHash;
//     }
// }

// #endif // GENETIC_SIMHASH_H

#ifndef GENETIC_SIMHASH_H
#define GENETIC_SIMHASH_H

#include "simhash.h" // The provided simhash library header.
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <algorithm> // for std::min

namespace GeneticSimhash
{

    inline uint64_t custom_nucleotide_hash(const std::string &segment)
    {
        uint64_t hash = 0;
        for (char nucleotide : segment)
        {
            uint64_t val = 0;
            switch (nucleotide)
            {
            case 'A':
                val = 1;
                break;
            case 'T':
                val = 2;
                break;
            case 'C':
                val = 3;
                break;
            case 'G':
                val = 4;
                break;
            default:
                val = 0;
                break;
            }
            hash = hash * 5 + val;
        }
        // Multiply by a large mixing constant to spread the bits into the full 64-bit range.
        hash *= 0x9e397f5ULL;
        return hash;
    }

    inline Simhash::hash_t computeSimhash(const std::string &geneticSequence, size_t numSegments = 32)
    {
        if (geneticSequence.empty())
        {
            throw std::invalid_argument("Genetic sequence is empty.");
        }
        // Ensure we do not divide into more segments than there are characters.
        numSegments = std::min(numSegments, geneticSequence.size());
        size_t seqLength = geneticSequence.size();
        size_t segLength = seqLength / numSegments;
        if (segLength == 0)
        {
            segLength = 1;
            numSegments = seqLength;
        }
        std::vector<Simhash::hash_t> segmentHashes;
        segmentHashes.reserve(numSegments);
        for (size_t i = 0; i < numSegments; ++i)
        {
            std::string segment;
            if (i == numSegments - 1)
            {
                // Last segment takes the remainder of the string.
                segment = geneticSequence.substr(i * segLength);
            }
            else
            {
                segment = geneticSequence.substr(i * segLength, segLength);
            }
            segmentHashes.push_back(custom_nucleotide_hash(segment));
        }
        return Simhash::compute(segmentHashes);
    }

    inline Simhash::hash_t deviateSimhash(Simhash::hash_t original, double ratio)
    {
        const size_t totalBits = 64;
        size_t numToFlip = static_cast<size_t>(ratio * totalBits + 0.5);
        Simhash::hash_t newHash = original;
        uint64_t seed = original;
        for (size_t i = 0; i < numToFlip; i++)
        {
            seed = seed * 6364136223846793005ULL + 1;
            size_t bitIndex = seed % totalBits;
            newHash ^= (1ULL << bitIndex);
        }
        return newHash;
    }
}

#endif // GENETIC_SIMHASH_H
