#pragma once

#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <tuple>
#include "SpeciesRecord.h"
#include "Bktree.h"

class PhylogeneticTree
{
public:
    // Constructs the tree covering a 3D space from (minX,minY,minZ) to (maxX,maxY,maxZ)
    PhylogeneticTree(int minX, int minY, int minZ,
                     int maxX, int maxY, int maxZ, long long int length_)
        : bounds_{minX, minY, minZ, maxX, maxY, maxZ},
          root_(std::make_unique<Node>(bounds_)),
          length(length_) {}

    // Insert a species record into the tree
    void insert(const SpeciesRecord &rec)
    {
        insertRec(root_.get(), rec);
        nameToCoords_[rec.speciesName] = std::make_tuple(rec.x, rec.y, rec.z);
    }

    // Remove by species name only, using stored coords
    bool remove(const std::string &name)
    {
        auto it = nameToCoords_.find(name);
        if (it == nameToCoords_.end())
            return false;
        bool removed = removeRec(root_.get(), name);
        if (removed)
            nameToCoords_.erase(it);
        return removed;
    }

    // Mutate a species: returns mutation ratio, repositions if mutated
    double mutate(const std::string &name)
    {
        auto it = nameToCoords_.find(name);
        if (it == nameToCoords_.end())
            return -1.0;
        std::shared_ptr<SpeciesRecord> rec = findSpeciesRec(root_.get(), name);
        if (!rec)
            return -1.0;
        double ratio = rec->mutate();
        if (ratio > 0.0)
        {
            auto coords = GeneticSimhash::makeCoords(rec->simhash);
            rec->x = std::get<0>(coords);
            rec->y = std::get<1>(coords);
            rec->z = std::get<2>(coords);
            nameToCoords_[name] = std::make_tuple(rec->x, rec->y, rec->z);
            remove(name);
            insert(*rec);
        }
        return ratio;
    }

    // Search & print the leaf that contains the given probe coords
    bool searchAndPrint(const SpeciesRecord &probe) const
    {
        return searchRec(root_.get(), probe);
    }

    // Print full tree structure
    void print() const
    {
        printRec(root_.get(), 0);
    }

private:
    struct Bounds
    {
        int minX, minY, minZ;
        int maxX, maxY, maxZ;
    };

    struct Node
    {
        Bounds b;
        bool isLeaf = true;
        std::unique_ptr<BKSpeciesTree> family;
        std::unique_ptr<Node> children[8];

        explicit Node(const Bounds &bounds)
            : b(bounds), family(std::make_unique<BKSpeciesTree>()) {}
    };

    Bounds bounds_;
    std::unique_ptr<Node> root_;
    std::unordered_map<std::string, std::tuple<int, int, int>> nameToCoords_;
    long long int length;

    // Recursive insert
    void insertRec(Node *node, const SpeciesRecord &rec)
    {
        if (!contains(node->b, rec))
            return;
        if (node->isLeaf)
        {
            if (node->family->canInsert(rec))
            {
                node->family->insert(rec);
                return;
            }
            subdivideNode(node);
        }
        int idx = octantIndex(node->b, rec.x, rec.y, rec.z);
        insertRec(node->children[idx].get(), rec);
    }

    // Recursive remove
    bool removeRec(Node *node, const std::string &name)
    {
        auto it = nameToCoords_.find(name);
        if (it == nameToCoords_.end())
            return false;
        int x = std::get<0>(it->second);
        int y = std::get<1>(it->second);
        int z = std::get<2>(it->second);
        Node *node_ = findLeaf(node, x, y, z);
        if (!node_)
            return false;
        node_->family->remove(name);
        return true;
    }

    std::shared_ptr<SpeciesRecord> findSpeciesRec(Node *node, const std::string &name)
    {
        if (!node)
            return nullptr;
        auto it = nameToCoords_.find(name);
        int x = std::get<0>(it->second);
        int y = std::get<1>(it->second);
        int z = std::get<2>(it->second);
        return findLeaf(node, x, y, z)->family->findExact(name);
    }

    // Recursive search
    bool searchRec(const Node *node, const SpeciesRecord &probe) const
    {
        if (!contains(node->b, probe))
            return false;
        if (node->isLeaf)
        {
            auto results = node->family->search(*probe.seq, 15.5);
            for (const auto &sp : results)
            {
                if (sp->speciesName == probe.speciesName)
                {
                    std::cout << "Found " << probe.speciesName
                              << " in leaf bounds ["
                              << node->b.minX << "," << node->b.maxX << "] x ["
                              << node->b.minY << "," << node->b.maxY << "] x ["
                              << node->b.minZ << "," << node->b.maxZ << "]\n";
                    return true;
                }
            }
            return false;
        }
        int idx = octantIndex(node->b, probe.x, probe.y, probe.z);
        return searchRec(node->children[idx].get(), probe);
    }

    // Helper to find leaf node by coordinates
    Node *findLeaf(Node *node, int x, int y, int z)
    {
        if (!node)
            return nullptr;
        if (node->isLeaf && inBounds(node->b, x, y, z))
            return node;
        int idx = octantIndex(node->b, x, y, z);
        return findLeaf(node->children[idx].get(), x, y, z);
    }

    // Print helper
    void printRec(const Node *node, int depth) const
    {
        std::string indent(depth * 2, ' ');
        if (node->isLeaf)
        {
            std::cout << indent << "Leaf ["
                      << node->b.minX << "," << node->b.maxX << "] x ["
                      << node->b.minY << "," << node->b.maxY << "] x ["
                      << node->b.minZ << "," << node->b.maxZ
                      << "] : " << node->family->size() << " members\n";
        }
        else
        {
            std::cout << indent << "Branch ["
                      << node->b.minX << "," << node->b.maxX << "]\n";
            for (const auto &c : node->children)
            {
                if (c)
                    printRec(c.get(), depth + 1);
            }
        }
    }

    // Split a leaf node and redistribute its family
    void subdivideNode(Node *node)
    {
        node->isLeaf = false;
        auto &b = node->b;
        int midX = (b.minX + b.maxX) >> 1;
        int midY = (b.minY + b.maxY) >> 1;
        int midZ = (b.minZ + b.maxZ) >> 1;
        Bounds bs[8] = {
            {b.minX, b.minY, b.minZ, midX, midY, midZ},
            {midX + 1, b.minY, b.minZ, b.maxX, midY, midZ},
            {b.minX, midY + 1, b.minZ, midX, b.maxY, midZ},
            {midX + 1, midY + 1, b.minZ, b.maxX, b.maxY, midZ},
            {b.minX, b.minY, midZ + 1, midX, midY, b.maxZ},
            {midX + 1, b.minY, midZ + 1, b.maxX, midY, b.maxZ},
            {b.minX, midY + 1, midZ + 1, midX, b.maxY, b.maxZ},
            {midX + 1, midY + 1, midZ + 1, b.maxX, b.maxY, b.maxZ},
        };
        for (int i = 0; i < 8; ++i)
        {
            node->children[i] = std::make_unique<Node>(bs[i]);
        }
        auto oldNodes = node->family->collectAll();
        node->family->reset();
        for (const auto &bkNode : oldNodes)
        {
            const auto &rec = *bkNode->species;
            int idx = octantIndex(node->b, rec.x, rec.y, rec.z);
            node->children[idx]->family->insert(rec);
        }
    }

    // AABB containment for SpeciesRecord
    bool contains(const Bounds &b, const SpeciesRecord &r) const
    {
        return r.x >= b.minX && r.x <= b.maxX && r.y >= b.minY && r.y <= b.maxY && r.z >= b.minZ && r.z <= b.maxZ;
    }

    bool contains(const Bounds &b, const std::tuple<int, int, int> coords) const
    {
        return std::get<0>(coords) >= b.minX && std::get<0>(coords) <= b.maxX &&
               std::get<1>(coords) >= b.minY && std::get<1>(coords) <= b.maxY &&
               std::get<2>(coords) >= b.minZ && std::get<2>(coords) <= b.maxZ;
    }

    bool inBounds(const Bounds &b, int x, int y, int z) const
    {
        return x >= b.minX && x <= b.maxX && y >= b.minY && y <= b.maxY && z >= b.minZ && z <= b.maxZ;
    }

    // Compute octant index for a point
    int octantIndex(const Bounds &b, int x, int y, int z) const
    {
        int idx = 0;
        int midX = (b.minX + b.maxX) >> 1;
        int midY = (b.minY + b.maxY) >> 1;
        int midZ = (b.minZ + b.maxZ) >> 1;
        if (x > midX)
            idx |= 1;
        if (y > midY)
            idx |= 2;
        if (z > midZ)
            idx |= 4;
        return idx;
    }
};

// class OctreeNode
// {
// private:
//     static const int CAPACITY = 4;
//     static int totalSubdivides;

//     int minX, minY, minZ;
//     int maxX, maxY, maxZ;

//     int depth;
//     std::unique_ptr<BKSpeciesTree> family;
//     std::vector<std::unique_ptr<Object3D>> objects;
//     std::unique_ptr<OctreeNode> children[8];

//     bool isLeaf;

// public:
//     OctreeNode(int minX_, int minY_, int minZ_, int maxX_, int maxY_, int maxZ_)
//         : minX(minX_), minY(minY_), minZ(minZ_), maxX(maxX_), maxY(maxY_), maxZ(maxZ_), isLeaf(true) {}

//     ~OctreeNode()
//     {
//         objects.clear();
//         for (auto &child : children)
//             child.reset();
//     }

//     bool contains(const Object3D &obj) const
//     {
//         return obj.x >= minX && obj.x <= maxX &&
//                obj.y >= minY && obj.y <= maxY &&
//                obj.z >= minZ && obj.z <= maxZ;
//     }

//     int getOctantIndex(const Object3D &obj) const
//     {
//         int midX = (minX + maxX) / 2;
//         int midY = (minY + maxY) / 2;
//         int midZ = (minZ + maxZ) / 2;
//         int index = 0;
//         if (obj.x > midX)
//             index |= 1;
//         if (obj.y > midY)
//             index |= 2;
//         if (obj.z > midZ)
//             index |= 4;
//         return index;
//     }

//     void subdivide()
//     {
//         ++totalSubdivides;
//         int midX = (minX + maxX) / 2;
//         int midY = (minY + maxY) / 2;
//         int midZ = (minZ + maxZ) / 2;

//         children[0] = std::make_unique<OctreeNode>(minX, minY, minZ, midX, midY, midZ);
//         children[1] = std::make_unique<OctreeNode>(midX + 1, minY, minZ, maxX, midY, midZ);
//         children[2] = std::make_unique<OctreeNode>(minX, midY + 1, minZ, midX, maxY, midZ);
//         children[3] = std::make_unique<OctreeNode>(midX + 1, midY + 1, minZ, maxX, maxY, midZ);
//         children[4] = std::make_unique<OctreeNode>(minX, minY, midZ + 1, midX, midY, maxZ);
//         children[5] = std::make_unique<OctreeNode>(midX + 1, minY, midZ + 1, maxX, midY, maxZ);
//         children[6] = std::make_unique<OctreeNode>(minX, midY + 1, midZ + 1, midX, maxY, maxZ);
//         children[7] = std::make_unique<OctreeNode>(midX + 1, midY + 1, midZ + 1, maxX, maxY, maxZ);

//         std::vector<std::unique_ptr<Object3D>> temp;
//         temp.swap(objects);
//         for (auto &obj : temp)
//         {
//             int idx = getOctantIndex(*obj);
//             children[idx]->insert(std::move(obj));
//         }

//         isLeaf = false;
//     }

//     void insert(std::unique_ptr<Object3D> obj)
//     {
//         if (!contains(*obj))
//             return;

//         if (isLeaf)
//         {
//             objects.push_back(std::move(obj));
//             if (objects.size() > CAPACITY)
//             {
//                 subdivide();
//             }
//         }
//         else
//         {
//             int idx = getOctantIndex(*obj);
//             children[idx]->insert(std::move(obj));
//         }
//     }

//     bool searchAndPrintNode(const Object3D *obj) const
//     {
//         if (!contains(*obj))
//             return false;

//         if (isLeaf)
//         {
//             for (const auto &o : objects)
//             {
//                 if (o.get() == obj)
//                 {
//                     std::cout << "Found (" << obj->x << ", " << obj->y << ", " << obj->z
//                               << ") in node with bounds: ["
//                               << minX << "," << maxX << "] x ["
//                               << minY << "," << maxY << "] x ["
//                               << minZ << "," << maxZ << "]\n";
//                     return true;
//                 }
//             }
//             return false;
//         }
//         else
//         {
//             int idx = getOctantIndex(*obj);
//             return children[idx]->searchAndPrintNode(obj);
//         }
//     }

//     bool remove(const Object3D *target)
//     {
//         if (!contains(*target))
//             return false;

//         if (isLeaf)
//         {
//             auto it = std::remove_if(objects.begin(), objects.end(),
//                                      [&](const std::unique_ptr<Object3D> &o)
//                                      {
//                                          return o.get() == target;
//                                      });
//             bool found = it != objects.end();
//             objects.erase(it, objects.end());
//             return found;
//         }
//         else
//         {
//             int idx = getOctantIndex(*target);
//             return children[idx]->remove(target);
//         }
//     }

//     void print(int depth = 0) const
//     {
//         std::string indent(depth * 2, ' ');
//         if (isLeaf)
//         {
//             std::cout << indent << "Leaf [" << minX << "," << maxX << "] x ["
//                       << minY << "," << maxY << "] x [" << minZ << "," << maxZ
//                       << "] : " << objects.size() << " object(s)\n";
//             for (const auto &obj : objects)
//             {
//                 std::cout << indent << "  (" << obj->x << ", " << obj->y << ", " << obj->z << ")\n";
//             }
//         }
//         else
//         {
//             std::cout << indent << "Branch Node [" << minX << "," << maxX << "] x ["
//                       << minY << "," << maxY << "] x [" << minZ << "," << maxZ << "]\n";
//             for (const auto &child : children)
//             {
//                 if (child)
//                     child->print(depth + 1);
//             }
//         }
//     }
// };

// int OctreeNode::totalSubdivides = 0;