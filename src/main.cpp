// #include "Octree.h"
// #include <random>
// #include <ctime>
// #include <vector>
// #include <iostream>

// #include <iostream>
// #include <vector>
// #include <string>
// #include <iomanip>
// #include <cmath>
// #include <chrono>
// #include <random>
// #include <algorithm>
// #include <map>
// #include <cassert>
// #include "GeneticSimhash.h"
// #include "simhash.h"

// // Include your existing code (assuming all in a header file)
// // You would need to move the previously created functions into a header file
// // or include them directly in this file

// // BitEncodedSeq class and SimHash namespace are assumed to be included

// // Forward declarations for the functions we defined earlier

// /**
//  * @brief Generate a random DNA sequence of specified length
//  *
//  * @param length Length of the sequence to generate
//  * @return std::string Random DNA sequence
//  */
// std::string
// generate_random_dna(size_t length)
// {
//     static const char nucleotides[] = "ACGT";
//     static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

//     std::string dna;
//     dna.reserve(length);

//     for (size_t i = 0; i < length; ++i)
//     {
//         dna.push_back(nucleotides[rng() % 4]);
//     }

//     return dna;
// }

// /**
//  * @brief Mutate a DNA sequence with specified number of mutations
//  *
//  * @param original Original DNA sequence
//  * @param mutations Number of point mutations to introduce
//  * @return std::string Mutated DNA sequence
//  */
// std::string mutate_dna(const std::string &original, size_t mutations)
// {
//     static const char nucleotides[] = "ACGT";
//     static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

//     std::string mutated = original;
//     std::uniform_int_distribution<> pos_dist(0, original.size() - 1);

//     for (size_t i = 0; i < mutations; ++i)
//     {
//         size_t pos = pos_dist(rng);
//         char current = mutated[pos];
//         char replacement;

//         // Choose a different nucleotide
//         do
//         {
//             replacement = nucleotides[rng() % 4];
//         } while (replacement == current);

//         mutated[pos] = replacement;
//     }

//     return mutated;
// }

// /**
//  * @brief Print a formatted representation of coordinates
//  */
// void print_coordinates(const std::tuple<double, double, double> &coords)
// {
//     std::cout << std::fixed << std::setprecision(6)
//               << "(" << std::get<0>(coords) << ", "
//               << std::get<1>(coords) << ", "
//               << std::get<2>(coords) << ")";
// }

// /**
//  * @brief Calculate Hamming distance between two DNA sequences
//  */
// size_t hamming_distance(const std::string &s1, const std::string &s2)
// {
//     size_t distance = 0;
//     size_t min_length = std::min(s1.length(), s2.length());

//     for (size_t i = 0; i < min_length; ++i)
//     {
//         if (s1[i] != s2[i])
//         {
//             distance++;
//         }
//     }

//     // Add difference in length if any
//     distance += std::max(s1.length(), s2.length()) - min_length;

//     return distance;
// }

// /**
//  * @brief Test basic functionality with single-point mutations
//  */
// void test_single_mutations()
// {
//     std::cout << "\n===== Testing Single Mutations =====\n";

//     // Create a base sequence
//     std::string base_dna = "ACGTACGTACGTACGTACGTACGTACGTACGT";
//     BitEncodedSeq base_seq(base_dna);

//     auto base_coords = claude::dna_simhash_to_coordinates(base_seq, 6, 1);
//     std::cout << "Base sequence: " << base_dna << "\n";
//     std::cout << "Base coordinates: ";
//     print_coordinates(base_coords);
//     std::cout << "\n\n";

//     // Test with mutations at different positions
//     for (size_t pos = 0; pos < base_dna.length(); pos += 4)
//     {
//         std::string mutated = base_dna;

//         // Try different mutations at this position
//         for (char nucleotide : {'A', 'C', 'G', 'T'})
//         {
//             if (mutated[pos] != nucleotide)
//             {
//                 mutated[pos] = nucleotide;
//                 BitEncodedSeq mutated_seq(mutated);

//                 auto coords = claude::dna_simhash_to_coordinates(mutated_seq, 6, 1);
//                 double distance = claude::dna_spatial_distance(base_seq, mutated_seq);

//                 std::cout << "Mutation at position " << pos << " ("
//                           << base_dna[pos] << " -> " << nucleotide << "): ";
//                 print_coordinates(coords);
//                 std::cout << " | Distance: " << distance << "\n";

//                 break; // Just do one mutation per position
//             }
//         }
//     }
// }

// /**
//  * @brief Test with multiple mutations and analyze distance correlation
//  */
// void test_multiple_mutations()
// {
//     std::cout << "\n===== Testing Multiple Mutations =====\n";

//     // Create a longer base sequence
//     std::string base_dna = generate_random_dna(100);
//     BitEncodedSeq base_seq(base_dna);

//     auto base_coords = claude::dna_simhash_to_coordinates(base_seq, 6, 1);
//     std::cout << "Base sequence (first 20 bases): " << base_dna.substr(0, 20) << "...\n";
//     std::cout << "Base coordinates: ";
//     print_coordinates(base_coords);
//     std::cout << "\n\n";

//     std::cout << "Mutations | Hamming Distance | Spatial Distance\n";
//     std::cout << "-------------------------------------------\n";

//     // Test with increasing number of mutations
//     for (size_t mutations = 1; mutations <= 50; mutations += 2)
//     {
//         std::string mutated = mutate_dna(base_dna, mutations);
//         BitEncodedSeq mutated_seq(mutated);

//         auto coords = claude::dna_simhash_to_coordinates(mutated_seq, 6, 1);
//         double spatial_distance = claude::dna_spatial_distance(base_seq, mutated_seq);
//         size_t ham_distance = hamming_distance(base_dna, mutated);

//         std::cout << std::setw(9) << mutations << " | "
//                   << std::setw(15) << ham_distance << " | "
//                   << std::setw(15) << spatial_distance << "\n";
//     }
// }

// /**
//  * @brief Test with structurally similar sequences (e.g., repeats, insertions)
//  */
// void test_structural_similarity()
// {
//     std::cout << "\n===== Testing Structural Similarity =====\n";

//     // Test with repeat sequences
//     std::vector<std::string> test_cases = {
//         "AAAAAAAAAA",                      // All A's
//         "ACACACACACAC",                    // AC repeats
//         "ACGTACGTACGT",                    // ACGT repeats
//         "AAAAAAAAAAAAAAAACCCCCCCCCCCCCCC", // A's followed by C's
//         "ACGTACGTACGTTTTTTTTT",            // ACGT repeats followed by T's
//         "ACGTACGTACGACGTACGTACG",          // Similar but not exact repeats
//         "AAAAACCCCCGGGGGTTTTTT"            // Blocks of each nucleotide
//     };

//     std::cout << "Sequence Pair | Hamming Distance | Spatial Distance\n";
//     std::cout << "-------------------------------------------\n";

//     // Compare each sequence with every other sequence
//     for (size_t i = 0; i < test_cases.size(); ++i)
//     {
//         BitEncodedSeq seq1(test_cases[i]);

//         for (size_t j = i + 1; j < test_cases.size(); ++j)
//         {
//             BitEncodedSeq seq2(test_cases[j]);

//             // Calculate distances
//             double spatial_distance = claude::dna_spatial_distance(seq1, seq2);
//             size_t ham_distance = hamming_distance(test_cases[i], test_cases[j]);

//             std::cout << "Case " << i + 1 << " vs " << j + 1 << " | "
//                       << std::setw(15) << ham_distance << " | "
//                       << std::setw(15) << spatial_distance << "\n";
//         }
//     }
// }

// /**
//  * @brief Test with insertion and deletion mutations
//  */
// void test_indels()
// {
//     std::cout << "\n===== Testing Insertions and Deletions =====\n";

//     std::string base_dna = "ACGTACGTACGTACGTACGTACGT";
//     BitEncodedSeq base_seq(base_dna);

//     std::vector<std::pair<std::string, std::string>> test_cases = {
//         {"Base", base_dna},
//         {"Insert-1", "ACGTACGTAACGTACGTACGTACGT"}, // Insert A at position 10
//         {"Insert-3", "ACGTACGTTTTACGTACGTACGT"},   // Insert TTT at position 8
//         {"Delete-1", "ACGTACGTCGTACGTACGTACGT"},   // Delete A at position 8
//         {"Delete-3", "ACGTACGTACGTACGT"},          // Delete 3 bases
//         {"Replace-3", "ACGTACGTTTTTACGTACGTACGT"}, // Replace 3 bases with TTTT
//     };

//     // Calculate and print coordinates for each test case
//     std::cout << "Test Case | Coordinates | Distance from Base\n";
//     std::cout << "-------------------------------------------\n";

//     for (const auto &[name, seq] : test_cases)
//     {
//         BitEncodedSeq encoded_seq(seq);
//         auto coords = claude::dna_simhash_to_coordinates(encoded_seq, 6, 1);
//         double distance = (name == "Base") ? 0.0 : claude::dna_spatial_distance(base_seq, encoded_seq);

//         std::cout << std::setw(10) << name << " | ";
//         print_coordinates(coords);
//         std::cout << " | " << distance << "\n";
//     }
// }

// /**
//  * @brief Generate closely related sequences and plot their distances
//  */
// void test_related_sequences()
// {
//     std::cout << "\n===== Testing Related Sequence Families =====\n";

//     // Generate 5 base sequences
//     const size_t num_families = 5;
//     const size_t seq_length = 50;
//     std::vector<std::string> base_sequences;

//     for (size_t i = 0; i < num_families; ++i)
//     {
//         base_sequences.push_back(generate_random_dna(seq_length));
//     }

//     // For each base sequence, create 5 variations with increasing mutations
//     std::map<std::string, std::vector<std::tuple<double, double, double>>> family_coords;

//     for (size_t family = 0; family < num_families; ++family)
//     {
//         std::string family_name = "Family-" + std::to_string(family + 1);
//         std::vector<std::tuple<double, double, double>> coords_list;

//         std::cout << family_name << " base: " << base_sequences[family].substr(0, 20) << "...\n";

//         // Add base sequence
//         BitEncodedSeq base_seq(base_sequences[family]);
//         coords_list.push_back(claude::dna_simhash_to_coordinates(base_seq, 6, 1));

//         // Add mutations
//         for (size_t mutations = 1; mutations <= 10; mutations += 2)
//         {
//             std::string mutated = mutate_dna(base_sequences[family], mutations);
//             BitEncodedSeq mutated_seq(mutated);
//             coords_list.push_back(claude::dna_simhash_to_coordinates(mutated_seq, 6, 1));

//             double distance = claude::dna_spatial_distance(base_seq, mutated_seq);
//             std::cout << "  " << mutations << " mutations: distance = " << distance << "\n";
//         }

//         family_coords[family_name] = coords_list;
//     }

//     // Calculate inter-family vs intra-family distances
//     std::cout << "\nIntra-family vs Inter-family distance analysis:\n";
//     std::cout << "-------------------------------------------\n";

//     double avg_intra_distance = 0.0;
//     int intra_count = 0;
//     double avg_inter_distance = 0.0;
//     int inter_count = 0;

//     for (size_t i = 0; i < num_families; ++i)
//     {
//         std::string family1 = "Family-" + std::to_string(i + 1);
//         BitEncodedSeq base1(base_sequences[i]);

//         // Intra-family distances (within same family)
//         for (size_t mutations = 1; mutations <= 10; mutations += 2)
//         {
//             std::string mutated = mutate_dna(base_sequences[i], mutations);
//             BitEncodedSeq mutated_seq(mutated);
//             double distance = claude::dna_spatial_distance(base1, mutated_seq);

//             avg_intra_distance += distance;
//             intra_count++;
//         }

//         // Inter-family distances (between different families)
//         for (size_t j = i + 1; j < num_families; ++j)
//         {
//             BitEncodedSeq base2(base_sequences[j]);
//             double distance = claude::dna_spatial_distance(base1, base2);

//             avg_inter_distance += distance;
//             inter_count++;
//         }
//     }

//     avg_intra_distance /= intra_count;
//     avg_inter_distance /= inter_count;

//     std::cout << "Average intra-family distance: " << avg_intra_distance << "\n";
//     std::cout << "Average inter-family distance: " << avg_inter_distance << "\n";
//     std::cout << "Ratio (inter/intra): " << (avg_inter_distance / avg_intra_distance) << "\n";
// }

// /**
//  * @brief Test k-mer size impact on coordinate generation
//  */
// void test_kmer_parameters()
// {
//     std::cout << "\n===== Testing k-mer Parameter Impact =====\n";

//     // Generate a test sequence
//     std::string base_dna = generate_random_dna(100);
//     BitEncodedSeq base_seq(base_dna);

//     std::cout << "Base sequence (first 20 bases): " << base_dna.substr(0, 20) << "...\n\n";

//     // Create a related sequence with 5 mutations
//     std::string related_dna = mutate_dna(base_dna, 5);
//     BitEncodedSeq related_seq(related_dna);

//     // Create an unrelated sequence
//     std::string unrelated_dna = generate_random_dna(100);
//     BitEncodedSeq unrelated_seq(unrelated_dna);

//     std::cout << "k-mer size | step | Related Distance | Unrelated Distance | Ratio\n";
//     std::cout << "--------------------------------------------------------------\n";

//     // Test different k-mer sizes and step values
//     for (size_t k = 3; k <= 12; k += 3)
//     {
//         for (size_t step : {1, 2, 4})
//         {
//             // Only process if k is valid for the sequence
//             if (k <= base_dna.length())
//             {
//                 double related_dist = dna_spatial_distance(
//                     base_seq,
//                     related_seq,
//                     k,
//                     step);

//                 double unrelated_dist = dna_spatial_distance(
//                     base_seq,
//                     unrelated_seq,
//                     k,
//                     step);

//                 double ratio = unrelated_dist / related_dist;

//                 std::cout << std::setw(9) << k << " | "
//                           << std::setw(4) << step << " | "
//                           << std::setw(16) << related_dist << " | "
//                           << std::setw(18) << unrelated_dist << " | "
//                           << std::setw(5) << ratio << "\n";
//             }
//         }
//     }
// }

// /**
//  * @brief Measure computation time for different sequence lengths
//  */
// void benchmark_performance()
// {
//     std::cout << "\n===== Performance Benchmark =====\n";

//     std::vector<size_t> lengths = {100, 500, 1000, 5000, 10000};

//     std::cout << "Sequence Length | Computation Time (ms)\n";
//     std::cout << "------------------------------------\n";

//     for (size_t length : lengths)
//     {
//         std::string dna = generate_random_dna(length);
//         BitEncodedSeq seq(dna);

//         auto start = std::chrono::high_resolution_clock::now();

//         // Run the computation 10 times for more stable measurement
//         for (int i = 0; i < 10; ++i)
//         {
//             auto coords = claude::dna_simhash_to_coordinates(seq, 6, 1);
//             // Use the result to prevent compiler optimization
//             volatile double x = std::get<0>(coords);
//         }

//         auto end = std::chrono::high_resolution_clock::now();
//         auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 10.0;

//         std::cout << std::setw(15) << length << " | "
//                   << std::setw(21) << duration << "\n";
//     }
// }

// /**
//  * Extension of dna_spatial_distance that takes k-mer and step parameters
//  */
// double dna_spatial_distance(const BitEncodedSeq &seq1, const BitEncodedSeq &seq2, size_t k, size_t step)
// {
//     // Get 3D coordinates for both sequences with specified parameters
//     auto [x1, y1, z1] = claude::dna_simhash_to_coordinates(seq1, k, step);
//     auto [x2, y2, z2] = claude::dna_simhash_to_coordinates(seq2, k, step);

//     // Calculate Euclidean distance
//     double dx = x2 - x1;
//     double dy = y2 - y1;
//     double dz = z2 - z1;

//     return std::sqrt(dx * dx + dy * dy + dz * dz);
// }

// int main()
// {
//     std::cout << "=== DNA SimHash to 3D Coordinates Test Suite ===\n";

//     // Run each test
//     test_single_mutations();
//     test_multiple_mutations();
//     test_structural_similarity();
//     test_indels();
//     test_related_sequences();
//     test_kmer_parameters();
//     benchmark_performance();

//     std::cout << "\nAll tests completed.\n";
//     return 0;
// }

// // int main()
// // {
// //     OctreeNode root(0, 0, 0, 4095, 4095, 4095);

// //     std::vector<Object3D *> rawRefs;
// //     rawRefs.reserve(20);

// //     std::mt19937 rng(static_cast<unsigned int>(time(nullptr)));
// //     std::uniform_int_distribution<int> dist(0, 4095);

// //     std::cout << "=== Inserting Objects ===\n";
// //     for (int i = 0; i < 20; ++i)
// //     {
// //         int x = dist(rng);
// //         int y = dist(rng);
// //         int z = dist(rng);
// //         auto obj = std::make_unique<Object3D>(x, y, z);
// //         Object3D *ref = obj.get();
// //         rawRefs.push_back(ref);
// //         root.insert(std::move(obj));
// //         std::cout << "Inserted (" << x << ", " << y << ", " << z << ")\n";
// //     }

// //     std::cout << "\n=== Octree After Insertions ===\n";
// //     root.print();

// //     std::cout << "\n=== Removing First 7 Objects ===\n";
// //     for (int i = 0; i < 7; ++i)
// //     {
// //         Object3D *ptr = rawRefs[i];
// //         int x = ptr->x;
// //         int y = ptr->y;
// //         int z = ptr->z;
// //         bool removed = root.remove(ptr);
// //         std::cout << "Removed (" << x << ", " << y << ", " << z << "): "
// //                   << (removed ? "Success" : "Failure") << "\n";
// //         rawRefs[i] = nullptr;
// //     }

// //     std::cout << "\n=== Octree After Removals ===\n";
// //     root.print();

// //     std::cout << "\n=== Total Subdivisions: " << OctreeNode::totalSubdivides << " ===\n";

// //     return 0;
// // }