#include "CSVParser.h"

int main()
{
    try
    {
        CSVParser parser("../input/MediumData.csv");
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