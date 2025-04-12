#include <iostream>
#include <vector>
#include <random>
#include "BWT.h"

namespace MarkovModel
{
    // Operates on the following Markov Probability Model:
    //   A -> A = 0.7;
    //   A -> C = 0.05;
    //   A -> G = 0.15;
    //   A -> T = 0.1;

    //   C -> C = 0.8;
    //   C -> G = 0.09;
    //   C -> T = 0.05;
    //   C -> A = 0.06;

    //   T -> T = 0.6;
    //   T -> C = 0.1;
    //   T -> A = 0.2;
    //   T -> G = 0.1;

    //   G -> G = 0.65;
    //   G -> C = 0.2;
    //   G -> A = 0.1;
    //   G -> T = 0.05;

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
            if (randNum <= 70)
                return base;
            else if (randNum >= 71 && randNum <= 75)
                return 'C';
            else if (randNum >= 76 && randNum <= 90)
                return 'G';
            else
                return 'T';
            break;
        case 'C':
            if (randNum <= 80)
                return base;
            else if (randNum >= 81 && randNum <= 85)
                return 'T';
            else if (randNum >= 86 && randNum <= 91)
                return 'A';
            else
                return 'G';
            break;
        case 'T':
            if (randNum <= 60)
                return base;
            else if (randNum >= 61 && randNum <= 70)
                return 'C';
            else if (randNum >= 71 && randNum <= 90)
                return 'A';
            else
                return 'G';
            break;
        case 'G':
            if (randNum <= 65)
                return base;
            else if (randNum >= 66 && randNum <= 85)
                return 'C';
            else if (randNum >= 86 && randNum <= 95)
                return 'A';
            else
                return 'T';
            break;
        }
        return base;
    }

    double mutateRLEBWT(RLEBWT &rle)
    {
        std::vector<std::pair<int, char>> newRLE;
        std::vector<std::pair<int, char>> inputRLE = rle.get();
        int mutationCount = 0;
        size_t originalLen = 0;
        for (const auto &run : inputRLE)
        {
            originalLen += run.first;
        }

        // Iterate over each run in the input vector.
        for (const auto &run : inputRLE)
        {
            int count = run.first;
            char origChar = run.second;

            // Mutate each base in the run one at a time.
            for (int i = 0; i < count; ++i)
            {
                char mutated = mutateBase(origChar);

                // Count a mutation if the base changed.
                if (mutated != origChar)
                {
                    mutationCount++;
                }

                // Append the mutated base to the new RLE representation.
                // Group runs of the same consecutive letter.
                if (!newRLE.empty() && newRLE.back().second == mutated)
                {
                    newRLE.back().first++;
                }
                else
                {
                    newRLE.push_back({1, mutated});
                }
            }
        }

        // Update the original RLEBWT vector with the mutated result.
        rle.set(newRLE);

        // Compute and return the mutation ratio:
        // (number of mutated bases in the original sequence) / (total original length)
        double mutationRatio = static_cast<double>(mutationCount) / static_cast<double>(originalLen);
        return mutationRatio;
    }
}