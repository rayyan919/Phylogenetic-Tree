#include "../include/CSVParser.h"
#include "../include/Testcases.h"

int main()
{
    try
    {
        // CSVParser parser("../input/MediumData.csv");
        // auto phyloTree = parser.createPhyloTree();
        // std::cout << "Phylogenetic tree created successfully." << std::endl;
        // phyloTree->print();
        // phyloTree->remove("African Elephant");
        // phyloTree->print();

        Testcases testcases("../input/MediumData.csv");
        testcases.testPrint();
        testcases.testMutate("African Elephant");
        testcases.testPrint();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}