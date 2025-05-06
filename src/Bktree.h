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
                             const BitEncodedSeq &b) const
    {
        if (a.size() != b.size())
            throw std::invalid_argument("Sequences must have equal length.");
        uint32_t mismatches = 0;
        uint32_t n = a.size();
        for (uint32_t i = 0; i < n; ++i)
            if (a.at(i) != b.at(i))
                ++mismatches;
        double raw = (static_cast<double>(mismatches) / n) * 100.0;
        double truncated = std::floor(raw * 10.0) / 10.0;
        return truncated; // truncate to 1 decimal place
    }

    // Recursive search helper
    void search_helper(const std::shared_ptr<BKSpeciesNode> &node,
                       const BitEncodedSeq &target,
                       double threshold,
                       std::vector<std::shared_ptr<SpeciesRecord>> &out) const
    {
        double d = hammingPercentage(node->getEncodedSeq(), target);
        if (d <= threshold)
            out.push_back(node->species);

        for (auto &kv : node->children)
        {
            double key = kv.first;
            if (key >= d - threshold && key <= d + threshold)
                search_helper(kv.second, target, threshold, out);
        }
    }

public:
    BKSpeciesTree() = default;

    bool empty() const { return count == 0; }
    size_t size() const { return count; }

    bool canInsert(const SpeciesRecord &sp) const
    {
        // 1) Don’t exceed your hard cap
        if (count >= MAX_CAPACITY)
            return false;

        // 2) Always allow the very first insert
        if (!root)
            return true;

        // 3) Run a BK-tree threshold query:
        //    find all nodes within MAX_HAMMING_DISTANCE of sp
        auto within = search(*sp.seq, MAX_HAMMING_DISTANCE);

        // 4) If that query returned *all* current nodes, you’re good.
        //    Otherwise there’s at least one “too far” species.
        return (within.size() == count);
    }

    // Print the tree with connections and hamming weights
    // Print the tree with connections and hamming weights
    void print() const
    {
        if (!root)
        {
            std::cout << "Tree is empty" << std::endl;
            return;
        }

        // Helper function to print a node and its children
        std::function<void(const std::shared_ptr<BKSpeciesNode> &, const std::string &)> printNode =
            [&printNode](const std::shared_ptr<BKSpeciesNode> &node, const std::string &prefix)
        {
            // Print current node's information
            std::cout << prefix << node->species->speciesName;

            // Print coordinates
            std::cout << " (" << node->species->x << ", " << node->species->y;
            if (node->species->z > 0)
            {
                std::cout << ", " << node->species->z;
            }
            std::cout << ")";

            std::cout << std::endl;

            // Print connections to children
            for (const auto &[hammingWeight, child] : node->children)
            {
                std::cout << prefix << "[___ (" << hammingWeight << "%) --> ";
                std::cout << child->species->speciesName;

                // Print coordinates
                std::cout << " (" << child->species->x << ", " << child->species->y;
                if (child->species->z > 0)
                {
                    std::cout << ", " << child->species->z;
                }
                std::cout << ")";

                std::cout << std::endl;

                // Recursively print children with increased indentation
                printNode(child, prefix + "    ");
            }
        };

        // Start printing from root
        printNode(root, "");

        // Print total count of species
        std::cout << "\nTotal species count: " << count << std::endl;
    }

    /// Find a single species by exact sequence+name (returns nullptr if not found)
    std::shared_ptr<SpeciesRecord> findExact(const std::string &name) const
    {
        if (!root)
            return nullptr;
        std::vector<std::shared_ptr<BKSpeciesNode>> all = collectAll();
        for (auto &node : all)
        {
            if (node->species->speciesName == name)
                return node->species;
        }
        return nullptr;
    }

    // Standard BK-tree insertion (no distance threshold here)
    void insert(const SpeciesRecord &sp)
    {
        if (!canInsert(sp))
            throw std::runtime_error("Cannot insert: capacity or hamming constraint");
        auto newPtr = std::make_shared<SpeciesRecord>(sp);
        if (!root)
        {
            root = std::make_shared<BKSpeciesNode>(sp);
            count = 1;
            return;
        }
        auto cur = root;
        while (true)
        {
            const auto &curSeq = cur->getEncodedSeq();
            double d = hammingPercentage(*newPtr->seq, curSeq);
            if (d == 0.0 && newPtr->speciesName == cur->species->speciesName)
                return; // exact duplicate

            auto it = cur->children.find(d);
            if (it != cur->children.end())
            {
                cur = it->second;
            }
            else
            {
                cur->children.emplace(d, std::make_shared<BKSpeciesNode>(*newPtr));
                ++count;
                return;
            }
        }
    }

    // Search by Hamming threshold
    std::vector<std::shared_ptr<SpeciesRecord>> search(const BitEncodedSeq &target, double threshold) const
    {
        std::vector<std::shared_ptr<SpeciesRecord>> results;
        if (root)
            search_helper(root, target, threshold, results);
        return results;
    }

    // Remove one species by name, rebuild tree in O(n)
    bool remove(const std::string &name)
    {
        if (!root)
            return false;

        auto all = collectAll();
        bool found = false;
        std::vector<SpeciesRecord> survivors;
        survivors.reserve(all.size() - 1);

        for (auto &node : all)
        {
            if (node->species->speciesName == name)
            {
                found = true;
            }
            else
            {
                survivors.push_back(*node->species);
            }
        }
        if (!found)
            return false;

        // rebuild
        reset();
        for (auto &rec : survivors)
            insert(rec);

        return true;
    }

    // Clear the entire tree
    void reset()
    {
        root.reset();
        count = 0;
    }

    // Pre-order collect of every node pointer
    std::vector<std::shared_ptr<BKSpeciesNode>> collectAll() const
    {
        std::vector<std::shared_ptr<BKSpeciesNode>> out;
        if (!root)
            return out;
        std::vector<std::shared_ptr<BKSpeciesNode>> stk{root};
        while (!stk.empty())
        {
            auto node = stk.back();
            stk.pop_back();
            out.push_back(node);
            for (auto &kv : node->children)
                stk.push_back(kv.second);
        }
        return out;
    }
};
