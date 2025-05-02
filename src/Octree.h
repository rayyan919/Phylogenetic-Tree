// #ifndef OCTREE_H
// #define OCTREE_H

// #include <iostream>
// #include <vector>
// #include <memory>
// #include <stdexcept>
// #include <string>
// #include <cstdint>
// #include "Bktree.h"
// #include "GeneticSimhash.h"

// namespace Phylo
// {

//     static const uint32_t MAX_X = 4095;
//     static const uint32_t MAX_Y = 4095;
//     static const uint32_t MAX_Z = 4095;

//     static const size_t CAPACITY = 15;
//     static const size_t MAX_RLEBWT_DISTANCE = 6;

//     struct OctreeNode
//     {
//         uint32_t minX, minY, minZ;
//         uint32_t maxX, maxY, maxZ;

//         std::unique_ptr<BKSpeciesTree> speciesTree;
//         std::vector<std::unique_ptr<OctreeNode>> children;

//         OctreeNode(uint32_t minX_, uint32_t minY_, uint32_t minZ_,
//                    uint32_t maxX_, uint32_t maxY_, uint32_t maxZ_)
//             : minX(minX_), minY(minY_), minZ(minZ_),
//               maxX(maxX_), maxY(maxY_), maxZ(maxZ_) {}

//         bool contains(uint32_t x, uint32_t y, uint32_t z) const
//         {
//             return (x >= minX && x <= maxX &&
//                     y >= minY && y <= maxY &&
//                     z >= minZ && z <= maxZ);
//         }

//         bool isLeaf() const { return children.empty(); }
//     };

//     class Octree
//     {
//     public:
//         Octree()
//         {
//             root = std::make_unique<OctreeNode>(0, 0, 0, MAX_X, MAX_Y, MAX_Z);
//         }

//         void insertSpecies(const std::shared_ptr<SpeciesRecord> &species)
//         {
//             insert(root.get(), species);
//         }

//         bool removeSpecies(const std::string &speciesName)
//         {
//             bool removed = remove(root.get(), speciesName);
//             if (removed)
//                 rebalance(root.get());
//             return removed;
//         }

//         OctreeNode *searchSpecies(const std::string &speciesName)
//         {
//             return search(root.get(), speciesName);
//         }

//     private:
//         std::unique_ptr<OctreeNode> root;

//         int getOctant(const OctreeNode *node, uint32_t x, uint32_t y, uint32_t z)
//         {
//             uint32_t midX = (node->minX + node->maxX) / 2;
//             uint32_t midY = (node->minY + node->maxY) / 2;
//             uint32_t midZ = (node->minZ + node->maxZ) / 2;

//             int octant = 0;
//             if (x > midX)
//                 octant |= 1;
//             if (y > midY)
//                 octant |= 2;
//             if (z > midZ)
//                 octant |= 4;
//             return octant;
//         }

//         void subdivide(OctreeNode *node)
//         {
//             uint32_t midX = (node->minX + node->maxX) / 2;
//             uint32_t midY = (node->minY + node->maxY) / 2;
//             uint32_t midZ = (node->minZ + node->maxZ) / 2;

//             node->children.resize(8);
//             node->children[0] = std::make_unique<OctreeNode>(node->minX, node->minY, node->minZ, midX, midY, midZ);
//             node->children[1] = std::make_unique<OctreeNode>(midX + 1, node->minY, node->minZ, node->maxX, midY, midZ);
//             node->children[2] = std::make_unique<OctreeNode>(node->minX, midY + 1, node->minZ, midX, node->maxY, midZ);
//             node->children[3] = std::make_unique<OctreeNode>(midX + 1, midY + 1, node->minZ, node->maxX, node->maxY, midZ);
//             node->children[4] = std::make_unique<OctreeNode>(node->minX, node->minY, midZ + 1, midX, midY, node->maxZ);
//             node->children[5] = std::make_unique<OctreeNode>(midX + 1, node->minY, midZ + 1, node->maxX, midY, node->maxZ);
//             node->children[6] = std::make_unique<OctreeNode>(node->minX, midY + 1, midZ + 1, midX, node->maxY, node->maxZ);
//             node->children[7] = std::make_unique<OctreeNode>(midX + 1, midY + 1, midZ + 1, node->maxX, node->maxY, node->maxZ);
//         }

//         // void insert(OctreeNode *node, const std::shared_ptr<SpeciesRecord> &species)
//         // {
//         //     if (!node->contains(species->x, species->y, species->z))
//         //         throw std::runtime_error("Species coordinates out of bounds.");

//         //     if (node->isLeaf())
//         //     {
//         //         if (!node->speciesTree)
//         //         {
//         //             node->speciesTree = std::make_unique<BKSpeciesTree>();
//         //             node->speciesTree->insert(species);
//         //             return;
//         //         }

//         //         std::vector<std::shared_ptr<SpeciesRecord>> similar;
//         //         node->speciesTree->search(species->rleBWT->get(), MAX_RLEBWT_DISTANCE, similar);

//         //         if (similar.empty() || node->speciesTree->size() >= CAPACITY)
//         //         {
//         //             subdivide(node);
//         //             redistribute(node);
//         //             insert(node, species);
//         //         }
//         //         else
//         //         {
//         //             node->speciesTree->insert(species);
//         //         }
//         //         return;
//         //     }

//         //     int oct = getOctant(node, species->x, species->y, species->z);
//         //     insert(node->children[oct].get(), species);
//         // }

//         void insert(OctreeNode *node,
//                     const std::shared_ptr<SpeciesRecord> &species)
//         {
//             if (!node->contains(species->x, species->y, species->z))
//                 throw std::runtime_error("Species coordinates out of bounds.");

//             // --- NEW: if this leaf has never seen any species, just insert and return ---
//             if (node->isLeaf() && !node->speciesTree)
//             {
//                 node->speciesTree = std::make_unique<BKSpeciesTree>();
//                 node->speciesTree->insert(species);
//                 return;
//             }

//             // Now either this is an internal node, or it's a leaf that already has 1+ species
//             if (node->isLeaf())
//             {
//                 // Gather any “similar” neighbors (by your RLEBWT metric)
//                 std::vector<std::shared_ptr<SpeciesRecord>> similar;
//                 node->speciesTree->search(
//                     species->rleBWT->get(),
//                     MAX_RLEBWT_DISTANCE,
//                     similar);

//                 // If none are similar _or_ you've hit capacity, split this family
//                 if (similar.empty() || node->speciesTree->size() >= CAPACITY)
//                 {
//                     subdivide(node);
//                     redistribute(node);
//                     // Now that this node has children, recurse back into it
//                     insert(node, species);
//                 }
//                 else
//                 {
//                     // There's room, and we've found similar kin — just add it
//                     node->speciesTree->insert(species);
//                 }
//                 return;
//             }

//             // If we're here, it's an internal node: pick the correct child by spatial location
//             int oct = getOctant(node, species->x, species->y, species->z);
//             insert(node->children[oct].get(), species);
//         }

//         void redistribute(OctreeNode *node)
//         {
//             auto speciesList = node->speciesTree->collectAll();
//             node->speciesTree.reset();
//             for (const auto &species : speciesList)
//             {
//                 int oct = getOctant(node, species->x, species->y, species->z);
//                 insert(node->children[oct].get(), species);
//             }
//         }

//         bool remove(OctreeNode *node, const std::string &speciesName)
//         {
//             if (node->isLeaf())
//             {
//                 if (node->speciesTree)
//                     return node->speciesTree->remove_by_name(speciesName);
//                 return false;
//             }
//             for (auto &child : node->children)
//             {
//                 if (remove(child.get(), speciesName))
//                     return true;
//             }
//             return false;
//         }

//         OctreeNode *search(OctreeNode *node, const std::string &speciesName)
//         {
//             if (node->isLeaf())
//             {
//                 if (node->speciesTree && node->speciesTree->exists(speciesName))
//                     return node;
//                 return nullptr;
//             }
//             for (auto &child : node->children)
//             {
//                 OctreeNode *found = search(child.get(), speciesName);
//                 if (found)
//                     return found;
//             }
//             return nullptr;
//         }

//         void rebalance(OctreeNode *node)
//         {
//             if (!node)
//                 return;

//             for (auto it = node->children.begin(); it != node->children.end();)
//             {
//                 rebalance(it->get());
//                 if ((*it)->isLeaf() && (!(*it)->speciesTree || (*it)->speciesTree->size() == 0))
//                     it = node->children.erase(it);
//                 else
//                     ++it;
//             }

//             if (!node->children.empty())
//             {
//                 bool allLeaf = true;
//                 size_t totalSpecies = 0;
//                 for (const auto &child : node->children)
//                 {
//                     if (!child->isLeaf())
//                     {
//                         allLeaf = false;
//                         break;
//                     }
//                     if (child->speciesTree)
//                         totalSpecies += child->speciesTree->size();
//                 }
//                 if (allLeaf && totalSpecies < CAPACITY)
//                 {
//                     node->speciesTree = std::make_unique<BKSpeciesTree>();
//                     for (auto &child : node->children)
//                     {
//                         if (child->speciesTree)
//                         {
//                             auto allSpecies = child->speciesTree->collectAll();
//                             for (auto &species : allSpecies)
//                             {
//                                 node->speciesTree->insert(species);
//                             }
//                         }
//                     }
//                     node->children.clear();
//                 }
//             }
//         }
//     };

// } // namespace Phylo

// #endif // OCTREE_H
#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>

struct Object3D
{
    int x, y, z;
    Object3D(int x_, int y_, int z_) : x(x_), y(y_), z(z_) {}
    bool operator==(const Object3D &other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }
};

class OctreeNode
{
public:
    static const int CAPACITY = 4;
    static int totalSubdivides;

    int minX, minY, minZ;
    int maxX, maxY, maxZ;

    bool isLeaf;
    std::vector<std::unique_ptr<Object3D>> objects;
    std::unique_ptr<OctreeNode> children[8];

    OctreeNode(int minX_, int minY_, int minZ_, int maxX_, int maxY_, int maxZ_)
        : minX(minX_), minY(minY_), minZ(minZ_), maxX(maxX_), maxY(maxY_), maxZ(maxZ_), isLeaf(true) {}

    ~OctreeNode()
    {
        objects.clear();
        for (auto &child : children)
            child.reset();
    }

    bool contains(const Object3D &obj) const
    {
        return obj.x >= minX && obj.x <= maxX &&
               obj.y >= minY && obj.y <= maxY &&
               obj.z >= minZ && obj.z <= maxZ;
    }

    int getOctantIndex(const Object3D &obj) const
    {
        int midX = (minX + maxX) / 2;
        int midY = (minY + maxY) / 2;
        int midZ = (minZ + maxZ) / 2;
        int index = 0;
        if (obj.x > midX)
            index |= 1;
        if (obj.y > midY)
            index |= 2;
        if (obj.z > midZ)
            index |= 4;
        return index;
    }

    void subdivide()
    {
        ++totalSubdivides;
        int midX = (minX + maxX) / 2;
        int midY = (minY + maxY) / 2;
        int midZ = (minZ + maxZ) / 2;

        children[0] = std::make_unique<OctreeNode>(minX, minY, minZ, midX, midY, midZ);
        children[1] = std::make_unique<OctreeNode>(midX + 1, minY, minZ, maxX, midY, midZ);
        children[2] = std::make_unique<OctreeNode>(minX, midY + 1, minZ, midX, maxY, midZ);
        children[3] = std::make_unique<OctreeNode>(midX + 1, midY + 1, minZ, maxX, maxY, midZ);
        children[4] = std::make_unique<OctreeNode>(minX, minY, midZ + 1, midX, midY, maxZ);
        children[5] = std::make_unique<OctreeNode>(midX + 1, minY, midZ + 1, maxX, midY, maxZ);
        children[6] = std::make_unique<OctreeNode>(minX, midY + 1, midZ + 1, midX, maxY, maxZ);
        children[7] = std::make_unique<OctreeNode>(midX + 1, midY + 1, midZ + 1, maxX, maxY, maxZ);

        std::vector<std::unique_ptr<Object3D>> temp;
        temp.swap(objects);
        for (auto &obj : temp)
        {
            int idx = getOctantIndex(*obj);
            children[idx]->insert(std::move(obj));
        }

        isLeaf = false;
    }

    void insert(std::unique_ptr<Object3D> obj)
    {
        if (!contains(*obj))
            return;

        if (isLeaf)
        {
            objects.push_back(std::move(obj));
            if (objects.size() > CAPACITY)
            {
                subdivide();
            }
        }
        else
        {
            int idx = getOctantIndex(*obj);
            children[idx]->insert(std::move(obj));
        }
    }

    bool searchAndPrintNode(const Object3D *obj) const
    {
        if (!contains(*obj))
            return false;

        if (isLeaf)
        {
            for (const auto &o : objects)
            {
                if (o.get() == obj)
                {
                    std::cout << "Found (" << obj->x << ", " << obj->y << ", " << obj->z
                              << ") in node with bounds: ["
                              << minX << "," << maxX << "] x ["
                              << minY << "," << maxY << "] x ["
                              << minZ << "," << maxZ << "]\n";
                    return true;
                }
            }
            return false;
        }
        else
        {
            int idx = getOctantIndex(*obj);
            return children[idx]->searchAndPrintNode(obj);
        }
    }

    bool remove(const Object3D *target)
    {
        if (!contains(*target))
            return false;

        if (isLeaf)
        {
            auto it = std::remove_if(objects.begin(), objects.end(),
                                     [&](const std::unique_ptr<Object3D> &o)
                                     {
                                         return o.get() == target;
                                     });
            bool found = it != objects.end();
            objects.erase(it, objects.end());
            return found;
        }
        else
        {
            int idx = getOctantIndex(*target);
            return children[idx]->remove(target);
        }
    }

    void print(int depth = 0) const
    {
        std::string indent(depth * 2, ' ');
        if (isLeaf)
        {
            std::cout << indent << "Leaf [" << minX << "," << maxX << "] x ["
                      << minY << "," << maxY << "] x [" << minZ << "," << maxZ
                      << "] : " << objects.size() << " object(s)\n";
            for (const auto &obj : objects)
            {
                std::cout << indent << "  (" << obj->x << ", " << obj->y << ", " << obj->z << ")\n";
            }
        }
        else
        {
            std::cout << indent << "Branch Node [" << minX << "," << maxX << "] x ["
                      << minY << "," << maxY << "] x [" << minZ << "," << maxZ << "]\n";
            for (const auto &child : children)
            {
                if (child)
                    child->print(depth + 1);
            }
        }
    }
};

int OctreeNode::totalSubdivides = 0;