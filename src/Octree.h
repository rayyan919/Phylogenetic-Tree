// #include <iostream>
// #include <vector>
// #include <memory>
// #include <stdexcept>
// #include <string>
// #include <cstdint>
// #include "GeneticSimhash.h"

// static const uint32_t MAX_X = (1U << 21) - 1;
// static const uint32_t MAX_Y = (1U << 21) - 1;
// static const uint32_t MAX_Z = (1U << 22) - 1;

// struct SpeciesRecord
// {
//     std::string speciesName;
//     std::string rleBWT;      // Stored run-length encoded BWT.
//     Simhash::hash_t simhash; // 64-bit simhash generated from the genetic sequence.
//     uint32_t x, y, z;        // 3D coordinates derived from the simhash.

//     SpeciesRecord(const std::string &name,
//                   const std::string &geneticSequence,
//                   const std::string &rleBWT_val)
//         : speciesName(name), rleBWT(rleBWT_val)
//     {
//         // Compute the simhash from the original genetic sequence.
//         simhash = GeneticSimhash::computeSimhash(geneticSequence);

//         // Check if our simhash effectively uses more than 32 bits.
//         // If the upper 32 bits are nonzero, we treat it as a full 64-bit value.
//         if (simhash >> 32)
//         {
//             // Partition a full 64-bit number: use 21 bits for x, 21 for y, 22 for z.
//             x = static_cast<uint32_t>((simhash >> 43) & ((1ULL << 21) - 1));
//             y = static_cast<uint32_t>((simhash >> 22) & ((1ULL << 21) - 1));
//             z = static_cast<uint32_t>(simhash & ((1ULL << 22) - 1));
//         }
//         else
//         {
//             // Otherwise, treat simhash as 32 bits.
//             // Partition a 32-bit number into, for example, 10 bits for x, 11 bits for y, and 11 bits for z.
//             x = static_cast<uint32_t>((simhash >> 22) & ((1U << 10) - 1));
//             y = static_cast<uint32_t>((simhash >> 11) & ((1U << 11) - 1));
//             z = static_cast<uint32_t>(simhash & ((1U << 11) - 1));
//         }

//         std::cout << x << ' ' << y << ' ' << z << std::endl;
//     }
// };

// struct OctreeNode
// {
//     // Bounding box of this node.
//     uint32_t minX, minY, minZ;
//     uint32_t maxX, maxY, maxZ;
//     // Species stored in this node (if a leaf).
//     std::vector<SpeciesRecord> speciesList;
//     // Child nodes – nonempty only if the node has been subdivided.
//     std::vector<std::unique_ptr<OctreeNode>> children;

//     OctreeNode(uint32_t minX_, uint32_t minY_, uint32_t minZ_,
//                uint32_t maxX_, uint32_t maxY_, uint32_t maxZ_)
//         : minX(minX_), minY(minY_), minZ(minZ_),
//           maxX(maxX_), maxY(maxY_), maxZ(maxZ_) {}

//     // Check whether a coordinate (x,y,z) is in this node.
//     bool contains(uint32_t x, uint32_t y, uint32_t z) const
//     {
//         return (x >= minX && x <= maxX &&
//                 y >= minY && y <= maxY &&
//                 z >= minZ && z <= maxZ);
//     }

//     // Whether this node is a leaf (i.e. has no children).
//     bool isLeaf() const
//     {
//         return children.empty();
//     }
// };

// class Octree
// {
// public:
//     // Define the coordinate space based on our mapping:
//     // x and y use 21 bits; z uses 22 bits.
//     // static const uint32_t MAX_X = (1U << 21) - 1;
//     // static const uint32_t MAX_Y = (1U << 21) - 1;
//     // static const uint32_t MAX_Z = (1U << 22) - 1;

//     Octree()
//     {
//         // Root node covers the full coordinate space.
//         root = std::make_unique<OctreeNode>(0, 0, 0, MAX_X, MAX_Y, MAX_Z);
//     }

//     // Insert a species into the octree.
//     void insertSpecies(const std::string &speciesName,
//                        const std::string &geneticSequence,
//                        const std::string &rleBWT)
//     {
//         SpeciesRecord newSpecies(speciesName, geneticSequence, rleBWT);
//         insert(root.get(), newSpecies);
//     }

//     // Remove a species by its name. Returns true if found and removed.
//     bool removeSpecies(const std::string &speciesName)
//     {
//         return remove(root.get(), speciesName);
//     }

//     // Search for a species by its name. Returns pointer to the node that contains it,
//     // or nullptr if not found.
//     OctreeNode *searchSpecies(const std::string &speciesName)
//     {
//         return search(root.get(), speciesName);
//     }

// private:
//     std::unique_ptr<OctreeNode> root;
//     const size_t CAPACITY = 8; // Maximum species per leaf node before subdivision.

//     // Recursive insertion using spatial indexing.
//     void insert(OctreeNode *node, const SpeciesRecord &sp)
//     {
//         // If the node is not a leaf, pass the species down to the appropriate child.
//         if (!node->isLeaf())
//         {
//             int octant = getOctant(node, sp.x, sp.y, sp.z);
//             insert(node->children[octant].get(), sp);
//             return;
//         }

//         // If this is a leaf and there is room, insert the species here.
//         if (node->speciesList.size() < CAPACITY)
//         {
//             node->speciesList.push_back(sp);
//             return;
//         }

//         // Otherwise, subdivide the node and reinsert all species.
//         subdivideNode(node);
//         // Now that children exist, reinsert the species from this node.
//         for (const auto &existing : node->speciesList)
//         {
//             int octant = getOctant(node, existing.x, existing.y, existing.z);
//             insert(node->children[octant].get(), existing);
//         }
//         node->speciesList.clear();
//         // Insert the new species into the appropriate child.
//         int octant = getOctant(node, sp.x, sp.y, sp.z);
//         insert(node->children[octant].get(), sp);
//     }

//     // Recursive removal. Returns true if the species is found and removed.
//     bool remove(OctreeNode *node, const std::string &speciesName)
//     {
//         // Check species stored in this node.
//         for (auto it = node->speciesList.begin(); it != node->speciesList.end(); ++it)
//         {
//             if (it->speciesName == speciesName)
//             {
//                 node->speciesList.erase(it);
//                 return true;
//             }
//         }
//         // Recurse into children if any.
//         for (auto &child : node->children)
//         {
//             if (remove(child.get(), speciesName))
//                 return true;
//         }
//         return false;
//     }

//     // Recursive search for a species by name.
//     OctreeNode *search(OctreeNode *node, const std::string &speciesName)
//     {
//         for (auto &sp : node->speciesList)
//         {
//             if (sp.speciesName == speciesName)
//                 return node;
//         }
//         for (auto &child : node->children)
//         {
//             OctreeNode *res = search(child.get(), speciesName);
//             if (res)
//                 return res;
//         }
//         return nullptr;
//     }

//     // Determine which octant (0-7) a point lies in within the node.
//     int getOctant(const OctreeNode *node, uint32_t x, uint32_t y, uint32_t z)
//     {
//         uint32_t midX = (node->minX + node->maxX) / 2;
//         uint32_t midY = (node->minY + node->maxY) / 2;
//         uint32_t midZ = (node->minZ + node->maxZ) / 2;
//         int octant = 0;
//         if (x > midX)
//             octant |= 1;
//         if (y > midY)
//             octant |= 2;
//         if (z > midZ)
//             octant |= 4;
//         return octant;
//     }

//     // Subdivide a leaf node into eight children.
//     void subdivideNode(OctreeNode *node)
//     {
//         uint32_t midX = (node->minX + node->maxX) / 2;
//         uint32_t midY = (node->minY + node->maxY) / 2;
//         uint32_t midZ = (node->minZ + node->maxZ) / 2;

//         // Reserve space for 8 children.
//         node->children.resize(8);
//         // Octant 0: [minX, midX] x [minY, midY] x [minZ, midZ]
//         node->children[0] = std::make_unique<OctreeNode>(node->minX, node->minY, node->minZ, midX, midY, midZ);
//         // Octant 1: [midX+1, maxX] x [minY, midY] x [minZ, midZ]
//         node->children[1] = std::make_unique<OctreeNode>(midX + 1, node->minY, node->minZ, node->maxX, midY, midZ);
//         // Octant 2: [minX, midX] x [midY+1, maxY] x [minZ, midZ]
//         node->children[2] = std::make_unique<OctreeNode>(node->minX, midY + 1, node->minZ, midX, node->maxY, midZ);
//         // Octant 3: [midX+1, maxX] x [midY+1, maxY] x [minZ, midZ]
//         node->children[3] = std::make_unique<OctreeNode>(midX + 1, midY + 1, node->minZ, node->maxX, node->maxY, midZ);
//         // Octant 4: [minX, midX] x [minY, midY] x [midZ+1, maxZ]
//         node->children[4] = std::make_unique<OctreeNode>(node->minX, node->minY, midZ + 1, midX, midY, node->maxZ);
//         // Octant 5: [midX+1, maxX] x [minY, midY] x [midZ+1, maxZ]
//         node->children[5] = std::make_unique<OctreeNode>(midX + 1, node->minY, midZ + 1, node->maxX, midY, node->maxZ);
//         // Octant 6: [minX, midX] x [midY+1, maxY] x [midZ+1, maxZ]
//         node->children[6] = std::make_unique<OctreeNode>(node->minX, midY + 1, midZ + 1, midX, node->maxY, node->maxZ);
//         // Octant 7: [midX+1, maxX] x [midY+1, maxY] x [midZ+1, maxZ]
//         node->children[7] = std::make_unique<OctreeNode>(midX + 1, midY + 1, midZ + 1, node->maxX, node->maxY, node->maxZ);
//     }
// };

#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include <string>
#include <cstdint>
#include <algorithm>
#include "GeneticSimhash.h" // This header uses our revised simhash logic

// -----------------------------------------------------------------------------
// Global parameters for coordinate scaling & node capacity.
// We want to scale our extracted coordinates so that the entire coordinate range
// is small enough (here 0..4095) to force subdivisions when species differ.
// For a full 64-bit simhash, we originally extracted 21 bits for x, 21 for y, and 22 for z,
// which yields ranges of 0..(2^21-1)=0..2097151 for x and y, and 0..4194303 for z.
// We now scale down:
//   new_x = old_x / 512   (since 2097151/512 ≈ 4095)
//   new_y = old_y / 512
//   new_z = old_z / 1024  (since 4194303/1024 ≈ 4095)
static const uint32_t MAX_X = 4095;
static const uint32_t MAX_Y = 4095;
static const uint32_t MAX_Z = 4095;

// Maximum species per leaf even if similar; also if a new species is not similar
// (Hamming difference > MAX_HAMMING_DIFF) to any species in a leaf, we force subdivision.
static const size_t CAPACITY = 15;
static const size_t MAX_HAMMING_DIFF = 6; // Allowed Hamming difference threshold

// -----------------------------------------------------------------------------
// SpeciesRecord: Holds the species name, rleBWT, computed simhash, and a 3D coordinate.
// The coordinate is derived from the simhash and then scaled down.
struct SpeciesRecord
{
    std::string speciesName;
    std::string rleBWT;      // Stored run-length encoded BWT.
    Simhash::hash_t simhash; // 64-bit simhash computed from the genetic sequence.
    uint32_t x, y, z;        // 3D coordinates after scaling to [0,4095].

    SpeciesRecord(const std::string &name,
                  const std::string &geneticSequence,
                  const std::string &rleBWT_val)
        : speciesName(name), rleBWT(rleBWT_val)
    {
        // Compute the simhash using our GeneticSimhash function.
        simhash = GeneticSimhash::computeSimhash(geneticSequence);

        // First, extract raw coordinates from the simhash.
        // We check if the simhash effectively uses more than 32 bits:
        if (simhash >> 32)
        {
            uint32_t raw_x = static_cast<uint32_t>((simhash >> 43) & ((1ULL << 21) - 1));
            uint32_t raw_y = static_cast<uint32_t>((simhash >> 22) & ((1ULL << 21) - 1));
            uint32_t raw_z = static_cast<uint32_t>(simhash & ((1ULL << 22) - 1));

            // Scale down to [0,4095]:
            x = raw_x / 512;
            y = raw_y / 512;
            z = raw_z / 1024;
        }
        else
        {
            // If simhash is only 32-bit, use a simpler extraction.
            uint32_t raw_x = static_cast<uint32_t>((simhash >> 22) & ((1U << 10) - 1));
            uint32_t raw_y = static_cast<uint32_t>((simhash >> 11) & ((1U << 11) - 1));
            uint32_t raw_z = static_cast<uint32_t>(simhash & ((1U << 11) - 1));
            x = raw_x;
            y = raw_y;
            z = raw_z;
        }
    }
};

// -----------------------------------------------------------------------------
// OctreeNode: Represents one node (family) in the octree.
struct OctreeNode
{
    // Bounding box of this node.
    uint32_t minX, minY, minZ;
    uint32_t maxX, maxY, maxZ;
    // Species stored in the node (only when a leaf).
    std::vector<SpeciesRecord> speciesList;
    // Child nodes; if empty, this node is a leaf.
    std::vector<std::unique_ptr<OctreeNode>> children;

    OctreeNode(uint32_t minX_, uint32_t minY_, uint32_t minZ_,
               uint32_t maxX_, uint32_t maxY_, uint32_t maxZ_)
        : minX(minX_), minY(minY_), minZ(minZ_),
          maxX(maxX_), maxY(maxY_), maxZ(maxZ_) {}

    // Returns true if a given coordinate lies in this node.
    bool contains(uint32_t x, uint32_t y, uint32_t z) const
    {
        return (x >= minX && x <= maxX &&
                y >= minY && y <= maxY &&
                z >= minZ && z <= maxZ);
    }

    // True if this node is a leaf.
    bool isLeaf() const
    {
        return children.empty();
    }
};

// -----------------------------------------------------------------------------
// Octree: The tree structure that clusters species by their 3D coordinates.
class Octree
{
public:
    Octree()
    {
        // Root node covers the full scaled coordinate space [0,4095]^3.
        root = std::make_unique<OctreeNode>(0, 0, 0, MAX_X, MAX_Y, MAX_Z);
    }

    // Insert a species into the octree.
    void insertSpecies(const std::string &speciesName,
                       const std::string &geneticSequence,
                       const std::string &rleBWT)
    {
        SpeciesRecord newSpecies(speciesName, geneticSequence, rleBWT);
        insert(root.get(), newSpecies);
    }

    // Remove a species by name; returns true if removed.
    bool removeSpecies(const std::string &speciesName)
    {
        bool removed = remove(root.get(), speciesName);
        if (removed)
            rebalance(root.get());
        return removed;
    }

    // Search for a species by name; returns pointer to the node that contains it (or nullptr if not found).
    OctreeNode *searchSpecies(const std::string &speciesName)
    {
        return search(root.get(), speciesName);
    }

private:
    std::unique_ptr<OctreeNode> root;

    // Recursive insertion.
    void insert(OctreeNode *node, const SpeciesRecord &sp)
    {
        // Ensure the species' coordinate is within the node.
        if (!node->contains(sp.x, sp.y, sp.z))
            throw std::runtime_error("Species coordinate is out of node bounds.");

        // If node is a leaf, check if we should store the species here.
        if (node->isLeaf())
        {
            if (node->speciesList.empty())
            {
                node->speciesList.push_back(sp);
                return;
            }
            // Check similarity with those already in this leaf.
            bool similarFound = false;
            for (const auto &existing : node->speciesList)
            {
                size_t diff = Simhash::num_differing_bits(existing.simhash, sp.simhash);
                if (diff <= MAX_HAMMING_DIFF)
                {
                    similarFound = true;
                    break;
                }
            }
            // If none is similar, force subdivision to create a new family.
            if (!similarFound)
            {
                subdivideNode(node);
                // Reinsert current species from this leaf.
                for (const auto &existing : node->speciesList)
                {
                    int oct = getOctant(node, existing.x, existing.y, existing.z);
                    insert(node->children[oct].get(), existing);
                }
                node->speciesList.clear();
                int oct = getOctant(node, sp.x, sp.y, sp.z);
                insert(node->children[oct].get(), sp);
                return;
            }
            // Otherwise, if similar is found, add if capacity not exceeded.
            if (node->speciesList.size() < CAPACITY)
            {
                node->speciesList.push_back(sp);
                return;
            }
            // Even if similar, if capacity is reached, subdivide.
            subdivideNode(node);
            for (const auto &existing : node->speciesList)
            {
                int oct = getOctant(node, existing.x, existing.y, existing.z);
                insert(node->children[oct].get(), existing);
            }
            node->speciesList.clear();
            int oct = getOctant(node, sp.x, sp.y, sp.z);
            insert(node->children[oct].get(), sp);
            return;
        }
        // If node is internal, select appropriate child.
        int oct = getOctant(node, sp.x, sp.y, sp.z);
        insert(node->children[oct].get(), sp);
    }

    // Recursive removal.
    bool remove(OctreeNode *node, const std::string &speciesName)
    {
        for (auto it = node->speciesList.begin(); it != node->speciesList.end(); ++it)
        {
            if (it->speciesName == speciesName)
            {
                node->speciesList.erase(it);
                return true;
            }
        }
        for (auto &child : node->children)
        {
            if (remove(child.get(), speciesName))
                return true;
        }
        return false;
    }

    // Recursive search for species.
    OctreeNode *search(OctreeNode *node, const std::string &speciesName)
    {
        for (auto &sp : node->speciesList)
        {
            if (sp.speciesName == speciesName)
                return node;
        }
        for (auto &child : node->children)
        {
            OctreeNode *res = search(child.get(), speciesName);
            if (res)
                return res;
        }
        return nullptr;
    }

    // Get the octant index (0 to 7) for the given coordinate in this node.
    int getOctant(const OctreeNode *node, uint32_t x, uint32_t y, uint32_t z)
    {
        uint32_t midX = (node->minX + node->maxX) / 2;
        uint32_t midY = (node->minY + node->maxY) / 2;
        uint32_t midZ = (node->minZ + node->maxZ) / 2;
        int octant = 0;
        if (x > midX)
            octant |= 1;
        if (y > midY)
            octant |= 2;
        if (z > midZ)
            octant |= 4;
        return octant;
    }

    // Subdivide a leaf node into 8 children.
    void subdivideNode(OctreeNode *node)
    {
        uint32_t midX = (node->minX + node->maxX) / 2;
        uint32_t midY = (node->minY + node->maxY) / 2;
        uint32_t midZ = (node->minZ + node->maxZ) / 2;
        node->children.resize(8);
        node->children[0] = std::make_unique<OctreeNode>(node->minX, node->minY, node->minZ, midX, midY, midZ);
        node->children[1] = std::make_unique<OctreeNode>(midX + 1, node->minY, node->minZ, node->maxX, midY, midZ);
        node->children[2] = std::make_unique<OctreeNode>(node->minX, midY + 1, node->minZ, midX, node->maxY, midZ);
        node->children[3] = std::make_unique<OctreeNode>(midX + 1, midY + 1, node->minZ, node->maxX, node->maxY, midZ);
        node->children[4] = std::make_unique<OctreeNode>(node->minX, node->minY, midZ + 1, midX, midY, node->maxZ);
        node->children[5] = std::make_unique<OctreeNode>(midX + 1, node->minY, midZ + 1, node->maxX, midY, node->maxZ);
        node->children[6] = std::make_unique<OctreeNode>(node->minX, midY + 1, midZ + 1, midX, node->maxY, node->maxZ);
        node->children[7] = std::make_unique<OctreeNode>(midX + 1, midY + 1, midZ + 1, node->maxX, node->maxY, node->maxZ);
    }

    // Rebalance the tree after deletion: remove empty children and merge underpopulated siblings.
    void rebalance(OctreeNode *node)
    {
        if (!node)
            return;
        // Recurse into children:
        for (auto it = node->children.begin(); it != node->children.end();)
        {
            rebalance(it->get());
            if ((*it)->isLeaf() && (*it)->speciesList.empty())
            {
                it = node->children.erase(it);
            }
            else
            {
                ++it;
            }
        }
        // Merge children if all children are leaves and total species count is below CAPACITY.
        if (!node->children.empty())
        {
            bool allLeaf = true;
            size_t totalSpecies = 0;
            for (auto &child : node->children)
            {
                if (!child->isLeaf())
                {
                    allLeaf = false;
                    break;
                }
                totalSpecies += child->speciesList.size();
            }
            if (allLeaf && totalSpecies < CAPACITY)
            {
                for (auto &child : node->children)
                {
                    for (auto &sp : child->speciesList)
                    {
                        node->speciesList.push_back(sp);
                    }
                }
                node->children.clear();
            }
        }
    }
};
