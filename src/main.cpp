
// #include "MarkovModel.h"
// #include "Octree.h"

// // apply BWT on a gene squence to compress it
// // apply minhash to generate a binary fingerprint
// // insert into octree using minhash
// // insert BWT string into BK-tree( leaf nodes of octree)
// //

// // int main()
// // {
// //     std::string geneSeq = "AAGCTTTAGCTA";
// //     RLEBWT rlebwt(geneSeq);

// //     std::cout << "RLEBWT as vector pairs: ";
// //     for (const auto &pair : rlebwt.get())
// //     {
// //         std::cout << pair.first << pair.second << " ";
// //     }
// //     std::cout << std::endl;

// //     double ratio = MarkovModel::mutateRLEBWT(rlebwt);

// //     for (const auto &pair : rlebwt.get())
// //     {
// //         std::cout << pair.first << pair.second;
// //     }
// //     std::cout << std::endl;

// //     return 0;
// // }

// // int main()
// // {
// //     try
// //     {
// //         Octree tree;

// //         // Dummy genetic sequences (they must be of the same length)
// //         std::string seqA = "ACGTACGCCCCCCCCCCCCCGTTAAAAAAAAAAAAAAAAAAAAGCCGTACGTACGTACGTACGTACGTACGCCCCCCCCCCCCCGTTAAAAAAAAAAAAAAAAAAAAGCCGTACGTACGTACGTACGT";
// //         std::string seqB = "ACGTACGTACCGAGGACGGTTTTTGCGGACGAACGTACGCGCGGCCGTACGTACGTACGTACGTACGTACGTACCGAGGACGGTTTTTGCGGACGAACGTACGCGCGGCCGTACGTACGTACGTACGT"; // Slight mutation
// //         std::string seqC = "TGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCA"; // More different

// //         // Dummy run-length encoded BWT strings.
// //         std::string rleBWT_A = "RLE_A";
// //         std::string rleBWT_B = "RLE_B";
// //         std::string rleBWT_C = "RLE_C";

// //         // For debugging: print simhash and derived coordinates.
// //         SpeciesRecord recordA("SpeciesA", seqA, );
// //         SpeciesRecord recordB("SpeciesB", seqB, );
// //         SpeciesRecord recordC("SpeciesC", seqC, );
// //         std::cout << "seqA simhash: " << recordA.simhash << " coord (" << recordA.x << ", " << recordA.y << ", " << recordA.z << ")" << std::endl;
// //         std::cout << "seqB simhash: " << recordB.simhash << " coord (" << recordB.x << ", " << recordB.y << ", " << recordB.z << ")" << std::endl;
// //         std::cout << "seqC simhash: " << recordC.simhash << " coord (" << recordC.x << ", " << recordC.y << ", " << recordC.z << ")" << std::endl;

// //         // Insert species.
// //         tree.insertSpecies("SpeciesA", seqA, rleBWT_A);
// //         tree.insertSpecies("SpeciesB", seqB, rleBWT_B);
// //         tree.insertSpecies("SpeciesC", seqC, rleBWT_C);

// //         OctreeNode *foundNode = tree.searchSpecies("SpeciesB");
// //         if (foundNode)
// //         {
// //             std::cout << "SpeciesB found in node bounded by: ["
// //                       << foundNode->minX << ", " << foundNode->minY << ", " << foundNode->minZ
// //                       << "] to ["
// //                       << foundNode->maxX << ", " << foundNode->maxY << ", " << foundNode->maxZ
// //                       << "]" << std::endl;
// //         }
// //         else
// //         {
// //             std::cout << "SpeciesB not found." << std::endl;
// //         }

// //         OctreeNode *foundNodeA = tree.searchSpecies("SpeciesA");
// //         if (foundNodeA)
// //         {
// //             std::cout << "SpeciesA found in node bounded by: ["
// //                       << foundNodeA->minX << ", " << foundNodeA->minY << ", " << foundNodeA->minZ
// //                       << "] to ["
// //                       << foundNodeA->maxX << ", " << foundNodeA->maxY << ", " << foundNodeA->maxZ
// //                       << "]" << std::endl;
// //         }
// //         else
// //         {
// //             std::cout << "SpeciesA not found." << std::endl;
// //         }

// //         OctreeNode *foundNodeC = tree.searchSpecies("SpeciesC");
// //         if (foundNodeC)
// //         {
// //             std::cout << "SpeciesC found in node bounded by: ["
// //                       << foundNodeC->minX << ", " << foundNodeC->minY << ", " << foundNodeC->minZ
// //                       << "] to ["
// //                       << foundNodeC->maxX << ", " << foundNodeC->maxY << ", " << foundNodeC->maxZ
// //                       << "]" << std::endl;
// //         }
// //         else
// //         {
// //             std::cout << "SpeciesC not found." << std::endl;
// //         }

// //         // Remove a species.
// //         if (tree.removeSpecies("SpeciesB"))
// //         {
// //             std::cout << "SpeciesB removed." << std::endl;
// //         }
// //         else
// //         {
// //             std::cout << "SpeciesB not removed (not found)." << std::endl;
// //         }
// //     }
// //     catch (const std::exception &ex)
// //     {
// //         std::cerr << "Exception: " << ex.what() << std::endl;
// //     }

// //     return 0;
// // }

// // int main()
// // {
// //     try
// //     {
// //         Octree tree;

// //         // Step 1: Original gene sequences
// //         std::string geneA = "AACCTTGGAA";
// //         std::string geneB = "AACCTTGAAA"; // slightly different
// //         std::string geneC = "GGGTTTCCCC"; // more different

// //         // Step 2: Create RLEBWTs
// //         std::shared_ptr<RLEBWT> rleA = std::make_shared<RLEBWT>(geneA);
// //         std::shared_ptr<RLEBWT> rleB = std::make_shared<RLEBWT>(geneB);
// //         std::shared_ptr<RLEBWT> rleC = std::make_shared<RLEBWT>(geneC);

// //         // Step 3: Mutate geneA (optional)
// //         double mutationRatio = MarkovModel::mutateRLEBWT(rleA);
// //         std::cout << "[Mutation Ratio for SpeciesA]: " << mutationRatio << "\n";

// //         // Step 4: Insert into octree
// //         tree.insertSpecies("SpeciesA", geneA, rleA);
// //         tree.insertSpecies("SpeciesB", geneB, rleB);
// //         tree.insertSpecies("SpeciesC", geneC, rleC);

// //         // Step 5: Search each
// //         for (const std::string &name : {"SpeciesA", "SpeciesB", "SpeciesC"})
// //         {
// //             OctreeNode *found = tree.searchSpecies(name);
// //             if (found)
// //             {
// //                 std::cout << name << " found in node bounded by: ["
// //                           << found->minX << ", " << found->minY << ", " << found->minZ
// //                           << "] to [" << found->maxX << ", " << found->maxY << ", " << found->maxZ << "]\n";
// //             }
// //             else
// //             {
// //                 std::cout << name << " not found.\n";
// //             }
// //         }

// //         // Step 6: Delete one and verify
// //         std::cout << "\nAttempting to remove SpeciesB...\n";
// //         if (tree.removeSpecies("SpeciesB"))
// //             std::cout << "SpeciesB removed successfully.\n";
// //         else
// //             std::cout << "SpeciesB was not found or could not be removed.\n";

// //         OctreeNode *postDelete = tree.searchSpecies("SpeciesB");
// //         std::cout << "After deletion, SpeciesB is "
// //                   << (postDelete ? "still found (❌)\n" : "not found (✅)\n");
// //     }
// //     catch (const std::exception &ex)
// //     {
// //         std::cerr << "Exception: " << ex.what() << "\n";
// //     }

// //     return 0;
// // }
// // using namespace Phylo;
// // int main()
// // {
// //     try
// //     {
// //         Octree tree;

// //         // Dummy genetic sequences (they must be of the same length)
// //         std::string seqA = "ACGTACGCCCCCCCCCCCCCGTTAAAAAAAAAAAAAAAAAAAAGCCGTACGTACGTACGTACGTACGTACGCCCCCCCCCCCCCGTTAAAAAAAAAAAAAAAAAAAAGCCGTACGTACGTACGTACGT";
// //         std::string seqB = "ACGTACGTACCGAGGACGGTTTTTGCGGACGAACGTACGCGCGGCCGTACGTACGTACGTACGTACGTACGTACCGAGGACGGTTTTTGCGGACGAACGTACGCGCGGCCGTACGTACGTACGTACGT"; // Slight mutation
// //         std::string seqC = "TGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCA"; // More different

// //         auto rleA = std::make_shared<RLEBWT>(seqA);
// //         auto rleB = std::make_shared<RLEBWT>(seqB);
// //         auto rleC = std::make_shared<RLEBWT>(seqC);

// //         std::string nameA = "SpeciesA";
// //         std::string nameB = "SpeciesB";
// //         std::string nameC = "SpeciesC";

// //         // Debug: compute one record each and print simhash/coords
// //         SpeciesRecord recA(nameA, seqA, rleA);
// //         SpeciesRecord recB(nameB, seqB, rleB);
// //         SpeciesRecord recC(nameC, seqC, rleC);
// //         std::cout << "seqA simhash: " << recA.simhash
// //                   << " coord(" << recA.x << "," << recA.y << "," << recA.z << ")\n";
// //         std::cout << "seqB simhash: " << recB.simhash
// //                   << " coord(" << recB.x << "," << recB.y << "," << recB.z << ")\n";
// //         std::cout << "seqC simhash: " << recC.simhash
// //                   << " coord(" << recC.x << "," << recC.y << "," << recC.z << ")\n";

// //         // **Now actually insert them into the octree:**
// //         tree.insertSpecies(nameA, seqA, rleA);
// //         tree.insertSpecies(nameB, seqB, rleB);
// //         tree.insertSpecies(nameC, seqC, rleC);

// //         // **And now search & print the bounding boxes:**
// //         auto printFind = [&](const std::string &name)
// //         {
// //             OctreeNode *n = tree.searchSpecies(name);
// //             if (n)
// //             {
// //                 std::cout << name << " in node ["
// //                           << n->minX << "," << n->minY << "," << n->minZ
// //                           << "]→[" << n->maxX << "," << n->maxY << "," << n->maxZ << "]\n";
// //             }
// //             else
// //             {
// //                 std::cout << name << " not found\n";
// //             }
// //         };
// //         printFind("SpeciesA");
// //         printFind("SpeciesB");
// //         printFind("SpeciesC");

// //         // Finally remove one and show that it’s gone:
// //         tree.removeSpecies("SpeciesB");
// //         printFind("SpeciesB");
// //     }
// //     catch (const std::exception &ex)
// //     {
// //         std::cerr << "Exception: " << ex.what() << std::endl;
// //     }
// //     return 0;
// // }

// using namespace Phylo;

// int main()
// {
//     try
//     {
//         Octree tree;

//         // All sequences must be the same length (≥96 bases)
//         std::string seqA =
//             "ACGTACGTACCGAGGACGGTTTTTGCGAAAAAAAAAACGCGCGGCCGTACGTACGTACGTACGTACGTACGT"
//             "ACCGAGGACGGTTTTTGCGGACGAACGTACGCGCGGCCGTACGTACGTACGTACGT";
//         std::string seqB =
//             "ACGTACGTACCGAGGACGGTTTTTGCGGACGAACGTACGCGCGGCCGTACGTACGTACGTACGTACGTACGT"
//             "ACCGAGGACGGTTTTTGCGGACGAACGTACGCGCGGCCGTACGTACGTACGTACGT";
//         std::string seqC =
//             "TGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATG"
//             "CATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCA";

//         // Wrap into shared_ptr<RLEBWT>
//         auto rleA = std::make_shared<RLEBWT>(seqA);
//         auto rleB = std::make_shared<RLEBWT>(seqB);
//         auto rleC = std::make_shared<RLEBWT>(seqC);

//         // Names
//         std::string nameA = "SpeciesA";
//         std::string nameB = "SpeciesB";
//         std::string nameC = "SpeciesC";

//         // Quick standalone record creation for debug print

//         SpeciesRecord recA(nameA, seqA, rleA);
//         SpeciesRecord recB(nameB, seqB, rleB);
//         SpeciesRecord recC(nameC, seqC, rleC);
//         std::cout << "SpeciesA → simhash: " << recA.simhash
//                   << " coord(" << recA.x << "," << recA.y << "," << recA.z << ")\n";
//         std::cout << "SpeciesB → simhash: " << recB.simhash
//                   << " coord(" << recB.x << "," << recB.y << "," << recB.z << ")\n";
//         std::cout << "SpeciesC → simhash: " << recC.simhash
//                   << " coord(" << recC.x << "," << recC.y << "," << recC.z << ")\n";

//         // Insert into Octree
//         tree.insertSpecies(std::make_shared<SpeciesRecord>(recA));
//         tree.insertSpecies(std::make_shared<SpeciesRecord>(recB));
//         tree.insertSpecies(std::make_shared<SpeciesRecord>(recC));

//         // Helper to print which node a species lives in
//         auto printLocation = [&](const std::string &spName)
//         {
//             OctreeNode *node = tree.searchSpecies(spName);
//             if (node)
//             {
//                 std::cout << spName << " is in leaf bounding box ["
//                           << node->minX << "," << node->minY << "," << node->minZ
//                           << "] → [" << node->maxX << "," << node->maxY << "," << node->maxZ
//                           << "]\n";
//             }
//             else
//             {
//                 std::cout << spName << " not found in tree\n";
//             }
//         };

//         printLocation(nameA);
//         printLocation(nameB);
//         printLocation(nameC);

//         // Test removal
//         std::cout << "\nRemoving " << nameB << "...\n";
//         if (tree.removeSpecies(nameB))
//         {
//             std::cout << nameB << " removed successfully\n";
//         }
//         else
//         {
//             std::cout << "Failed to remove " << nameB << "\n";
//         }
//         printLocation(nameB);
//     }
//     catch (const std::exception &ex)
//     {
//         std::cerr << "Error: " << ex.what() << std::endl;
//         return 1;
//     }
//     return 0;
// }

// // bwt   --Rayyan
// // octree  ...node --Rayyan
// // bktree  ...node --Tanzeel
// // bk-octree     --Rayyan
// // minhash   --Tanzeel
// // Markov model  -- Tanzeel
// //  others: jaccard similarily + leven distance
// // csv parser

// #include <iostream>
// #include <vector>
// #include <string>
// #include <cstdint>
// #include <bitset>
// #include "BitEncodedSeq.h"
// #include "MarkovModel.h"
// using namespace MarkovModel;
// int main()
// {

//     std::string geneA = "ACTCGTACAAAAGTCG";

//     BitEncodedSeq bitSeq(geneA);
//     std::cout << bitSeq.at(0) << std::endl;                                                 // Access the first base
//     bitSeq.set(0, 'G');                                                                     // Set the first base to 'G'
//     std::cout << "First base after mutation: " << bitSeq.at(0) << std::endl;                // Should print 'G'
//     std::cout << "Size of the sequence: " << bitSeq.size() << std::endl;                    // Should print 16
//     std::cout << "Is the sequence empty? " << (bitSeq.empty() ? "Yes" : "No") << std::endl; // Should print No
//     std::cout << bitSeq.at(0) << ' ' << bitSeq.at(1) << ' ' << bitSeq.at(14) << std::endl;  // Should print 'G'
//     // Should print 'C' and 'C' respectively
//     bitSeq.printSeq();     // Print the entire sequence
//     bitSeq.printEncoded(); // Print the encoded sequence

//     mutateEncodedSeq(bitSeq);
//     bitSeq.printSeq();
//     bitSeq.printEncoded();

//     mutateEncodedSeq(bitSeq);
//     bitSeq.printSeq();
//     bitSeq.printEncoded();

//     mutateEncodedSeq(bitSeq);
//     bitSeq.printSeq();
//     bitSeq.printEncoded();

//     mutateEncodedSeq(bitSeq);
//     bitSeq.printSeq();
//     bitSeq.printEncoded();

//     return 0;
// }

#include "Octree.h"
#include <random>
#include <ctime>
#include <vector>
#include <iostream>

int main()
{
    OctreeNode root(0, 0, 0, 4095, 4095, 4095);

    std::vector<Object3D *> rawRefs;
    rawRefs.reserve(20);

    std::mt19937 rng(static_cast<unsigned int>(time(nullptr)));
    std::uniform_int_distribution<int> dist(0, 4095);

    std::cout << "=== Inserting Objects ===\n";
    for (int i = 0; i < 20; ++i)
    {
        int x = dist(rng);
        int y = dist(rng);
        int z = dist(rng);
        auto obj = std::make_unique<Object3D>(x, y, z);
        Object3D *ref = obj.get();
        rawRefs.push_back(ref);
        root.insert(std::move(obj));
        std::cout << "Inserted (" << x << ", " << y << ", " << z << ")\n";
    }

    std::cout << "\n=== Octree After Insertions ===\n";
    root.print();

    std::cout << "\n=== Removing First 7 Objects ===\n";
    for (int i = 0; i < 7; ++i)
    {
        Object3D *ptr = rawRefs[i];
        int x = ptr->x;
        int y = ptr->y;
        int z = ptr->z;
        bool removed = root.remove(ptr);
        std::cout << "Removed (" << x << ", " << y << ", " << z << "): "
                  << (removed ? "Success" : "Failure") << "\n";
        rawRefs[i] = nullptr;
    }

    std::cout << "\n=== Octree After Removals ===\n";
    root.print();

    std::cout << "\n=== Total Subdivisions: " << OctreeNode::totalSubdivides << " ===\n";

    return 0;
}