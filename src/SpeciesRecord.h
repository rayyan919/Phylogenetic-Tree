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
                  const std::string &newSeq, std::size_t simhashSegments = 32)
        : speciesName{name}
    {
        if (seqLength == 0)
        {
            seqLength = newSeq.size();
            this->seq = std::make_unique<BitEncodedSeq>(newSeq); // 2-bit DNA
            recomputeSimhash(simhashSegments);
        }
        else if (newSeq.size() == seqLength)
        {
            this->seq = std::make_unique<BitEncodedSeq>(newSeq); // 2-bit DNA
            recomputeSimhash(simhashSegments);
        }
        else if (newSeq.size() > seqLength)
        {
            this->seq = std::make_unique<BitEncodedSeq>(newSeq.substr(0, seqLength)); // 2-bit DNA
            recomputeSimhash(simhashSegments);
        }
        else
        {
            // Skip species with shorter sequences
            throw std::invalid_argument("Species sequence is shorter than the required length.");
        }
    }
    // ---------- assignment ----------------------------------------------------
    SpeciesRecord operator=(SpeciesRecord &other)
    {
        swap(*this, other);
        return *this;
    }

    // ---------- comparator ----------------------------------------------------
    bool operator==(const SpeciesRecord &other) const
    {
        return speciesName == other.speciesName && *seq == *other.seq;
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

    void printSpecies()
    {
        std::cout << "Species: " << speciesName << "\n"
                  << "Simhash: " << simhash << "\n"
                  << "Coordinates: (" << x << ", " << y << ", " << z << ")\n";
        seq->printSeq();
        std::cout << "------------------------------------" << std::endl;
    }

private:
    void recomputeSimhash(std::size_t simhashSegments)
    {
        simhash = GeneticSimhash::computeSimhash(*seq, simhashSegments);
        auto coords = GeneticSimhash::makeCoords(simhash);
        x = std::get<0>(coords);
        y = std::get<1>(coords);
        z = std::get<2>(coords);
    }
};
std::size_t SpeciesRecord::seqLength = 0;
