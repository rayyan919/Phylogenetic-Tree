#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include "GeneticSimhash.h"
#include "MarkovModel.h"

struct SpeciesRecord
{
    static std::size_t seqLength;
    std::string speciesName;
    std::unique_ptr<BitEncodedSeq> seq; // 2-bit DNA (non-null)
    Simhash::hash_t simhash{};          // current SimHash
    std::uint32_t x{}, y{}, z{};        // coords 0-4095

    // ---------- ctor ----------------------------------------------------
    SpeciesRecord(const std::string &name,
                  const std::string &newSeq, std::size_t simhashSegments = 32);
    // ---------- assignment ----------------------------------------------------
    SpeciesRecord operator=(SpeciesRecord &other);

    // ---------- comparator ----------------------------------------------------
    bool operator==(const SpeciesRecord &other) const;

    // ---------- copy ctor ----------------------------------------------------
    SpeciesRecord(const SpeciesRecord &other);

    // ---------- mutate in-place  ----------------------------------------
    // Returns fraction of bases mutated (0.0 … 1.0).
    double mutate(std::size_t simhashSegments = 32);

    void printSpecies();

private:
    void recomputeSimhash(std::size_t simhashSegments);

    // swap helper
    friend void swap(SpeciesRecord &a, SpeciesRecord &b) noexcept;
};
