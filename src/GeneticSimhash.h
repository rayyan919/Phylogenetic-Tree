#ifndef GENETIC_SIMHASH_H
#define GENETIC_SIMHASH_H

#include "simhash.h" // The provided simhash library header.
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <tuple>
#include <algorithm> // for std::min
#include "BitEncodedSeq.h"

namespace GeneticSimhash
{

    uint64_t custom_nucleotide_hash(const BitEncodedSeq &seq,
                                    std::uint32_t start, std::uint32_t length);

    Simhash::hash_t computeSimhash(const BitEncodedSeq &seq, std::size_t numSegments = 32);

    std::tuple<std::uint32_t, std::uint32_t, std::uint32_t> makeCoords(Simhash::hash_t simhash);
}

#endif // GENETIC_SIMHASH_H