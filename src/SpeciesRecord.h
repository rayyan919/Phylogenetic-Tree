#pragma once
#include <cstdint>
#include <memory>
#include <string>
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
                  const std::string &newSeq, std::size_t simhashSegments = 32)
        : speciesName{name}
    {
        this->seq = std::make_unique<BitEncodedSeq>(newSeq); // 2-bit DNA
        recomputeSimhash(simhashSegments);
    }
    // ---------- assignment ----------------------------------------------------
    SpeciesRecord operator=(SpeciesRecord &other)
    {
        swap(*this, other);
        return *this;
    }
    // ---------- copy ctor ----------------------------------------------------
    SpeciesRecord(const SpeciesRecord &other)
        : speciesName{other.speciesName},
          seq(std::make_unique<BitEncodedSeq>(*other.seq)),
          simhash(other.simhash),
          x(other.x),
          y(other.y),
          z(other.z)
    {
    }
    // swap helper
    friend void swap(SpeciesRecord &a, SpeciesRecord &b) noexcept
    {
        using std::swap;
        swap(a.seq, b.seq);
        swap(a.simhash, b.simhash);
        swap(a.x, b.x);
        swap(a.y, b.y);
        swap(a.z, b.z);
        swap(a.speciesName, b.speciesName);
    }

    // ---------- mutate in-place  ----------------------------------------
    // Returns fraction of bases mutated (0.0 … 1.0).
    double mutate(std::size_t simhashSegments = 32)
    {
        double ratio = MarkovModel::mutateEncodedSeq(*seq);
        if (ratio > 0.0)
        {
            recomputeSimhash(simhashSegments);
        }
        return ratio;
    }

private:
    void recomputeSimhash(std::size_t simhashSegments)
    {
        simhash = GeneticSimhash::computeSimhash(*seq, simhashSegments);

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
