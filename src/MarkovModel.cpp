#include "MarkovModel.h"

namespace MarkovModel
{
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

    // To leverage O(1) per base mutation this function avoids using complex statistical models.
    char mutateBase(char base)
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

    double mutateEncodedSeq(BitEncodedSeq &seq)
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
        return static_cast<double>(mutatedBases) / seq.size();
    }
}