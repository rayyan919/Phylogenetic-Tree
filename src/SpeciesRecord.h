// #include <vector>
// #include <string>
// #include <algorithm>
// #include <iostream>
// #include <cstdint>
// #include <unordered_set>
// #include <memory>
// #include <unordered_map>
// #include "GeneticSimhash.h"
// #include "BWT.h"

// // SpeciesRecord: Holds the species name, rleBWT, computed simhash, and a 3D coordinate.
// // The coordinate is derived from the simhash and then scaled down.
// struct SpeciesRecord
// {
//     std::string speciesName;
//     std::unique_ptr<BitEncodedSeq> Seq;
//     Simhash::hash_t simhash;
//     uint32_t x, y, z; // 3D coordinates after scaling to [0,4095].

//     SpeciesRecord(const std::string &name,
//                   const std::string &geneticSequence,
//                   std::unique_ptr<BitEncodedSeq> seq)
//         : speciesName(name), Seq(std::move(seq))
//     {
//         // Compute the simhash using our GeneticSimhash function.
//         simhash = GeneticSimhash::computeSimhash(Seq);

//         if (simhash >> 32)
//         {
//             uint32_t raw_x = static_cast<uint32_t>((simhash >> 43) & ((1ULL << 21) - 1));
//             uint32_t raw_y = static_cast<uint32_t>((simhash >> 22) & ((1ULL << 21) - 1));
//             uint32_t raw_z = static_cast<uint32_t>(simhash & ((1ULL << 22) - 1));

//             // Scale down to [0,4095]:
//             x = raw_x / 512;
//             y = raw_y / 512;
//             z = raw_z / 1024;
//         }
//         else
//         {
//             // If simhash is only 32-bit.
//             uint32_t raw_x = static_cast<uint32_t>((simhash >> 22) & ((1U << 10) - 1));
//             uint32_t raw_y = static_cast<uint32_t>((simhash >> 11) & ((1U << 11) - 1));
//             uint32_t raw_z = static_cast<uint32_t>(simhash & ((1U << 11) - 1));
//             x = raw_x;
//             y = raw_y;
//             z = raw_z;
//         }
//     }
// };

#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include "BitEncodedSeq.h"
#include "GeneticSimhash.h"
#include "MarkovModel.h"

struct SpeciesRecord
{
    std::string speciesName;
    std::unique_ptr<BitEncodedSeq> seq; // 2-bit DNA (non-null)
    Simhash::hash_t simhash{};          // current SimHash
    std::uint32_t x{}, y{}, z{};        // coords 0-4095

    // ---------- ctor ----------------------------------------------------
    SpeciesRecord(const std::string &name,
                  std::unique_ptr<BitEncodedSeq> packedSeq,
                  std::size_t simhashSegments = 32)
        : speciesName{name},
          seq{std::move(packedSeq)}
    {
        recomputeSimhash(simhashSegments);
    }

    // ---------- mutate in-place  ----------------------------------------
    // Returns fraction of bases mutated (0.0 … 1.0).
    double mutate(std::size_t simhashSegments = 32)
    {
        double ratio = MarkovModel::mutateEncodedSeq(*seq);
        if (ratio > 0.0) // something changed → new SimHash/coords
            recomputeSimhash(simhashSegments);
        return ratio;
    }

private:
    void recomputeSimhash(std::size_t numSeg)
    {
        simhash = GeneticSimhash::computeSimhash(*seq, numSeg);

        if (simhash >> 32)
        {
            uint32_t raw_x = static_cast<uint32_t>((simhash >> 43) & ((1ULL << 21) - 1));
            uint32_t raw_y = static_cast<uint32_t>((simhash >> 22) & ((1ULL << 21) - 1));
            uint32_t raw_z = static_cast<uint32_t>(simhash & ((1ULL << 22) - 1));

            // Scale down to [0,4095]:
            x = raw_x / 512;
            y = raw_y / 512;
            z = raw_z / 1024;
        }
        else
        {
            // If simhash is only 32-bit.
            uint32_t raw_x = static_cast<uint32_t>((simhash >> 22) & ((1U << 10) - 1));
            uint32_t raw_y = static_cast<uint32_t>((simhash >> 11) & ((1U << 11) - 1));
            uint32_t raw_z = static_cast<uint32_t>(simhash & ((1U << 11) - 1));
            x = raw_x;
            y = raw_y;
            z = raw_z;
        }
    }
};
