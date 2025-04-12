#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <cstdint>
#include <unordered_set>
#include <memory>
#include <unordered_map>
#include "BWT.h"
#include "GeneticSimhash.h"

// SpeciesRecord: Holds the species name, rleBWT, computed simhash, and a 3D coordinate.
// The coordinate is derived from the simhash and then scaled down.
struct SpeciesRecord
{
    std::string speciesName;
    std::shared_ptr<RLEBWT> rleBWT;
    Simhash::hash_t simhash;
    uint32_t x, y, z; // 3D coordinates after scaling to [0,4095].

    SpeciesRecord(const std::string &name,
                  const std::string &geneticSequence,
                  std::shared_ptr<RLEBWT> rleBWT_val)
        : speciesName(name), rleBWT(std::move(rleBWT_val))
    {
        // Compute the simhash using our GeneticSimhash function.
        simhash = GeneticSimhash::computeSimhash(geneticSequence);

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
            // If simhash is only 32-bit.
            uint32_t raw_x = static_cast<uint32_t>((simhash >> 22) & ((1U << 10) - 1));
            uint32_t raw_y = static_cast<uint32_t>((simhash >> 11) & ((1U << 11) - 1));
            uint32_t raw_z = static_cast<uint32_t>(simhash & ((1U << 11) - 1));
            x = raw_x;
            y = raw_y;
            z = raw_z;
        }
    }
};

struct BKSpeciesNode
{
    std::shared_ptr<SpeciesRecord> species;
    std::unordered_map<int, std::shared_ptr<BKSpeciesNode>> children;

    BKSpeciesNode(const std::shared_ptr<SpeciesRecord> &sp) : species(sp) {}
};

class BKSpeciesTree
{
private:
    std::shared_ptr<BKSpeciesNode> root;
    size_t count; // number of species currently stored

    // Compute Levenshtein-like distance between two RLEBWTs.
    int Levenshtein_distance(const std::vector<std::pair<int, char>> &seq1,
                             const std::vector<std::pair<int, char>> &seq2)
    {
        size_t m = seq1.size(), n = seq2.size();
        std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
        for (size_t i = 0; i <= m; i++)
            dp[i][0] = i;
        for (size_t j = 0; j <= n; j++)
            dp[0][j] = j;
        for (size_t i = 1; i <= m; i++)
        {
            for (size_t j = 1; j <= n; j++)
            {
                if (seq1[i - 1].second == seq2[j - 1].second)
                {
                    int diff = std::abs(seq1[i - 1].first - seq2[j - 1].first);
                    dp[i][j] = std::min({dp[i - 1][j] + 1,
                                         dp[i][j - 1] + 1,
                                         dp[i - 1][j - 1] + diff});
                }
                else
                {
                    dp[i][j] = std::min({dp[i - 1][j] + 1,
                                         dp[i][j - 1] + 1,
                                         dp[i - 1][j - 1] + 1});
                }
            }
        }
        return dp[m][n];
    }

    // Helper to extract the RLEBWT vector from a species.
    const std::vector<std::pair<int, char>> &getRLEBWT(const std::shared_ptr<SpeciesRecord> &sp) const
    {
        return sp->rleBWT->get();
    }

    // Recursive helper to remove a node by species (using its RLEBWT metric).
    bool remove_helper(const std::shared_ptr<BKSpeciesNode> &node,
                       const std::shared_ptr<SpeciesRecord> &sp)
    {
        std::vector<int> keys;
        for (const auto &pair : node->children)
            keys.push_back(pair.first);

        for (int key : keys)
        {
            auto child = node->children[key];
            if (Levenshtein_distance(getRLEBWT(child->species), getRLEBWT(sp)) == 0 &&
                child->species->speciesName == sp->speciesName) // match by name as well
            {
                auto subtree = collect_nodes(child);
                if (!subtree.empty())
                    subtree.erase(subtree.begin());
                node->children.erase(key);
                for (auto &s : subtree)
                    insert(s);
                count--; // adjust count for the removed node
                return true;
            }
            else
            {
                if (remove_helper(child, sp))
                    return true;
            }
        }
        return false;
    }

    // Helper to collect all species from a given BKSpeciesNode.
    std::vector<std::shared_ptr<SpeciesRecord>> collect_nodes(const std::shared_ptr<BKSpeciesNode> &node)
    {
        std::vector<std::shared_ptr<SpeciesRecord>> result;
        if (!node)
            return result;
        result.push_back(node->species);
        for (auto &child_pair : node->children)
        {
            auto child_nodes = collect_nodes(child_pair.second);
            result.insert(result.end(), child_nodes.begin(), child_nodes.end());
        }
        return result;
    }

    // Recursive helper for searching the BK tree.
    void search_helper(const std::shared_ptr<BKSpeciesNode> &node,
                       const std::vector<std::pair<int, char>> &target,
                       int threshold,
                       std::vector<std::shared_ptr<SpeciesRecord>> &results)
    {
        int d = Levenshtein_distance(getRLEBWT(node->species), target);
        if (d <= threshold)
            results.push_back(node->species);

        for (const auto &child_pair : node->children)
        {
            int k = child_pair.first;
            if (k >= d - threshold && k <= d + threshold)
                search_helper(child_pair.second, target, threshold, results);
        }
    }

public:
    BKSpeciesTree() : root(nullptr), count(0) {}

    // Insert a species into the BK tree.
    void insert(const std::shared_ptr<SpeciesRecord> &sp)
    {
        if (!root)
        {
            root = std::make_shared<BKSpeciesNode>(sp);
            count = 1;
            return;
        }
        auto current = root;
        while (true)
        {
            int d = Levenshtein_distance(getRLEBWT(sp), getRLEBWT(current->species));
            if (d == 0)
            {
                // If names are also equal, treat as duplicate.
                if (sp->speciesName == current->species->speciesName)
                    return;
            }
            auto it = current->children.find(d);
            if (it != current->children.end())
            {
                current = it->second;
            }
            else
            {
                current->children[d] = std::make_shared<BKSpeciesNode>(sp);
                count++;
                return;
            }
        }
    }

    // Remove a species given by a shared_ptr.
    bool remove(const std::shared_ptr<SpeciesRecord> &sp)
    {
        if (!root)
            return false;
        if (Levenshtein_distance(getRLEBWT(root->species), getRLEBWT(sp)) == 0 &&
            root->species->speciesName == sp->speciesName)
        {
            // Remove root and reinsert its children.
            auto nodes = collect_nodes(root);
            if (!nodes.empty())
                nodes.erase(nodes.begin());
            root = nullptr;
            count = 0;
            for (const auto &s : nodes)
                insert(s);
            return true;
        }
        bool removed = remove_helper(root, sp);
        if (removed)
            count--;
        return removed;
    }

    // Remove by species name (traversing all nodes, as CAPACITY is small).
    bool remove_by_name(const std::string &speciesName)
    {
        if (!root)
            return false;
        // Collect all species and then try to find the one with matching name.
        auto all = collectAll();
        for (auto &s : all)
        {
            if (s->speciesName == speciesName)
                return remove(s);
        }
        return false;
    }

    // Search the BK tree for species whose RLEBWT vector is within the threshold
    // of the given target (using our Levenshtein metric).
    void search(const std::vector<std::pair<int, char>> &target, int threshold,
                std::vector<std::shared_ptr<SpeciesRecord>> &results)
    {
        if (!root)
            return;
        search_helper(root, target, threshold, results);
    }

    // Return the number of species stored.
    size_t size() const { return count; }

    // Public helper to collect all species from the tree.
    std::vector<std::shared_ptr<SpeciesRecord>> collectAll()
    {
        if (!root)
            return {};
        return collect_nodes(root);
    }

    // Check whether a species with the given name exists in the tree.
    bool exists(const std::string &speciesName)
    {
        auto all = collectAll();
        for (auto &s : all)
            if (s->speciesName == speciesName)
                return true;
        return false;
    }
};
