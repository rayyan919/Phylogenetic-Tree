// #include <vector>
// #include <string>
// #include <algorithm>
// #include <iostream>
// #include <cstdint>
// #include <unordered_set>

// // Sequence Encoding
// #ifndef SEQUENCE_ENCODING_HPP
// #define SEQUENCE_ENCODING_HPP

// #include <string>
// #include <vector>
// #include <unordered_map>

// /**
//  * @class SequenceEncoding
//  * @brief Abstract base class for genome sequence encoding methods
//  */
// class SequenceEncoding
// {
// public:
//     virtual ~SequenceEncoding() = default;

//     /**
//      * @brief Encode a DNA sequence
//      * @param sequence Raw DNA sequence string
//      * @return Encoded representation
//      */
//     virtual std::string encode(const std::string &sequence) = 0;

//     /**
//      * @brief Decode an encoded DNA sequence
//      * @param encoded Encoded DNA sequence
//      * @return Original DNA sequence
//      */
//     virtual std::string decode(const std::string &encoded) = 0;

//     /**
//      * @brief Calculate similarity between two encoded sequences
//      * @param seq1 First encoded sequence
//      * @param seq2 Second encoded sequence
//      * @return Similarity score (0.0-1.0)
//      */
//     virtual double calculateSimilarity(const std::string &seq1, const std::string &seq2) = 0;

//     /**
//      * @brief Extract k-mers from encoded sequence
//      * @param encoded Encoded sequence
//      * @param k Length of k-mers
//      * @return Vector of k-mers
//      */
//     virtual std::vector<std::string> extractKmers(const std::string &encoded, int k) = 0;
// };

// /**
//  * @class BWTEncoding
//  * @brief Burrows-Wheeler Transform implementation for sequence encoding
//  */
// class BWTEncoding : public SequenceEncoding
// {
// private:
//     // Helper methods for BWT
//     std::string createBWT(const std::string &s);
//     std::string inverseBWT(const std::string &bwt);

//     // Distance calculation on BWT strings
//     double levenshteinDistance(const std::string &s1, const std::string &s2);

// public:
//     BWTEncoding() = default;
//     ~BWTEncoding() = default;

//     std::string encode(const std::string &sequence) override;
//     std::string decode(const std::string &encoded) override;
//     double calculateSimilarity(const std::string &seq1, const std::string &seq2);
//     std::vector<std::string> extractKmers(const std::string &encoded, int k);
// };

// /**
//  * @class BitwiseEncoding
//  * @brief Bitwise encoding implementation for sequence encoding
//  */
// class BitwiseEncoding : public SequenceEncoding
// {
// private:
//     // Mapping for DNA nucleotides to bit patterns
//     std::unordered_map<char, unsigned char> nucleotideToBits;
//     std::unordered_map<unsigned char, char> bitsToNucleotide;

//     // Helper methods
//     std::vector<unsigned char> encodeSequence(const std::string &sequence);
//     std::string decodeSequence(const std::vector<unsigned char> &encoded);

// public:
//     BitwiseEncoding();
//     ~BitwiseEncoding() override = default;

//     std::string encode(const std::string &sequence) override;
//     std::string decode(const std::string &encoded) override;
//     double calculateSimilarity(const std::string &seq1, const std::string &seq2) override;
//     std::vector<std::string> extractKmers(const std::string &encoded, int k) override;
// };

// class RLE_BWT
// {
// private:
//     std::string rleBWT;

//     // RLE-BWT transformer to compress gene sequences
//     std::string compRleBWT(const std::string &gene)
//     {
//         std::vector<std::string> rotations;
//         int n = gene.size();
//         for (int i = 1; i < n; i++)
//         {
//             rotations.push_back(gene.substr(n - i - 1) + gene.substr(0, n - i - 1));
//         }

//         std::sort(rotations.begin(), rotations.end());

//         std::string bwt;
//         for (int i = 0; i < n; i++)
//         {
//             bwt += rotations[i][n - 1];
//         }

//         std::string rleBWT;
//         int count = 1;
//         for (int i = 1; i < bwt.size(); i++)
//         {
//             if (bwt[i] == bwt[i - 1])
//             {
//                 count++;
//             }
//             else
//             {
//                 rleBWT += bwt[i - 1] + std::to_string(count);
//                 count = 1;
//             }
//         }

//         return rleBWT;
//     }

//     // Hash function for MinHash computation
//     std::uint64_t hashFunction(const std::string &str, int seed)
//     {
//         std::hash<std::string> hasher;
//         return hasher(str + std::to_string(seed));
//     }

//     std::vector<std::uint64_t> minHash(const std::string &rleBWT, int numHashes)
//     {
//         std::vector<std::uint64_t> minHashes;
//         std::unordered_set<std::string> kmers;
//         int k = 3;

//         for (int i = 0; i < rleBWT.size() - k; i++)
//         {
//             kmers.insert(rleBWT.substr(i, k));
//         }

//         // Needs to be complete////////////////////////
//     }

// public:
//     RLE_BWT(std::string &gene)
//     {
//         rleBWT = compRleBWT(gene);
//     }
// };

// #endif // SEQUENCE_ENCODING_HPP

// #ifndef MINHASH_HPP
// #define MINHASH_HPP

// #include <vector>
// #include <string>
// #include <unordered_set>
// #include <functional>
// // #include "SequenceEncoding.hpp"

// /**
//  * @class MinHash
//  * @brief Implementation of MinHash for approximating Jaccard similarity
//  */
// class MinHash
// {
// private:
//     int numHashFunctions;
//     std::vector<std::pair<uint64_t, uint64_t>> hashParams; // a, b parameters for hash functions

//     // Helper method to generate hash values
//     uint64_t hash(uint64_t x, uint64_t a, uint64_t b, uint64_t prime);

// public:
//     /**
//      * @brief Constructor for MinHash with specified number of hash functions
//      * @param numHashFunctions Number of hash functions to use
//      */
//     explicit MinHash(int numHashFunctions = 100);

//     /**
//      * @brief Generate MinHash signature for a set of k-mers
//      * @param kmers Set of k-mers extracted from a sequence
//      * @return Vector of minimum hash values (signature)
//      */
//     std::vector<uint64_t> generateSignature(const std::vector<std::string> &kmers);

//     /**
//      * @brief Generate MinHash signature directly from a sequence using encoding
//      * @param sequence The DNA sequence
//      * @param encoding The encoding method to use
//      * @param k The k-mer size
//      * @return Vector of minimum hash values (signature)
//      */
//     std::vector<uint64_t> generateSignatureFromSequence(
//         const std::string &sequence,
//         SequenceEncoding &encoding,
//         int k);

//     /**
//      * @brief Calculate Jaccard similarity between two MinHash signatures
//      * @param sig1 First signature
//      * @param sig2 Second signature
//      * @return Estimated Jaccard similarity (0.0-1.0)
//      */
//     double estimateJaccardSimilarity(
//         const std::vector<uint64_t> &sig1,
//         const std::vector<uint64_t> &sig2);

//     /**
//      * @brief Calculate Hamming distance between two MinHash signatures
//      * @param sig1 First signature
//      * @param sig2 Second signature
//      * @return Hamming distance
//      */
//     int hammingDistance(
//         const std::vector<uint64_t> &sig1,
//         const std::vector<uint64_t> &sig2);

//     /**
//      * @brief Compute mean MinHash signature from multiple signatures
//      * @param signatures Vector of MinHash signatures
//      * @return Mean signature
//      */
//     std::vector<uint64_t> computeMeanSignature(
//         const std::vector<std::vector<uint64_t>> &signatures);
// };

// #endif // MINHASH_HPP

// #ifndef SPECIES_HPP
// #define SPECIES_HPP

// #include <string>
// #include <vector>
// // #include "SequenceEncoding.hpp"
// // #include "MinHash.hpp"

// /**
//  * @class Species
//  * @brief Represents a species with its genomic information
//  */
// class Species
// {
// private:
//     std::string id;
//     std::string name;
//     std::string rawSequence;
//     std::string encodedSequence;
//     std::vector<uint64_t> minHashSignature;

// public:
//     /**
//      * @brief Constructor for Species
//      * @param id Unique identifier
//      * @param name Scientific name
//      * @param sequence Genomic sequence
//      */
//     Species(const std::string &id, const std::string &name, const std::string &sequence);

//     /**
//      * @brief Calculate encoded sequence using provided encoding method
//      * @param encoding The encoding method to use
//      */
//     void encodeSequence(SequenceEncoding &encoding);

//     /**
//      * @brief Generate MinHash signature for this species
//      * @param minHash MinHash object to use
//      * @param encoding Encoding method for k-mer extraction
//      * @param k K-mer size
//      */
//     void generateSignature(MinHash &minHash, SequenceEncoding &encoding, int k);

//     /**
//      * @brief Apply mutation to the species based on Markov model
//      * @param mutationRate Probability of mutation
//      * @param encoding Encoding method to update encoded sequence
//      * @param minHash MinHash object to update signature
//      * @param k K-mer size for signature update
//      * @return True if mutation occurred and signature changed
//      */
//     bool mutate(double mutationRate, SequenceEncoding &encoding, MinHash &minHash, int k);

//     // Getters
//     const std::string &getId() const { return id; }
//     const std::string &getName() const { return name; }
//     const std::string &getRawSequence() const { return rawSequence; }
//     const std::string &getEncodedSequence() const { return encodedSequence; }
//     const std::vector<uint64_t> &getMinHashSignature() const { return minHashSignature; }
// };

// #endif // SPECIES_HPP

// #ifndef BKTREE_HPP
// #define BKTREE_HPP

// #include <unordered_map>
// #include <vector>
// #include <memory>
// #include <functional>
// // #include "Species.hpp"
// // #include "MinHash.hpp"
// // #include "SequenceEncoding.hpp"

// /**
//  * @class BKTreeNode
//  * @brief Node in the BK-Tree
//  */
// class BKTreeNode
// {
// private:
//     std::shared_ptr<Species> species;
//     std::unordered_map<int, std::shared_ptr<BKTreeNode>> children;

// public:
//     /**
//      * @brief Constructor for BKTreeNode
//      * @param species The species stored in this node
//      */
//     explicit BKTreeNode(std::shared_ptr<Species> species);

//     // Getters and setters
//     std::shared_ptr<Species> getSpecies() const { return species; }
//     const std::unordered_map<int, std::shared_ptr<BKTreeNode>> &getChildren() const { return children; }

//     /**
//      * @brief Add a child node at specified distance
//      * @param distance Edit distance between species
//      * @param node Child node to add
//      */
//     void addChild(int distance, std::shared_ptr<BKTreeNode> node);

//     /**
//      * @brief Remove a child node at specified distance
//      * @param distance Edit distance between species
//      * @return True if child was removed
//      */
//     bool removeChild(int distance);

//     /**
//      * @brief Check if node has a child at specified distance
//      * @param distance Edit distance
//      * @return True if child exists
//      */
//     bool hasChild(int distance) const;

//     /**
//      * @brief Get child node at specified distance
//      * @param distance Edit distance
//      * @return Pointer to child node
//      */
//     std::shared_ptr<BKTreeNode> getChild(int distance) const;
// };

// /**
//  * @class BKTree
//  * @brief Burkhard-Keller Tree for species organization within clusters
//  */
// class BKTree
// {
// private:
//     std::shared_ptr<BKTreeNode> root;
//     std::function<int(const Species &, const Species &)> distanceFunction;
//     std::shared_ptr<SequenceEncoding> encoding;
//     std::shared_ptr<MinHash> minHash;

//     // Helper methods
//     void searchWithinDistanceRecursive(
//         std::shared_ptr<BKTreeNode> node,
//         const Species &querySpecies,
//         int maxDistance,
//         std::vector<std::pair<std::shared_ptr<Species>, int>> &results);

//     /**
//      * @brief Compute distance buckets for a species
//      * @param species The species to calculate buckets for
//      * @return Map of distance ranges to expected distances
//      */
//     std::unordered_map<int, int> computeDistanceBuckets(const Species &species);

// public:
//     /**
//      * @brief Constructor for BKTree
//      * @param encoding Sequence encoding method
//      * @param minHash MinHash implementation
//      */
//     BKTree(std::shared_ptr<SequenceEncoding> encoding, std::shared_ptr<MinHash> minHash);

//     /**
//      * @brief Insert a species into the BK-Tree
//      * @param species The species to insert
//      * @return True if insertion was successful
//      */
//     bool insert(std::shared_ptr<Species> species);

//     /**
//      * @brief Insert with precomputed distance buckets
//      * @param species The species to insert
//      * @param distanceBuckets Precomputed distances
//      * @return True if insertion was successful
//      */
//     bool insertWithBuckets(
//         std::shared_ptr<Species> species,
//         const std::unordered_map<int, int> &distanceBuckets);

//     /**
//      * @brief Remove a species from the BK-Tree
//      * @param speciesId ID of species to remove
//      * @return True if removal was successful
//      */
//     bool remove(const std::string &speciesId);

//     /**
//      * @brief Search for species within specified distance
//      * @param querySpecies The query species
//      * @param maxDistance Maximum edit distance
//      * @return Vector of species and their distances
//      */
//     std::vector<std::pair<std::shared_ptr<Species>, int>> searchWithinDistance(
//         const Species &querySpecies,
//         int maxDistance);

//     /**
//      * @brief Find the nearest species to the query
//      * @param querySpecies The query species
//      * @param k Number of nearest neighbors to find
//      * @return Vector of species and their distances
//      */
//     std::vector<std::pair<std::shared_ptr<Species>, int>> findNearestNeighbors(
//         const Species &querySpecies,
//         int k);

//     /**
//      * @brief Get all species in the tree
//      * @return Vector of all species
//      */
//     std::vector<std::shared_ptr<Species>> getAllSpecies();

//     /**
//      * @brief Get the root node
//      * @return Root node pointer
//      */
//     std::shared_ptr<BKTreeNode> getRoot() const { return root; }

//     /**
//      * @brief Get the count of species in the tree
//      * @return Number of species
//      */
//     size_t size() const;

//     /**
//      * @brief Check if the tree is empty
//      * @return True if empty
//      */
//     bool empty() const { return root == nullptr; }
// };

// #endif // BKTREE_HPP

// #ifndef OCTREE_NODE_HPP
// #define OCTREE_NODE_HPP

// #include <array>
// #include <memory>
// #include <vector>
// // #include "BKTree.hpp"
// // #include "MinHash.hpp"
// // #include "Species.hpp"

// /**
//  * @class OctreeNode
//  * @brief Node in the Octree structure for hierarchical clustering
//  */
// class OctreeNode
// {
// private:
//     // Octree has 8 children per node
//     std::array<std::shared_ptr<OctreeNode>, 8> children;

//     // MinHash signature representing mean of all species in this node
//     std::vector<uint64_t> meanSignature;

//     // BK-Tree for species within this node's cluster
//     std::shared_ptr<BKTree> bkTree;

//     // Depth of this node in the Octree
//     int depth;

//     // Split threshold - when to divide this node
//     int splitThreshold;

//     // Parent node
//     std::weak_ptr<OctreeNode> parent;

//     // Flag indicating if this is a leaf node
//     bool leaf;

//     // Octant index in parent's children array (0-7)
//     int octantIndex;

// public:
//     /**
//      * @brief Constructor for OctreeNode
//      * @param encoding Sequence encoding method
//      * @param minHash MinHash implementation
//      * @param depth Depth in the Octree
//      * @param splitThreshold Threshold for node splitting
//      * @param parent Parent node
//      * @param octantIndex Index in parent's children array
//      */
//     OctreeNode(
//         std::shared_ptr<SequenceEncoding> encoding,
//         std::shared_ptr<MinHash> minHash,
//         int depth = 0,
//         int splitThreshold = 20,
//         std::weak_ptr<OctreeNode> parent = std::weak_ptr<OctreeNode>(),
//         int octantIndex = -1);

//     /**
//      * @brief Get the BK-Tree for this node
//      * @return BK-Tree pointer
//      */
//     std::shared_ptr<BKTree> getBKTree() const { return bkTree; }

//     /**
//      * @brief Get the mean MinHash signature
//      * @return Vector of hash values
//      */
//     const std::vector<uint64_t> &getMeanSignature() const { return meanSignature; }

//     /**
//      * @brief Update mean signature based on species in BK-Tree
//      * @param minHash MinHash implementation
//      */
//     void updateMeanSignature(std::shared_ptr<MinHash> minHash);

//     /**
//      * @brief Check if this is a leaf node
//      * @return True if leaf
//      */
//     bool isLeaf() const { return leaf; }

//     /**
//      * @brief Get child at specified index
//      * @param index Child index (0-7)
//      * @return Child node pointer
//      */
//     std::shared_ptr<OctreeNode> getChild(int index) const;

//     /**
//      * @brief Get all children nodes
//      * @return Array of child nodes
//      */
//     const std::array<std::shared_ptr<OctreeNode>, 8> &getChildren() const { return children; }

//     /**
//      * @brief Get the depth of this node
//      * @return Depth value
//      */
//     int getDepth() const { return depth; }

//     /**
//      * @brief Get the octant index in parent's array
//      * @return Octant index
//      */
//     int getOctantIndex() const { return octantIndex; }

//     /**
//      * @brief Get the parent node
//      * @return Parent node pointer
//      */
//     std::shared_ptr<OctreeNode> getParent() const { return parent.lock(); }

//     /**
//      * @brief Insert a species into this node
//      * @param species Species to insert
//      * @param minHash MinHash implementation
//      * @return True if insertion was successful
//      */
//     bool insertSpecies(std::shared_ptr<Species> species, std::shared_ptr<MinHash> minHash);

//     /**
//      * @brief Remove a species from this node
//      * @param speciesId ID of species to remove
//      * @param minHash MinHash implementation
//      * @return True if removal was successful
//      */
//     bool removeSpecies(const std::string &speciesId, std::shared_ptr<MinHash> minHash);

//     /**
//      * @brief Split this node into children
//      * @param minHash MinHash implementation
//      * @return True if split was successful
//      */
//     bool split(std::shared_ptr<MinHash> minHash);

//     /**
//      * @brief Merge child nodes into this node
//      * @param minHash MinHash implementation
//      * @return True if merge was successful
//      */
//     bool merge(std::shared_ptr<MinHash> minHash);

//     /**
//      * @brief Determine the appropriate child for a species
//      * @param species The species to place
//      * @param minHash MinHash implementation
//      * @return Index of appropriate child (0-7)
//      */
//     int determineChildIndex(const Species &species, std::shared_ptr<MinHash> minHash);

//     /**
//      * @brief Get number of species in this node's BK-Tree
//      * @return Species count
//      */
//     size_t getSpeciesCount() const;

//     /**
//      * @brief Get all species in this node's BK-Tree
//      * @return Vector of species
//      */
//     std::vector<std::shared_ptr<Species>> getAllSpecies() const;
// };

// #endif // OCTREE_NODE_HPP

// #ifndef HYBRID_BK_OCTREE_HPP
// #define HYBRID_BK_OCTREE_HPP

// #include <memory>
// #include <vector>
// #include <string>
// // #include "OctreeNode.hpp"
// // #include "MinHash.hpp"
// // #include "Species.hpp"
// // #include "SequenceEncoding.hpp"

// /**
//  * @class HybridBKOctree
//  * @brief Main class for the hybrid BK-Octree structure
//  */
// class HybridBKOctree
// {
// private:
//     std::shared_ptr<OctreeNode> root;
//     std::shared_ptr<SequenceEncoding> encoding;
//     std::shared_ptr<MinHash> minHash;
//     int kmerSize;
//     int maxDepth;
//     int splitThreshold;

//     // Helper methods
//     std::shared_ptr<OctreeNode> findBestNodeForSpecies(
//         const Species &species,
//         std::shared_ptr<OctreeNode> startNode = nullptr);

// public:
//     /**
//      * @brief Constructor for HybridBKOctree
//      * @param encoding Sequence encoding method
//      * @param minHash MinHash implementation
//      * @param kmerSize K-mer size for MinHash
//      * @param maxDepth Maximum depth of Octree
//      * @param splitThreshold Node split threshold
//      */
//     HybridBKOctree(
//         std::shared_ptr<SequenceEncoding> encoding,
//         std::shared_ptr<MinHash> minHash,
//         int kmerSize = 10,
//         int maxDepth = 8,
//         int splitThreshold = 20);

//     /**
//      * @brief Insert a species into the hybrid structure
//      * @param species Species to insert
//      * @return True if insertion was successful
//      */
//     bool insertSpecies(std::shared_ptr<Species> species);

//     /**
//      * @brief Remove a species from the structure
//      * @param speciesId ID of species to remove
//      * @return True if removal was successful
//      */
//     bool removeSpecies(const std::string &speciesId);

//     /**
//      * @brief Find species similar to query species
//      * @param querySpecies Query species
//      * @param maxDistance Maximum edit distance
//      * @param maxResults Maximum number of results
//      * @return Vector of similar species
//      */
//     std::vector<std::pair<std::shared_ptr<Species>, double>> findSimilarSpecies(
//         const Species &querySpecies,
//         double maxDistance = 0.2,
//         int maxResults = 10);

//     /**
//      * @brief Apply mutation to all species according to Markov model
//      * @param mutationRate Probability of mutation
//      * @return Number of species that mutated
//      */
//     int simulateMutation(double mutationRate);

//     /**
//      * @brief Get root node of the Octree
//      * @return Root node pointer
//      */
//     std::shared_ptr<OctreeNode> getRoot() const { return root; }

//     /**
//      * @brief Get all species in the structure
//      * @return Vector of all species
//      */
//     std::vector<std::shared_ptr<Species>> getAllSpecies() const;

//     /**
//      * @brief Get total number of species in the structure
//      * @return Species count
//      */
//     size_t size() const;

//     /**
//      * @brief Balance the tree structure
//      * @return True if balancing was successful
//      */
//     bool balance();

//     /**
//      * @brief Get the sequence encoding method
//      * @return Encoding method pointer
//      */
//     std::shared_ptr<SequenceEncoding> getEncoding() const { return encoding; }

//     /**
//      * @brief Get the MinHash implementation
//      * @return MinHash implementation pointer
//      */
//     std::shared_ptr<MinHash> getMinHash() const { return minHash; }
// };

// #endif // HYBRID_BK_OCTREE_HPP

// #ifndef MARKOV_MODEL_HPP
// #define MARKOV_MODEL_HPP

// #include <vector>
// #include <unordered_map>
// #include <string>
// #include <random>
// // #include "Species.hpp"

// /**
//  * @class MarkovModel
//  * @brief Implements a Markov Chain model for DNA sequence mutations
//  */
// class MarkovModel
// {
// private:
//     // Transition probabilities for nucleotides
//     std::unordered_map<char, std::unordered_map<char, double>> transitionMatrix;

//     // Random number generator
//     std::mt19937 rng;

//     // Base mutation rate
//     double baseMutationRate;

// public:
//     /**
//      * @brief Constructor for MarkovModel
//      * @param baseMutationRate Base mutation probability
//      * @param seed Random seed
//      */
//     explicit MarkovModel(double baseMutationRate = 0.001, unsigned int seed = std::random_device{}());

//     /**
//      * @brief Set custom transition probabilities
//      * @param matrix Transition probability matrix
//      */
//     void setTransitionMatrix(
//         const std::unordered_map<char, std::unordered_map<char, double>> &matrix);

//     /**
//      * @brief Get transition probability from one nucleotide to another
//      * @param from Source nucleotide
//      * @param to Target nucleotide
//      * @return Transition probability
//      */
//     double getTransitionProbability(char from, char to) const;

//     /**
//      * @brief Apply mutations to a DNA sequence
//      * @param sequence Original DNA sequence
//      * @param mutationRate Custom mutation rate (or use base rate if 0)
//      * @return Mutated sequence
//      */
//     std::string mutateSequence(
//         const std::string &sequence,
//         double mutationRate = 0.0);

//     /**
//      * @brief Apply mutations to a species
//      * @param species Species to mutate
//      * @param mutationRate Custom mutation rate (or use base rate if 0)
//      * @return True if mutation occurred
//      */
//     bool mutateSpecies(Species &species, double mutationRate = 0.0);

//     /**
//      * @brief Simulate genetic drift for multiple species
//      * @param species Vector of species to evolve
//      * @param generations Number of generations to simulate
//      * @param mutationRate Custom mutation rate (or use base rate if 0)
//      * @return Vector of mutation counts per generation
//      */
//     std::vector<int> simulateGeneticDrift(
//         std::vector<Species> &species,
//         int generations,
//         double mutationRate = 0.0);

//     /**
//      * @brief Get the base mutation rate
//      * @return Base mutation rate
//      */
//     double getBaseMutationRate() const { return baseMutationRate; }

//     /**
//      * @brief Set the base mutation rate
//      * @param rate New base mutation rate
//      */
//     void setBaseMutationRate(double rate) { baseMutationRate = rate; }
// };

// #endif // MARKOV_MODEL_HPP

// #ifndef PHYLOGENETIC_VISUALIZER_HPP
// #define PHYLOGENETIC_VISUALIZER_HPP

// #include <string>
// #include <vector>
// #include <memory>
// // #include "HybridBKOctree.hpp"
// // #include "Species.hpp"

// /**
//  * @class PhylogeneticVisualizer
//  * @brief Interface for visualizing the phylogenetic tree
//  */
// class PhylogeneticVisualizer
// {
// public:
//     virtual ~PhylogeneticVisualizer() = default;

//     /**
//      * @brief Initialize the visualizer
//      * @return True if initialization was successful
//      */
//     virtual bool initialize() = 0;

//     /**
//      * @brief Update the visualization with current tree state
//      * @param tree The hybrid BK-Octree
//      * @return True if update was successful
//      */
//     virtual bool update(const HybridBKOctree &tree) = 0;

//     /**
//      * @brief Save the current visualization to a file
//      * @param filename Output filename
//      * @return True if save was successful
//      */
//     virtual bool saveToFile(const std::string &filename) = 0;

//     /**
//      * @brief Display the visualization
//      * @return True if display was successful
//      */
//     virtual bool display() = 0;

//     /**
//      * @brief Highlight a species in the visualization
//      * @param speciesId ID of species to highlight
//      * @return True if highlighting was successful
//      */
//     virtual bool highlightSpecies(const std::string &speciesId) = 0;

//     /**
//      * @brief Track the evolution of a species over time
//      * @param speciesId ID of species to track
//      * @param generations Number of generations to track
//      * @return True if tracking was successful
//      */
//     virtual bool trackSpeciesEvolution(
//         const std::string &speciesId,
//         int generations) = 0;
// };

// /**
//  * @class PyQtGraphVisualizer
//  * @brief PyQtGraph implementation of the phylogenetic visualizer
//  */
// class PyQtGraphVisualizer : public PhylogeneticVisualizer
// {
// private:
//     // Implementation details for PyQtGraph
//     std::string pythonScript;
//     std::vector<std::string> treeHistory;
//     std::string tempDataFile;

//     /**
//      * @brief Convert tree to JSON format for Python
//      * @param tree The hybrid BK-Octree
//      * @return JSON string representation
//      */
//     std::string treeToJSON(const HybridBKOctree &tree);

//     /**
//      * @brief Execute Python script for visualization
//      * @param command Command to execute
//      * @return True if execution was successful
//      */
//     bool executePythonScript(const std::string &command);

// public:
//     /**
//      * @brief Constructor for PyQtGraphVisualizer
//      * @param pythonScriptPath Path to Python visualization script
//      */
//     explicit PyQtGraphVisualizer(const std::string &pythonScriptPath = "visualize.py");

//     /**
//      * @brief Destructor for PyQtGraphVisualizer
//      */
//     ~PyQtGraphVisualizer() override;

//     bool initialize() override;
//     bool update(const HybridBKOctree &tree) override;
//     bool saveToFile(const std::string &filename) override;
//     bool display() override;
//     bool highlightSpecies(const std::string &speciesId) override;
//     bool trackSpeciesEvolution(const std::string &speciesId, int generations) override;
// };

// #endif // PHYLOGENETIC_VISUALIZER_HPP

// #ifndef PHYLOGENETIC_SYSTEM_HPP
// #define PHYLOGENETIC_SYSTEM_HPP

// #include <memory>
// #include <string>
// #include <vector>
// #include <unordered_map>
// #include <functional>
// // #include "HybridBKOctree.hpp"
// // #include "MarkovModel.hpp"
// // #include "PhylogeneticVisualizer.hpp"
// // #include "Species.hpp"
// // #include "SequenceEncoding.hpp"
// // #include "MinHash.hpp"

// /**
//  * @class PhylogeneticSystem
//  * @brief Main system class that coordinates all components
//  */
// class PhylogeneticSystem
// {
// private:
//     // Core components
//     std::shared_ptr<HybridBKOctree> tree;
//     std::shared_ptr<MarkovModel> markovModel;
//     std::shared_ptr<PhylogeneticVisualizer> visualizer;
//     std::shared_ptr<SequenceEncoding> encoding;
//     std::shared_ptr<MinHash> minHash;

//     // Parameters
//     int kmerSize;
//     int maxOctreeDepth;
//     int splitThreshold;
//     double baseMutationRate;

//     // Data tracking
//     int currentGeneration;
//     std::unordered_map<std::string, std::vector<std::shared_ptr<Species>>> evolutionHistory;

//     // Callback for mutation events
//     std::function<void(const std::string &, const Species &)> mutationCallback;

// public:
//     /**
//      * @brief Constructor for PhylogeneticSystem
//      * @param encodingType Type of sequence encoding to use ("BWT" or "Bitwise")
//      * @param numHashFunctions Number of hash functions for MinHash
//      * @param kmerSize K-mer size for sequence comparison
//      * @param maxOctreeDepth Maximum depth of the Octree
//      * @param splitThreshold Threshold for node splitting
//      * @param baseMutationRate Base rate for genetic mutations
//      */
//     PhylogeneticSystem(
//         const std::string &encodingType = "BWT",
//         int numHashFunctions = 100,
//         int kmerSize = 10,
//         int maxOctreeDepth = 8,
//         int splitThreshold = 20,
//         double baseMutationRate = 0.001);

//     /**
//      * @brief Initialize the system
//      * @param visualizerType Type of visualizer to use
//      * @return True if initialization was successful
//      */
//     bool initialize(const std::string &visualizerType = "PyQtGraph");

//     /**
//      * @brief Load species data from a CSV file
//      * @param filename Path to CSV file
//      * @return Number of species loaded
//      */
//     int loadSpeciesFromCSV(const std::string &filename);

//     /**
//      * @brief Save current system state to a file
//      * @param filename Output filename
//      * @return True if save was successful
//      */
//     bool saveToFile(const std::string &filename);

//     /**
//      * @brief Load system state from a file
//      * @param filename Input filename
//      * @return True if load was successful
//      */
//     bool loadFromFile(const std::string &filename);

//     /**
//      * @brief Add a new species to the system
//      * @param id Species ID
//      * @param name Species name
//      * @param sequence Genomic sequence
//      * @return True if addition was successful
//      */
//     bool addSpecies(
//         const std::string &id,
//         const std::string &name,
//         const std::string &sequence);

//     /**
//      * @brief Remove a species from the system
//      * @param speciesId ID of species to remove
//      * @return True if removal was successful
//      */
//     bool removeSpecies(const std::string &speciesId);

//     /**
//      * @brief Find species similar to a query species
//      * @param querySpeciesId ID of query species
//      * @param maxDistance Maximum distance for similarity
//      * @param maxResults Maximum number of results
//      * @return Vector of similar species with similarity scores
//      */
//     std::vector<std::pair<std::shared_ptr<Species>, double>> findSimilarSpecies(
//         const std::string &querySpeciesId,
//         double maxDistance = 0.2,
//         int maxResults = 10);

//     /**
//      * @brief Simulate evolution for a specified number of generations
//      * @param generations Number of generations to simulate
//      * @param mutationRate Custom mutation rate (or use base rate if 0)
//      * @param trackHistory Whether to track evolutionary history
//      * @return Number of mutations that occurred
//      */
//     int simulateEvolution(
//         int generations,
//         double mutationRate = 0.0,
//         bool trackHistory = true);

//     /**
//      * @brief Get the evolution history for a species
//      * @param speciesId ID of species
//      * @return Vector of species states across generations
//      */
//     std::vector<std::shared_ptr<Species>> getEvolutionHistory(const std::string &speciesId);

//     /**
//      * @brief Visualize the current phylogenetic tree
//      * @param highlightSpeciesId ID of species to highlight (empty for none)
//      * @return True if visualization was successful
//      */
//     bool visualizeTree(const std::string &highlightSpeciesId = "");

//     /**
//      * @brief Visualize the evolution of a species
//      * @param speciesId ID of species to track
//      * @param generations Number of generations to display (0 for all)
//      * @return True if visualization was successful
//      */
//     bool visualizeEvolution(
//         const std::string &speciesId,
//         int generations = 0);

//     /**
//      * @brief Set callback function for mutation events
//      * @param callback Function to call when a mutation occurs
//      */
//     void setMutationCallback(
//         std::function<void(const std::string &, const Species &)> callback);

//     /**
//      * @brief Get all species in the system
//      * @return Vector of all species
//      */
//     std::vector<std::shared_ptr<Species>> getAllSpecies() const;

//     /**
//      * @brief Get the current generation number
//      * @return Generation number
//      */
//     int getCurrentGeneration() const { return currentGeneration; }

//     /**
//      * @brief Get the tree structure
//      * @return Hybrid BK-Octree pointer
//      */
//     std::shared_ptr<HybridBKOctree> getTree() const { return tree; }

//     /**
//      * @brief Get the Markov model
//      * @return Markov model pointer
//      */
//     std::shared_ptr<MarkovModel> getMarkovModel() const { return markovModel; }
// };

// #endif // PHYLOGENETIC_SYSTEM_HPP

// #ifndef DATA_PARSER_HPP
// #define DATA_PARSER_HPP

// #include <string>
// #include <vector>
// #include <memory>
// // #include "Species.hpp"

// /**
//  * @class DataParser
//  * @brief Abstract base class for parsing genomic data
//  */
// class DataParser
// {
// public:
//     virtual ~DataParser() = default;

//     /**
//      * @brief Parse data from a file
//      * @param filename Input filename
//      * @return Vector of parsed species
//      */
//     virtual std::vector<std::shared_ptr<Species>> parseFile(const std::string &filename) = 0;

//     /**
//      * @brief Write species data to a file
//      * @param filename Output filename
//      * @param species Vector of species to write
//      * @return True if write was successful
//      */
//     virtual bool writeFile(
//         const std::string &filename,
//         const std::vector<std::shared_ptr<Species>> &species) = 0;
// };

// /**
//  * @class CSVParser
//  * @brief Implementation of DataParser for CSV files
//  */
// class CSVParser : public DataParser
// {
// private:
//     char delimiter;
//     bool hasHeader;

//     /**
//      * @brief Parse a single line of CSV data
//      * @param line CSV line
//      * @return Species pointer if parsing was successful, nullptr otherwise
//      */
//     std::shared_ptr<Species> parseLine(const std::string &line);

//     /**
//      * @brief Split a string by delimiter
//      * @param s String to split
//      * @return Vector of tokens
//      */
//     std::vector<std::string> split(const std::string &s);

// public:
//     /**
//      * @brief Constructor for CSVParser
//      * @param delimiter CSV delimiter character
//      * @param hasHeader Whether file has a header row
//      */
//     explicit CSVParser(char delimiter = ',', bool hasHeader = true);

//     std::vector<std::shared_ptr<Species>> parseFile(const std::string &filename) override;

//     bool writeFile(
//         const std::string &filename,
//         const std::vector<std::shared_ptr<Species>> &species) override;
// };

// /**
//  * @class FASTAParser
//  * @brief Implementation of DataParser for FASTA files
//  */
// class FASTAParser : public DataParser
// {
// private:
//     /**
//      * @brief Parse a FASTA sequence
//      * @param header FASTA header line
//      * @param sequence Sequence lines
//      * @return Species pointer
//      */
//     std::shared_ptr<Species> parseSequence(
//         const std::string &header,
//         const std::string &sequence);

// public:
//     FASTAParser() = default;

//     std::vector<std::shared_ptr<Species>> parseFile(const std::string &filename) override;

//     bool writeFile(
//         const std::string &filename,
//         const std::vector<std::shared_ptr<Species>> &species) override;
// };

// #endif // DATA_PARSER_HPP
