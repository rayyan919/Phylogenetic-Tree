#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <cstdint>

class RLE_BWT
{
private:
    std::string rleBWT;

    // RLE-BWT transformer to compress gene sequences
    std::string compRleBWT(const std::string &gene)
    {
        std::vector<std::string> rotations;
        int n = gene.size();
        for (int i = 1; i < n; i++)
        {
            rotations.push_back(gene.substr(n - i - 1) + gene.substr(0, n - i - 1));
        }

        std::sort(rotations.begin(), rotations.end());

        std::string bwt;
        for (int i = 0; i < n; i++)
        {
            bwt += rotations[i][n - 1];
        }

        std::string rleBWT;
        int count = 1;
        for (int i = 1; i < bwt.size(); i++)
        {
            if (bwt[i] == bwt[i - 1])
            {
                count++;
            }
            else
            {
                rleBWT += bwt[i - 1] + std::to_string(count);
                count = 1;
            }
        }

        return rleBWT;
    }

    // Hash function for MinHash computation
    std::uint64_t hashFunction(const std::string &str, int seed)
    {
        std::hash<std::string> hasher;
        return hasher(str + std::to_string(seed));
    }

    std::vector<std::uint64_t> minHash(const std::string &rleBWT, int numHashes)
    {
        std::vector<std::uint64_t> minHashes;
        std::vector<std::uint64_t> hash1;
        std::vector<std::uint64_t> hash2;
        std::vector<std::uint64_t> hash3;
        for (int i = 0; i < numHashes; i++)
        {
                }
    }

public:
    RLE_BWT(std::string &gene)
    {
        rleBWT = compRleBWT(gene);
    }
};

struct BKNode
{
};

struct OctreeNode
{
    BKNode *subtree;
};

class BK_Octree
{
};
