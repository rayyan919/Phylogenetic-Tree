
#include "MarkovModel.h"
#include "Octree.h"

// apply BWT on a gene squence to compress it
// apply minhash to generate a binary fingerprint
// insert into octree using minhash
// insert BWT string into BK-tree( leaf nodes of octree)
//

// int main()
// {
//     std::string geneSeq = "AAGCTTTAGCTA";
//     RLEBWT rlebwt(geneSeq);

//     std::cout << "RLEBWT as vector pairs: ";
//     for (const auto &pair : rlebwt.get())
//     {
//         std::cout << pair.first << pair.second << " ";
//     }
//     std::cout << std::endl;

//     double ratio = MarkovModel::mutateRLEBWT(rlebwt);

//     for (const auto &pair : rlebwt.get())
//     {
//         std::cout << pair.first << pair.second;
//     }
//     std::cout << std::endl;

//     return 0;
// }

// -----------------------------------------------------------------------------
// Main function to test the phylogenetic octree.
int main()
{
    try
    {
        Octree tree;

        // Dummy genetic sequences (they must be of the same length)
        std::string seqA = "ACGTACGCCCCCCCCCCCCCGTTAAAAAAAAAAAAAAAAAAAAGCCGTACGTACGTACGTACGTACGTACGCCCCCCCCCCCCCGTTAAAAAAAAAAAAAAAAAAAAGCCGTACGTACGTACGTACGT";
        std::string seqB = "ACGTACGTACCGAGGACGGTTTTTGCGGACGAACGTACGCGCGGCCGTACGTACGTACGTACGTACGTACGTACCGAGGACGGTTTTTGCGGACGAACGTACGCGCGGCCGTACGTACGTACGTACGT"; // Slight mutation
        std::string seqC = "TGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCATGCA"; // More different

        // Dummy run-length encoded BWT strings.
        std::string rleBWT_A = "RLE_A";
        std::string rleBWT_B = "RLE_B";
        std::string rleBWT_C = "RLE_C";

        // For debugging: print simhash and derived coordinates.
        SpeciesRecord recordA("SpeciesA", seqA, rleBWT_A);
        SpeciesRecord recordB("SpeciesB", seqB, rleBWT_B);
        SpeciesRecord recordC("SpeciesC", seqC, rleBWT_C);
        std::cout << "seqA simhash: " << recordA.simhash << " coord (" << recordA.x << ", " << recordA.y << ", " << recordA.z << ")" << std::endl;
        std::cout << "seqB simhash: " << recordB.simhash << " coord (" << recordB.x << ", " << recordB.y << ", " << recordB.z << ")" << std::endl;
        std::cout << "seqC simhash: " << recordC.simhash << " coord (" << recordC.x << ", " << recordC.y << ", " << recordC.z << ")" << std::endl;

        // Insert species.
        tree.insertSpecies("SpeciesA", seqA, rleBWT_A);
        tree.insertSpecies("SpeciesB", seqB, rleBWT_B);
        tree.insertSpecies("SpeciesC", seqC, rleBWT_C);

        OctreeNode *foundNode = tree.searchSpecies("SpeciesB");
        if (foundNode)
        {
            std::cout << "SpeciesB found in node bounded by: ["
                      << foundNode->minX << ", " << foundNode->minY << ", " << foundNode->minZ
                      << "] to ["
                      << foundNode->maxX << ", " << foundNode->maxY << ", " << foundNode->maxZ
                      << "]" << std::endl;
        }
        else
        {
            std::cout << "SpeciesB not found." << std::endl;
        }

        OctreeNode *foundNodeA = tree.searchSpecies("SpeciesA");
        if (foundNodeA)
        {
            std::cout << "SpeciesA found in node bounded by: ["
                      << foundNodeA->minX << ", " << foundNodeA->minY << ", " << foundNodeA->minZ
                      << "] to ["
                      << foundNodeA->maxX << ", " << foundNodeA->maxY << ", " << foundNodeA->maxZ
                      << "]" << std::endl;
        }
        else
        {
            std::cout << "SpeciesA not found." << std::endl;
        }

        OctreeNode *foundNodeC = tree.searchSpecies("SpeciesC");
        if (foundNodeC)
        {
            std::cout << "SpeciesC found in node bounded by: ["
                      << foundNodeC->minX << ", " << foundNodeC->minY << ", " << foundNodeC->minZ
                      << "] to ["
                      << foundNodeC->maxX << ", " << foundNodeC->maxY << ", " << foundNodeC->maxZ
                      << "]" << std::endl;
        }
        else
        {
            std::cout << "SpeciesC not found." << std::endl;
        }

        // Remove a species.
        if (tree.removeSpecies("SpeciesB"))
        {
            std::cout << "SpeciesB removed." << std::endl;
        }
        else
        {
            std::cout << "SpeciesB not removed (not found)." << std::endl;
        }
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }

    return 0;
}

// bwt   --Rayyan
// octree  ...node --Rayyan
// bktree  ...node --Tanzeel
// bk-octree     --Rayyan
// minhash   --Tanzeel
// Markov model  -- Tanzeel
//  others: jaccard similarily + leven distance
// csv parser