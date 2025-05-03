#ifndef GENETIC_SIMHASH_H
#define GENETIC_SIMHASH_H

#include "simhash.h" // The provided simhash library header.
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <algorithm> // for std::min
#include "BitEncodedSeq.h"

namespace GeneticSimhash
{

    uint64_t custom_nucleotide_hash(const BitEncodedSeq &seq,
                                    std::uint32_t start, std::uint32_t length);

    Simhash::hash_t computeSimhash(const BitEncodedSeq &seq, std::size_t numSegments = 32);

}

#endif // GENETIC_SIMHASH_H

// namespace claude
// {
//     std::uint64_t
//     custom_kmer_hash(const BitEncodedSeq &seq, std::uint32_t start, std::uint32_t length);
//     std::uint64_t mix_hash(std::uint64_t hash);
//     std::tuple<double, double, double> dna_simhash_to_coordinates(const BitEncodedSeq &seq, size_t k, size_t step);
//     double dna_spatial_distance(const BitEncodedSeq &seq1, const BitEncodedSeq &seq2);
//     /**
//      * @brief Computes a SimHash for a DNA sequence and translates it to 3D coordinates
//      *
//      * This function takes a BitEncodedSeq and calculates a SimHash based on overlapping
//      * k-mers from the sequence. The SimHash is then mapped to x,y,z coordinates in a way
//      * that preserves locality - similar sequences will be close in 3D space.
//      *
//      * @param seq The BitEncodedSeq DNA sequence to hash
//      * @param k The k-mer size to use (default: 6)
//      * @param step The sliding window step size (default: 1)
//      * @return std::tuple<double, double, double> The x,y,z coordinates
//      */
//     std::tuple<double, double, double> dna_simhash_to_coordinates(
//         const BitEncodedSeq &seq,
//         size_t k = 6,
//         size_t step = 1)
//     {
//         // Check if sequence is too short for k-mers
//         if (seq.size() < k)
//         {
//             throw std::invalid_argument("Sequence length must be >= k");
//         }

//         // 1. Generate k-mer hashes
//         std::vector<Simhash::hash_t> kmer_hashes;
//         for (std::uint32_t i = 0; i <= seq.size() - k; i += step)
//         {
//             // Hash each k-mer
//             Simhash::hash_t hash = custom_kmer_hash(seq, i, k);
//             kmer_hashes.push_back(hash);
//         }

//         // 2. Compute the SimHash from all k-mer hashes
//         Simhash::hash_t simhash = Simhash::compute(kmer_hashes);

//         // 3. Split the 64-bit simhash into three parts for x, y, z coordinates
//         // We'll use a bit interleaving approach for better spatial locality

//         // Extract bits using masks for each dimension
//         std::uint64_t x_bits = 0, y_bits = 0, z_bits = 0;

//         for (size_t i = 0; i < 21; ++i)
//         { // Extract 21 bits for each dimension (63 bits total)
//             // Get bits at positions 3i, 3i+1, 3i+2
//             x_bits |= ((simhash >> (3 * i)) & 1ULL) << i;
//             y_bits |= ((simhash >> (3 * i + 1)) & 1ULL) << i;
//             z_bits |= ((simhash >> (3 * i + 2)) & 1ULL) << i;
//         }

//         // 4. Convert to floating point and normalize to a reasonable range
//         // We'll use signed coordinates in [-1.0, 1.0] range

//         // Maximum value for 21 bits
//         const double max_val = (1ULL << 21) - 1;

//         // Convert to range [-1.0, 1.0]
//         double x = (static_cast<double>(x_bits) / max_val) * 2.0 - 1.0;
//         double y = (static_cast<double>(y_bits) / max_val) * 2.0 - 1.0;
//         double z = (static_cast<double>(z_bits) / max_val) * 2.0 - 1.0;

//         return std::make_tuple(x, y, z);
//     }

//     /**
//      * @brief Improved k-mer hash function for DNA sequences
//      *
//      * This function generates a hash for a k-mer in a BitEncodedSeq.
//      * It's designed to ensure that similar k-mers produce similar hashes,
//      * which is crucial for the SimHash algorithm to work effectively.
//      *
//      * @param seq The BitEncodedSeq containing the DNA sequence
//      * @param start Starting position of the k-mer
//      * @param length Length of the k-mer (k)
//      * @return std::uint64_t Hash value for the k-mer
//      */
//     std::uint64_t custom_kmer_hash(const BitEncodedSeq &seq,
//                                    std::uint32_t start,
//                                    std::uint32_t length)
//     {
//         // We'll use a rolling hash function that's sensitive to the position
//         // of nucleotides (similar to the one you provided but with improvements)
//         std::uint64_t hash = 0;

//         for (std::uint32_t i = 0; i < length; ++i)
//         {
//             char nucleotide = seq.at(start + i);
//             std::uint64_t val = 0;

//             // Map nucleotides to values that maintain Hamming distance properties
//             switch (nucleotide)
//             {
//             case 'A':
//                 val = 0;
//                 break; // 00
//             case 'C':
//                 val = 1;
//                 break; // 01
//             case 'T':
//                 val = 2;
//                 break; // 10
//             case 'G':
//                 val = 3;
//                 break; // 11
//             default:
//                 val = 0;
//                 break; // Shouldn't happen with valid DNA
//             }

//             // Weight by position - multiply by prime number (5) for each position
//             // This ensures position matters while keeping similar sequences close
//             hash = hash * 5 + val;
//         }

//         // Apply a bit-mixing function that preserves locality
//         // This helps spread the values while maintaining similarity distances
//         return mix_hash(hash);
//     }

//     /**
//      * @brief Mixes bits in a hash while preserving locality
//      *
//      * This function applies transformations to distribute the hash bits
//      * while ensuring that similar inputs yield similar outputs.
//      *
//      * @param hash Input hash value
//      * @return std::uint64_t Mixed hash value
//      */
//     std::uint64_t mix_hash(std::uint64_t hash)
//     {
//         // Apply a mixing function that preserves locality
//         // XOR with shifted versions helps spread bits while maintaining Hamming distance
//         hash ^= hash >> 33;
//         hash *= 0xff51afd7ed558ccdULL;
//         hash ^= hash >> 33;
//         hash *= 0xc4ceb9fe1a85ec53ULL;
//         hash ^= hash >> 33;

//         return hash;
//     }

//     /**
//      * @brief Calculate Euclidean distance between DNA sequences in 3D space
//      *
//      * Utility function to test similarity between two DNA sequences by
//      * calculating the distance between their 3D coordinates.
//      *
//      * @param seq1 First DNA sequence
//      * @param seq2 Second DNA sequence
//      * @return double Euclidean distance in 3D space
//      */
//     double dna_spatial_distance(const BitEncodedSeq &seq1, const BitEncodedSeq &seq2)
//     {
//         // Get 3D coordinates for both sequences
//         auto [x1, y1, z1] = dna_simhash_to_coordinates(seq1);
//         auto [x2, y2, z2] = dna_simhash_to_coordinates(seq2);

//         // Calculate Euclidean distance
//         double dx = x2 - x1;
//         double dy = y2 - y1;
//         double dz = z2 - z1;

//         return std::sqrt(dx * dx + dy * dy + dz * dz);
//     }

//     /**
//      * Example usage:
//      *
//      * BitEncodedSeq seq1("ACGTACGTACGT");
//      * BitEncodedSeq seq2("ACGTACGTACGA"); // One base different
//      *
//      * auto [x1, y1, z1] = dna_simhash_to_coordinates(seq1);
//      * std::cout << "Coordinates for seq1: (" << x1 << ", " << y1 << ", " << z1 << ")\n";
//      *
//      * double distance = dna_spatial_distance(seq1, seq2);
//      * std::cout << "Spatial distance: " << distance << "\n";
//      */
// }

// namespace deepseek
// {
//     struct Point
//     {
//         double x;
//         double y;
//         double z;
//     };
//     // Generate a deterministic 3D vector for each bit in the SimHash
//     std::array<std::tuple<double, double, double>, 64> generate_bit_vectors()
//     {
//         std::array<std::tuple<double, double, double>, 64> vectors;
//         for (int i = 0; i < 64; ++i)
//         {
//             // Use a simple deterministic method to generate vector components
//             double theta = 2.0 * M_PI * i / 64.0;
//             double phi = M_PI * i / 32.0;
//             double x = 0.5 * std::sin(theta) * std::cos(phi);
//             double y = 0.5 * std::sin(theta) * std::sin(phi);
//             double z = 0.5 * std::cos(theta);
//             vectors[i] = std::make_tuple(x, y, z);
//         }
//         return vectors;
//     }

//     Point simhash_to_coordinates(Simhash::hash_t hash)
//     {
//         static const auto bit_vectors = generate_bit_vectors();
//         Point p{0.0, 0.0, 0.0};
//         for (int i = 0; i < 64; ++i)
//         {
//             if (hash & (1ULL << i))
//             {
//                 p.x += std::get<0>(bit_vectors[i]);
//                 p.y += std::get<1>(bit_vectors[i]);
//                 p.z += std::get<2>(bit_vectors[i]);
//             }
//         }
//         return p;
//     }

//     std::vector<Simhash::hash_t> generate_hashes(const BitEncodedSeq &seq)
//     {
//         std::vector<Simhash::hash_t> hashes;
//         const uint32_t window_size = 4; // Tune this for sensitivity
//         const uint32_t step = 1;        // Overlapping windows for more sensitivity
//         const uint32_t length = seq.size();

//         if (window_size > length)
//         {
//             // If sequence is shorter than window, hash the entire sequence
//             Simhash::hash_t hash = GeneticSimhash::custom_nucleotide_hash(seq, 0, length);
//             hashes.push_back(hash);
//         }
//         else
//         {
//             for (uint32_t start = 0; start + window_size <= length; start += step)
//             {
//                 Simhash::hash_t hash = GeneticSimhash::custom_nucleotide_hash(seq, start, window_size);
//                 hashes.push_back(hash);
//             }
//         }
//         return hashes;
//     }

//     // Main function to compute coordinates from a BitEncodedSeq
//     Point calculate_coordinates(const BitEncodedSeq &seq)
//     {
//         std::vector<Simhash::hash_t> hashes = generate_hashes(seq);
//         Simhash::hash_t simhash = Simhash::compute(hashes);
//         return simhash_to_coordinates(simhash);
//     }
// }

//
