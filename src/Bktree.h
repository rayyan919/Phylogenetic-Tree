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

// std::vector<std::pair<int, char>> parseRLE(const std::string &rle) {
//     std::vector<std::pair<int, char>> result;
//     int n = rle.size(), i = 0;

//     while (i < n) {
//         int count = 0;
//         while (i < n && isdigit(rle[i])) {
//             count = count * 10 + (rle[i] - '0');
//             i++;
//         }
//         if (i < n && isalpha(rle[i])) {
//             result.emplace_back(count, rle[i]);
//             i++;
//         }
//     }
//     return result;
// }

//     int levenshteinDistance(const std::string &rle1, const std::string &rle2) {
//     auto seq1 = parseRLE(rle1);
//     auto seq2 = parseRLE(rle2);

//     int m = seq1.size(), n = seq2.size();
//     std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));

//     for (int i = 0; i <= m; i++) dp[i][0] = i;
//     for (int j = 0; j <= n; j++) dp[0][j] = j;

//     for (int i = 1; i <= m; i++) {
//         for (int j = 1; j <= n; j++) {
//             if (seq1[i - 1].second == seq2[j - 1].second) {
//                 int diff = std::abs(seq1[i - 1].first - seq2[j - 1].first);
//                 dp[i][j] = std::min({
//                     dp[i - 1][j] + 1,
//                     dp[i][j - 1] + 1,
//                     dp[i - 1][j - 1] + diff // cost for mismatched run lengths
//                 });
//             } else {
//                 dp[i][j] = std::min({
//                     dp[i - 1][j] + 1,
//                     dp[i][j - 1] + 1,
//                     dp[i - 1][j - 1] + 1 // substitution
//                 });
//             }
//         }
//     }

//     return dp[m][n];
// }

// int levenshteinDistance(const std::string &a, const std::string &b) {
//     int m = a.size(), n = b.size();
//     std::vector<std::vector<int>> dp(m+1, std::vector<int>(n+1));

//     for (int i = 0; i <= m; ++i) dp[i][0] = i;
//     for (int j = 0; j <= n; ++j) dp[0][j] = j;

//     for (int i = 1; i <= m; ++i)
//         for (int j = 1; j <= n; ++j)
//             dp[i][j] = std::min({ dp[i-1][j-1] + (a[i-1] != b[j-1]), dp[i-1][j] + 1, dp[i][j-1] + 1 });

//     return dp[m][n];
// }

// class BKNode{
// public:
//     std::string sequence;
//     std::unordered_map<int, std::unique_ptr<BKNode>> children;

//     BKNode(const std::string &seq) : sequence(seq) {}

// };

// class BKTree {
// private:
//     std::unique_ptr<BKNode> root;

// public:
//     void insert(const std::string &seq) {
//         if (!root) {
//             root = std::make_unique<BKNode>(seq);
//             return;
//         }

//         BKNode *current = root.get();
//         while (true) {
//             int dist = levenshteinDistance(seq, current->sequence);
//             if (dist == 0) return; // already exists

//             auto it = current->children.find(dist);
//             if (it == current->children.end()) {
//                 current->children[dist] = std::make_unique<BKNode>(seq);
//                 return;
//             } else {
//                 current = it->second.get();
//             }
//         }
//     }

//     void search(const std::string &query, int tolerance, std::vector<std::string> &results) {
//         searchRecursive(root.get(), query, tolerance, results);
//     }

// private:
//     void searchRecursive(BKNode *node, const std::string &query, int tol, std::vector<std::string> &res) {
//         if (!node) return;

//         int dist = levenshteinDistance(query, node->sequence);
//         if (dist <= tol) res.push_back(node->sequence);

//         for (int d = dist - tol; d <= dist + tol; ++d) {
//             if (node->children.find(d) != node->children.end()) {
//                 searchRecursive(node->children[d].get(), query, tol, res);
//             }
//         }
//     }
// };

////

class BKNode
{
public:
    std::vector<std::shared_ptr<BKNode>> children;
    std::shared_ptr<RLEBWT> seq;

    BKNode(const std::shared_ptr<RLEBWT> &ss)
    {
        seq = ss;
    }
};

class BKTree
{
private:
    std::shared_ptr<BKNode> root;

    int Levenshtein_distance(const std::vector<std::pair<int, char>> &seq1,
                             const std::vector<std::pair<int, char>> &seq2)
    {
        int m = seq1.size(), n = seq2.size();
        std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));

        for (int i = 0; i <= m; i++)
            dp[i][0] = i;
        for (int j = 0; j <= n; j++)
            dp[0][j] = j;

        for (int i = 1; i <= m; i++)
        {
            for (int j = 1; j <= n; j++)
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

public:
    void insert(std::shared_ptr<RLEBWT> new_seq)
    {
        if (!root)
        {
            root = std::make_shared<BKNode>(new_seq);
            return;
        }

        auto current = root;
        while (true)
        {
            int d = Levenshtein_distance(new_seq->get(), current->seq->get());

            bool find_child = false;
            for (auto &child : current->children)
            {
                if (Levenshtein_distance(child->seq->get(), new_seq->get()) == 0)
                {
                    return;
                }
                if (Levenshtein_distance(child->seq->get(), current->seq->get()) == d)
                {
                    current = child;
                    find_child = true;
                    break;
                }
            }

            if (!find_child)
            {
                auto new_Node = std::make_shared<BKNode>(new_seq);
                current->children.push_back(new_Node);
                return;
            }
        }
    }

    void search(const std::vector<std::pair<int, char>> &target_RLE, int threshold,
                std::vector<std::shared_ptr<RLEBWT>> &results)
    {
        if (!root)
            return;

        std::vector<std::shared_ptr<BKNode>> stack = {root};

        while (!stack.empty())
        {
            auto node = stack.back();
            stack.pop_back();

            int d = Levenshtein_distance(node->seq->get(), target_RLE);
            if (d <= threshold)
            {
                results.push_back(node->seq);
            }

            for (auto &child : node->children)
            {
                int child_distance = Levenshtein_distance(child->seq->get(), node->seq->get());
                if (d - threshold <= child_distance && child_distance <= d + threshold)
                {
                    stack.push_back(child);
                }
            }
        }
    }

    void delete_seq(const std::vector<std::pair<int, char>> &target)
    {
        if (!root)
            return;

        if (Levenshtein_distance(root->seq->get(), target) == 0)
        {
            // Deleting root node
            if (root->children.empty())
            {
                root = nullptr;
            }
            else
            {
                auto new_root = root->children.back();
                root->children.pop_back();
                for (auto &child : root->children)
                {
                    insert(child->seq); // Reinsert other children
                }
                root = new_root;
            }
            return;
        }

        delete_helper(root, target);
    }

    bool delete_helper(std::shared_ptr<BKNode> parent, const std::vector<std::pair<int, char>> &target)
    {
        for (auto it = parent->children.begin(); it != parent->children.end(); ++it)
        {
            auto &child = *it;
            if (Levenshtein_distance(child->seq->get(), target) == 0)
            {
                // Found the node to delete
                std::vector<std::shared_ptr<BKNode>> grandchildren = child->children;
                parent->children.erase(it); // Remove child

                for (auto &grandchild : grandchildren)
                {
                    insert(grandchild->seq); // Reinsert grandchildren
                }
                return true;
            }
            else
            {
                if (delete_helper(child, target))
                {
                    return true; // Found and deleted in subtree
                }
            }
        }
        return false;
    }
};
