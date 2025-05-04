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

    /// Find a single species by exact sequence+name (returns nullptr if not found)
    std::shared_ptr<SpeciesRecord> findExact(
        const BitEncodedSeq &targetSeq,
        const std::string &targetName) const
    {
        // 1) Query the BK-tree for zero‐distance hits
        auto candidates = search(targetSeq, 0.0);

        // 2) Among those, pick the one with the matching name
        for (auto &recPtr : candidates)
        {
            if (recPtr->speciesName == targetName)
                return recPtr;
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
// class BKSpeciesTree
// {
// private:
//     std::shared_ptr<BKSpeciesNode> root;
//     size_t count; // number of species currently stored

//     double hammingPercentage(const BitEncodedSeq &a,
//                              const BitEncodedSeq &b)
//     {
//         if (a.size() != b.size())
//             throw std::invalid_argument("Sequences must have equal length.");

//         std::uint32_t mismatches = 0;
//         const std::uint32_t n = a.size();

//         for (std::uint32_t i = 0; i < n; ++i)
//             if (a.at(i) != b.at(i))
//                 ++mismatches;

//         return (static_cast<double>(mismatches) / n) * 100.0;
//     }

//     BitEncodedSeq &getEncodedSeq(std::shared_ptr<BKSpeciesNode> node) const
//     {
//         return *node->species->seq;
//     }

//     // Recursive helper to remove a node by species (using its RLEBWT metric).
//     bool remove_helper(const std::shared_ptr<BKSpeciesNode> &node,
//                        const std::string name)
//     {
//         if (!node)
//         {
//             return false;
//         }
//         remove if (node->species->speciesName == name)
//         {
//             // Remove the node by replacing it with its first child.
//             if (!node->children.empty())
//             {
//                 auto first_child = node->children.begin()->second;
//                 node->species = std::move(first_child->species);
//                 node->children = std::move(first_child->children);
//             }
//             else
//             {
//                 node.reset(); // remove leaf node
//             }
//             return true;
//         }
//     }

//     // Helper to collect all species from a given BKSpeciesNode.
//     std::vector<std::shared_ptr<SpeciesRecord>> collect_nodes(const std::shared_ptr<BKSpeciesNode> &node)
//     {
//         std::vector<std::shared_ptr<SpeciesRecord>> result;
//         if (!node)
//             return result;
//         result.push_back(node->species);
//         for (auto &child_pair : node->children)
//         {
//             auto child_nodes = collect_nodes(child_pair.second);
//             result.insert(result.end(), child_nodes.begin(), child_nodes.end());
//         }
//         return result;
//     }

//     // Recursive helper for searching the BK tree.
//     void search_helper(const std::shared_ptr<BKSpeciesNode> &node,
//                        const std::vector<std::pair<int, char>> &target,
//                        int threshold,
//                        std::vector<std::shared_ptr<SpeciesRecord>> &results)
//     {
//         int d = Levenshtein_distance(getRLEBWT(node->species), target);
//         if (d <= threshold)
//             results.push_back(node->species);

//         for (const auto &child_pair : node->children)
//         {
//             int k = child_pair.first;
//             if (k >= d - threshold && k <= d + threshold)
//                 search_helper(child_pair.second, target, threshold, results);
//         }
//     }

// public:
//     BKSpeciesTree() : root(nullptr), count(0) {}

//     // Insert a species into the BK tree.
//     void insert(const std::shared_ptr<SpeciesRecord> &sp)
//     {
//         if (!root)
//         {
//             root = std::make_shared<BKSpeciesNode>(sp);
//             count = 1;
//             return;
//         }
//         auto current = root;
//         while (true)
//         {
//             int d = Levenshtein_distance(getRLEBWT(sp), getRLEBWT(current->species));
//             if (d == 0)
//             {
//                 // If names are also equal, treat as duplicate.
//                 if (sp->speciesName == current->species->speciesName)
//                     return;
//             }
//             auto it = current->children.find(d);
//             if (it != current->children.end())
//             {
//                 current = it->second;
//             }
//             else
//             {
//                 current->children[d] = std::make_shared<BKSpeciesNode>(sp);
//                 count++;
//                 return;
//             }
//         }
//     }

//     // Remove a species given by a shared_ptr.
//     bool remove(const std::shared_ptr<SpeciesRecord> &sp)
//     {
//         if (!root)
//             return false;
//         if (Levenshtein_distance(getRLEBWT(root->species), getRLEBWT(sp)) == 0 &&
//             root->species->speciesName == sp->speciesName)
//         {
//             // Remove root and reinsert its children.
//             auto nodes = collect_nodes(root);
//             if (!nodes.empty())
//                 nodes.erase(nodes.begin());
//             root = nullptr;
//             count = 0;
//             for (const auto &s : nodes)
//                 insert(s);
//             return true;
//         }
//         bool removed = remove_helper(root, sp);
//         if (removed)
//             count--;
//         return removed;
//     }

//     // Remove by species name (traversing all nodes, as CAPACITY is small).
//     bool remove_by_name(const std::string &speciesName)
//     {
//         if (!root)
//             return false;
//         // Collect all species and then try to find the one with matching name.
//         auto all = collectAll();
//         for (auto &s : all)
//         {
//             if (s->speciesName == speciesName)
//                 return remove(s);
//         }
//         return false;
//     }

//     // Search the BK tree for species whose RLEBWT vector is within the threshold
//     // of the given target (using our Levenshtein metric).
//     void search(const std::vector<std::pair<int, char>> &target, int threshold,
//                 std::vector<std::shared_ptr<SpeciesRecord>> &results)
//     {
//         if (!root)
//             return;
//         search_helper(root, target, threshold, results);
//     }

//     // Return the number of species stored.
//     size_t size() const { return count; }

//     // Public helper to collect all species from the tree.
//     std::vector<std::shared_ptr<SpeciesRecord>> collectAll()
//     {
//         if (!root)
//             return {};
//         return collect_nodes(root);
//     }

//     // Check whether a species with the given name exists in the tree.
//     bool exists(const std::string &speciesName)
//     {
//         auto all = collectAll();
//         for (auto &s : all)
//             if (s->speciesName == speciesName)
//                 return true;
//         return false;
//     }
// };
