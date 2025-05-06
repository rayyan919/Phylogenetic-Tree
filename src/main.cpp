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

// int main()
// {
//     try
//     {
//         // 1. Create initial phylogenetic tree from CSV data
//         CSVParser parser("../input/MediumData.csv");
//         auto phyloTree = parser.createPhyloTree();

//         std::cout << "\n=== Initial Phylogenetic Tree ===" << std::endl;
//         phyloTree->print();

//         // 2. Test Case 1: Insert a new species with exact 160-char sequence
//         std::cout << "\n=== Test Case 1: Inserting a New Species ===" << std::endl;

//         std::string testSeq =
//             "ATCGGCTATAGCTAGCTAGCTCAGTCGATCGATCGTACGTACGTACGTACG" // 50
//             "ATCGATCGATCGATCGATCGTAGCTAGCTAGCTAGCTGATCGATCGATCG" // 50
//             "ATCGATCGATCGATCGATCGATCGATCGATCGATATCGGCTATAGCTAGC" // 50
//             "TAGCTCAGTCGATCGATCGTACGTACGTACGTACGATCGATCGATCGATC" // 50
//             "GATCGTAGCTAGCTAGCTAGCTGATCGATCGATCGATCGATCGATCGATC" // 46 (total 246)
//             "GATCG";
//         SpeciesRecord newSpecies("Test Species", testSeq);
//         phyloTree->insert(newSpecies);
//         std::cout << "After insertion:" << std::endl;
//         phyloTree->print();

//         // 3. Test Case 2: Remove a species
//         std::cout << "\n=== Test Case 2: Removing a Species ===" << std::endl;
//         std::string speciesToRemove = "African Elephant";
//         bool removed = phyloTree->remove(speciesToRemove);
//         std::cout << (removed ? "Successfully removed " : "Failed to remove ") << speciesToRemove << std::endl;
//         std::cout << "After removal:" << std::endl;
//         phyloTree->print();

//         // 4. Test Case 3: Mutate a species
//         std::cout << "\n=== Test Case 3: Mutating a Species ===" << std::endl;
//         std::string speciesToMutate = "Asian Elephant";
//         double mutationRatio = phyloTree->mutate(speciesToMutate);
//         if (mutationRatio >= 0) {
//             std::cout << "Mutated " << speciesToMutate << " (mutation ratio: " << mutationRatio << ")" << std::endl;
//         } else {
//             std::cout << speciesToMutate << " not found" << std::endl;
//         }
//         std::cout << "After mutation:" << std::endl;
//         phyloTree->print();

//         // 5. Test Case 4: Search operations
//         std::cout << "\n=== Test Case 4: Search Operations ===" << std::endl;
//         // Search for our test species
//         std::cout << "Searching for Test Species:" << std::endl;
//         bool found = phyloTree->searchAndPrint(SpeciesRecord("Test Species", testSeq));
//         std::cout << (found ? "Found!" : "Not found!") << std::endl;

//         // 6. Test Case 5: Insert a slightly modified version
//         std::cout << "\n=== Test Case 5: Inserting Related Species ===" << std::endl;
//         std::string modifiedSeq = testSeq;
//         modifiedSeq[10] = 'T';  // Single mutation
//         modifiedSeq[75] = 'G';  // Second mutation
//         SpeciesRecord modifiedSpecies("Modified Test Species", modifiedSeq);
//         phyloTree->insert(modifiedSpecies);
//         std::cout << "After inserting modified species:" << std::endl;
//         phyloTree->print();

//         // 7. Test Case 6: Verify both test species exist
//         std::cout << "\n=== Test Case 6: Verifying Both Test Species ===" << std::endl;
//         std::cout << "Searching for original test species:" << std::endl;
//         phyloTree->searchAndPrint(newSpecies);
//         std::cout << "Searching for modified test species:" << std::endl;
//         phyloTree->searchAndPrint(modifiedSpecies);
//     }
//     catch (const std::exception &e)
//     {
//         std::cerr << "Error: " << e.what() << std::endl;
//         return 1;
//     }

//     return 0;
// }