// #pragma once

// #include <vector>
// #include <string>
// #include <algorithm>
// #include <iostream>
// #include <cstdint>
// #include <unordered_set>
// #include <memory>
// #include <unordered_map>
// #include "GeneticSimhash.h"
// #include "SpeciesRecord.h"

// struct BKSpeciesNode
// {
//     std::unique_ptr<SpeciesRecord> species;
//     std::unordered_map<int, std::shared_ptr<BKSpeciesNode>> children;

//     BKSpeciesNode(const std::unique_ptr<SpeciesRecord> &sp) : species(std::move(sp)) {}
// };

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
//                        const std::shared_ptr<SpeciesRecord> &sp)
//     {
//         std::vector<int> keys;
//         for (const auto &pair : node->children)
//             keys.push_back(pair.first);

//         for (int key : keys)
//         {
//             auto child = node->children[key];
//             if (Levenshtein_distance(getRLEBWT(child->species), getRLEBWT(sp)) == 0 &&
//                 child->species->speciesName == sp->speciesName) // match by name as well
//             {
//                 auto subtree = collect_nodes(child);
//                 if (!subtree.empty())
//                     subtree.erase(subtree.begin());
//                 node->children.erase(key);
//                 for (auto &s : subtree)
//                     insert(s);
//                 count--; // adjust count for the removed node
//                 return true;
//             }
//             else
//             {
//                 if (remove_helper(child, sp))
//                     return true;
//             }
//         }
//         return false;
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
