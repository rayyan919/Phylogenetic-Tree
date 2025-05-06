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
    std::unordered_map<std::string, std::tuple<int, int, int>> nameToCoords_;
    // Constructs the tree covering a 3D space from (minX,minY,minZ) to (maxX,maxY,maxZ)
    PhylogeneticTree(int minX, int minY, int minZ,
                     int maxX, int maxY, int maxZ);

    // Insert a species record into the tree
    void insert(const SpeciesRecord &rec);

    // Remove by species name only, using stored coords
    bool remove(const std::string &name);

    // Mutate a species: returns mutation ratio, repositions if mutated
    double mutate(const std::string &name);

    // Search & print the leaf that contains the given probe coords
    bool searchAndPrint(const std::string &name);

    // Print full tree structure
    void print() const;

    int getLength() const { return length; }

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
    static int length;

    // Recursive insert
    void insertRec(Node *node, const SpeciesRecord &rec);

    // Recursive remove
    bool removeRec(Node *node, const std::string &name);

    std::shared_ptr<SpeciesRecord> findSpeciesRec(Node *node, const std::string &name);

    // Helper to find leaf node by coordinates
    Node *findLeaf(Node *node, int x, int y, int z);

    // Print helper to print entire tree
    void printRec(const Node *node, int depth) const;

    // Split a leaf node and redistribute its family
    void subdivideNode(Node *node);

    // AABB containment for SpeciesRecord
    bool contains(const Bounds &b, const SpeciesRecord &r) const;
    bool contains(const Bounds &b, const std::tuple<int, int, int> coords) const;

    bool inBounds(const Bounds &b, int x, int y, int z) const;
    // Compute octant index for a point
    int octantIndex(const Bounds &b, int x, int y, int z) const;
};
