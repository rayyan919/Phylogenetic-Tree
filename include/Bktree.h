#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <cstdint>
#include <unordered_set>
#include <memory>
#include <functional>
#include <stdexcept>
#include <cmath>
#include <unordered_map>
#include "GeneticSimhash.h"
#include "SpeciesRecord.h"

struct BKSpeciesNode
{
    std::shared_ptr<SpeciesRecord> species;
    std::unordered_map<double, std::shared_ptr<BKSpeciesNode>> children;

    BKSpeciesNode(const SpeciesRecord &sp) { species = std::make_shared<SpeciesRecord>(sp); }

    // deep‐copy species, shallow‐copy children
    BKSpeciesNode(const BKSpeciesNode &other)
        : species(std::make_shared<SpeciesRecord>(*other.species)),
          children(other.children)
    {
    }

    // copy‐and‐swap assignment
    BKSpeciesNode &operator=(BKSpeciesNode other)
    {
        swap(*this, other);
        return *this;
    }

    BitEncodedSeq &getEncodedSeq() const
    {
        return *species->seq;
    }

    // swap helper
    friend void swap(BKSpeciesNode &a, BKSpeciesNode &b) noexcept
    {
        using std::swap;
        swap(a.species, b.species);
        swap(a.children, b.children);
    }
};

class BKSpeciesTree
{
private:
    int MAX_CAPACITY = 20;
    double MAX_HAMMING_DISTANCE = 15.5;
    std::shared_ptr<BKSpeciesNode> root;
    size_t count = 0;

    // Compute Hamming distance percentage between two bit sequences
    double hammingPercentage(const BitEncodedSeq &a,
                             const BitEncodedSeq &b) const;

    // Recursive search helper
    void search_helper(const std::shared_ptr<BKSpeciesNode> &node,
                       const BitEncodedSeq &target,
                       double threshold,
                       std::vector<std::shared_ptr<SpeciesRecord>> &out) const;

public:
    BKSpeciesTree() = default;

    bool empty() const;
    size_t size() const;

    bool canInsert(const SpeciesRecord &sp) const;

    // Print the tree with connections and hamming weights
    void print() const;

    /// Find a single species by exact sequence+name (returns nullptr if not found)
    std::shared_ptr<SpeciesRecord> findExact(const std::string &name) const;

    // Standard BK-tree insertion (no distance threshold here)
    void insert(const SpeciesRecord &sp);

    // Search by Hamming threshold
    std::vector<std::shared_ptr<SpeciesRecord>> search(const BitEncodedSeq &target, double threshold) const;

    // Remove one species by name, rebuild tree in O(n)
    bool remove(const std::string &name);

    // Clear the entire tree
    void reset();

    // Pre-order collect of every node pointer
    std::vector<std::shared_ptr<BKSpeciesNode>> collectAll() const;
};
