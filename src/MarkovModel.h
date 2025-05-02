#include <iostream>
#include <vector>
#include <random>
#include "BitEncodedSeq.h"
namespace MarkovModel
{
    // To leverage O(1) per base mutation this function avoids using complex statistical models.
    char mutateBase(char base);

    double mutateEncodedSeq(BitEncodedSeq &seq);
}