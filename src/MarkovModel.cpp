#include "../include/MarkovModel.h"

// Operates on the following Markov Probability Model:
//   A -> A = 0.96;
//   A -> C = 0.01;
//   A -> G = 0.02;
//   A -> T = 0.01;

//   C -> C = 0.95;
//   C -> G = 0.02;
//   C -> T = 0.02;
//   C -> A = 0.01;

//   T -> T = 0.96;
//   T -> C = 0.02;
//   T -> A = 0.01;
//   T -> G = 0.01;

//   G -> G = 0.97;
//   G -> C = 0.01;
//   G -> A = 0.01;
//   G -> T = 0.01;

// The probabilities are based on the following limitations:
// - The model is based on the assumption that the sequence is a DNA sequence.
// - The model is based on the assumption that the sequence is a single strand of DNA.
// - It also assumes only substitution type mutations are allowed. Insertion and deletion mutations are not allowed.
//   This is to ensure the fixed length of the sequence is maintained.
// - The model is based on the assumption that the sequence is a single strand of DNA.
// To leverage O(1) per base mutation this function avoids using complex statistical models.
char MarkovModel::mutateBase(char base)
{
    int min = 1;
    int max = 100;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    int randNum = dis(gen);
    switch (base)
    {
    case 'A':
        if (randNum <= 96)
            return base;
        else if (randNum <= 97)
            return 'C';
        else if (randNum <= 99)
            return 'G';
        else
            return 'T';
        break;
    case 'C':
        if (randNum <= 95)
            return base;
        else if (randNum <= 97)
            return 'T';
        else if (randNum <= 99)
            return 'A';
        else
            return 'G';
        break;
    case 'T':
        if (randNum <= 96)
            return base;
        else if (randNum <= 98)
            return 'C';
        else if (randNum <= 99)
            return 'A';
        else
            return 'G';
        break;
    case 'G':
        if (randNum <= 97)
            return base;
        else if (randNum <= 98)
            return 'C';
        else if (randNum <= 99)
            return 'A';
        else
            return 'T';
        break;
    }

    return base;
}

double MarkovModel::mutateEncodedSeq(BitEncodedSeq &seq)
{
    int mutatedBases = 0;
    for (std::uint32_t i = 0; i < seq.size(); ++i)
    {
        char base = seq.at(i);
        char mutatedBase = mutateBase(base);
        if (base != mutatedBase)
        {
            seq.set(i, mutatedBase);
            ++mutatedBases;
        }
    }
    double raw = static_cast<double>(mutatedBases) / seq.size();
    double truncated = std::floor(raw * 10.0) / 10.0;
    return truncated;
}
