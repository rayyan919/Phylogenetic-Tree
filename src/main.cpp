// #include "Octree.h"
// #include <random>
// #include <ctime>
// #include <vector>
// #include <iostream>

// #include <iostream>
// #include <vector>
// #include <string>
// #include <iomanip>
// #include <cmath>
// #include <chrono>
// #include <random>
// #include <algorithm>
// #include <map>
// #include <cassert>
// #include "GeneticSimhash.h"
// #include "simhash.h"

//===========================================

// LAST EDITED 6TH MAY 2025 AT 6:01 PM

//==========================================

// #include "SpeciesRecord.h"

// int main()
// {
//     try
//     {
//         SpeciesRecord sp1("species1", "ACGTACGTCGCGCGCTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGT");
//         sp1.printSpecies();

//         SpeciesRecord sp3("species3", "ACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTCGCGCTGCTTGAGTCGT");
//         sp3.printSpecies();
//         SpeciesRecord sp4("species4", "ACGTACGTACGTACGTACCGTCGCTGTCGTACGTACGTACGTACGTACGTACGTACGTACGTACGT");
//         sp4.printSpecies();
//         SpeciesRecord sp5("species5", "ACGTACGTACGTACGTAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGTACGTACGT");
//         sp5.printSpecies();
//         SpeciesRecord sp2("species2", "ACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTAC");
//         sp2.printSpecies();
//     }
//     catch (const std::invalid_argument &e)
//     {
//         std::cout << "Error: " << e.what() << std::endl;
//     }
//     return 0;
// }

#include "CSVParser.h"

int main()
{
    try
    {
        CSVParser parser("../input/SmallData.csv");
        auto phyloTree = parser.createPhyloTree();
        std::cout << "Phylogenetic tree created successfully." << std::endl;
        phyloTree->print();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}