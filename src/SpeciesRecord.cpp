#include "SpeciesRecord.h"

std::size_t SpeciesRecord::seqLength = 0;
SpeciesRecord::SpeciesRecord(const std::string &name,
                             const std::string &newSeq, std::size_t simhashSegments)
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

SpeciesRecord SpeciesRecord::SpeciesRecord::operator=(SpeciesRecord &other)
{
    swap(*this, other);
    return *this;
}

bool SpeciesRecord::operator==(const SpeciesRecord &other) const
{
    return speciesName == other.speciesName && *seq == *other.seq;
}

SpeciesRecord::SpeciesRecord(const SpeciesRecord &other)
    : speciesName{other.speciesName},
      seq(std::make_unique<BitEncodedSeq>(*other.seq)),
      simhash(other.simhash),
      x(other.x),
      y(other.y),
      z(other.z)
{
}

double SpeciesRecord::mutate(std::size_t simhashSegments)
{
    double ratio = MarkovModel::mutateEncodedSeq(*seq);
    if (ratio > 0.0)
    {
        recomputeSimhash(simhashSegments);
    }
    return ratio;
}

void SpeciesRecord::printSpecies()
{
    std::cout << "Species: " << speciesName << "\n"
              << "Simhash: " << simhash << "\n"
              << "Coordinates: (" << x << ", " << y << ", " << z << ")\n";
    seq->printSeq();
    std::cout << "------------------------------------" << std::endl;
}

void SpeciesRecord::recomputeSimhash(std::size_t simhashSegments)
{
    simhash = GeneticSimhash::computeSimhash(*seq, simhashSegments);
    auto coords = GeneticSimhash::makeCoords(simhash);
    x = std::get<0>(coords);
    y = std::get<1>(coords);
    z = std::get<2>(coords);
}

void swap(SpeciesRecord &a, SpeciesRecord &b) noexcept
{
    using std::swap;
    swap(a.seq, b.seq);
    swap(a.simhash, b.simhash);
    swap(a.x, b.x);
    swap(a.y, b.y);
    swap(a.z, b.z);
    swap(a.speciesName, b.speciesName);
}