#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include <string>
#include <cstdint>
#include <algorithm>
#include "BKtree.h"
#include "GeneticSimhash.h" // This header our revised simhash logic

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
// If Hamming difference > MAX_HAMMING_DIFF to any species in a leaf, we force subdivision.
static const size_t CAPACITY = 15;
static const size_t MAX_HAMMING_DIFF = 6;

// OctreeNode: Each node represents a family in the octree.
struct OctreeNode
{
    uint32_t minX, minY, minZ;
    uint32_t maxX, maxY, maxZ;
    // For leaves, store species in a BK tree.
    std::unique_ptr<BKSpeciesTree> speciesTree;
    // Children: if empty, this node is a leaf.
    std::vector<std::unique_ptr<OctreeNode>> children;

    OctreeNode(uint32_t minX_, uint32_t minY_, uint32_t minZ_,
               uint32_t maxX_, uint32_t maxY_, uint32_t maxZ_)
        : minX(minX_), minY(minY_), minZ(minZ_),
          maxX(maxX_), maxY(maxY_), maxZ(maxZ_) {}

    // Returns true if the given coordinate is within this node.
    bool contains(uint32_t x, uint32_t y, uint32_t z) const
    {
        return (x >= minX && x <= maxX &&
                y >= minY && y <= maxY &&
                z >= minZ && z <= maxZ);
    }

    bool isLeaf() const { return children.empty(); }
};

// Octree: Clusters species by their 3D coordinates.
class Octree
{
public:
    Octree()
    {
        // The root covers the full coordinate space.
        root = std::make_unique<OctreeNode>(0, 0, 0, MAX_X, MAX_Y, MAX_Z);
    }

    // Insert a species into the octree.
    void insertSpecies(const std::string &speciesName,
                       const std::string &geneticSequence,
                       std::shared_ptr<RLEBWT> rleBWT_val)
    {
        SpeciesRecord newSpecies(speciesName, geneticSequence, rleBWT_val);
        insert(root.get(), newSpecies);
    }

    // Remove a species by name.
    bool removeSpecies(const std::string &speciesName)
    {
        bool removed = remove(root.get(), speciesName);
        if (removed)
            rebalance(root.get());
        return removed;
    }

    // Search for a species by name. Returns the pointer to the octree node containing it (or nullptr).
    OctreeNode *searchSpecies(const std::string &speciesName)
    {
        return search(root.get(), speciesName);
    }

private:
    std::unique_ptr<OctreeNode> root;

    // Determine the octant index (0-7) for a coordinate within a node.
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

    // Recursive insertion into the octree.
    void insert(OctreeNode *node, const SpeciesRecord &sp)
    {
        if (!node->contains(sp.x, sp.y, sp.z))
            throw std::runtime_error("Species coordinate is out of node bounds.");

        if (node->isLeaf())
        {
            // If no BK tree exists yet for this leaf, create one.
            if (!node->speciesTree)
                node->speciesTree = std::make_unique<BKSpeciesTree>();

            // Check for similarity within the existing family (using RLEBWT metric).
            std::vector<std::shared_ptr<SpeciesRecord>> similar;
            node->speciesTree->search(sp.rleBWT->get(), MAX_HAMMING_DIFF, similar);

            // If no similar species found, force subdivision to create a new family.
            if (similar.empty())
            {
                subdivideNode(node);
                // Reinsert species from the BK tree into children.
                auto allSpecies = node->speciesTree->collectAll();
                for (const auto &existing : allSpecies)
                {
                    int oct = getOctant(node, existing->x, existing->y, existing->z);
                    insert(node->children[oct].get(), *existing);
                }
                node->speciesTree.reset();
                int oct = getOctant(node, sp.x, sp.y, sp.z);
                insert(node->children[oct].get(), sp);
                return;
            }

            // If similar species exist and capacity is not exceeded insert.
            if (node->speciesTree->size() < CAPACITY)
            {
                node->speciesTree->insert(std::make_shared<SpeciesRecord>(sp));
                return;
            }

            // Otherwise, if similar exists but capacity is reached, subdivide.
            subdivideNode(node);
            auto allSpecies = node->speciesTree->collectAll();
            for (const auto &existing : allSpecies)
            {
                int oct = getOctant(node, existing->x, existing->y, existing->z);
                insert(node->children[oct].get(), *existing);
            }
            node->speciesTree.reset();
            int oct = getOctant(node, sp.x, sp.y, sp.z);
            insert(node->children[oct].get(), sp);
            return;
        }
        // If node is internal, determine the correct child.
        int oct = getOctant(node, sp.x, sp.y, sp.z);
        insert(node->children[oct].get(), sp);
    }

    // Try to remove from the BK tree if leaf, or from children otherwise.
    bool remove(OctreeNode *node, const std::string &speciesName)
    {
        if (node->isLeaf())
        {
            if (node->speciesTree && node->speciesTree->remove_by_name(speciesName))
                return true;
            return false;
        }
        for (auto &child : node->children)
        {
            if (remove(child.get(), speciesName))
                return true;
        }
        return false;
    }

    // Return pointer to the node that contains the species.
    OctreeNode *search(OctreeNode *node, const std::string &speciesName)
    {
        if (node->isLeaf())
        {
            if (node->speciesTree && node->speciesTree->exists(speciesName))
                return node;
            return nullptr;
        }
        for (auto &child : node->children)
        {
            OctreeNode *res = search(child.get(), speciesName);
            if (res)
                return res;
        }
        return nullptr;
    }

    // Rebalance the tree after deletion: merge underpopulated sibling families.
    void rebalance(OctreeNode *node)
    {
        if (!node)
            return;
        // Recurse into children.
        for (auto it = node->children.begin(); it != node->children.end();)
        {
            rebalance(it->get());
            if ((*it)->isLeaf() && (!(*it)->speciesTree || (*it)->speciesTree->size() == 0))
                it = node->children.erase(it);
            else
                ++it;
        }
        // Merge children if all are leaves and total species count is below CAPACITY.
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
                if (child->speciesTree)
                    totalSpecies += child->speciesTree->size();
            }
            if (allLeaf && totalSpecies < CAPACITY)
            {
                node->speciesTree = std::make_unique<BKSpeciesTree>();
                for (auto &child : node->children)
                {
                    if (child->speciesTree)
                    {
                        auto allSp = child->speciesTree->collectAll();
                        for (auto &sp : allSp)
                        {
                            node->speciesTree->insert(sp);
                        }
                    }
                }
                node->children.clear();
            }
        }
    }
};
