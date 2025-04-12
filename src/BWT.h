#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <cstdint>
#include <unordered_set>
#include <memory>

class RLEBWT
{
public:
    // Constructor: takes a gene sequence, computes its BWT,
    // and converts that into a run-length encoded vector of pairs.
    RLEBWT(const std::string &gene)
    {
        std::string bwt = computeBWT(gene);
        rle = runLengthEncode(bwt);
    }

    // Returns the run-length encoded BWT as a vector of (count, base) pairs.
    const std::vector<std::pair<int, char>> &get() const
    {
        return rle;
    }

    // Updates the RLEBWT with a new run-length encoded vector.
    void set(const std::vector<std::pair<int, char>> &newRLE)
    {
        rle = newRLE;
    }

private:
    // Stores the run-length encoded BWT as a vector of pairs.
    // Each pair is of the form {count, base}
    std::vector<std::pair<int, char>> rle;

    // Computes the Burrows–Wheeler Transform of the input gene sequence.
    // The input gene sequence is appended with a terminator '$', all rotations are generated,
    // sorted, and the last column of the sorted rotations is returned as the BWT.
    std::string computeBWT(const std::string &input)
    {
        // Append a unique terminator (assumed not to appear in the input, e.g. '$').
        std::string s = input + "$";
        size_t n = s.size();

        // Generate all rotations (cyclic shifts) of the string.
        std::vector<std::string> rotations;
        rotations.reserve(n);
        for (size_t i = 0; i < n; ++i)
        {
            // Rotation: substring from i to end concatenated with substring from beginning to i.
            rotations.push_back(s.substr(i) + s.substr(0, i));
        }

        // Sort rotations lexicographically.
        std::sort(rotations.begin(), rotations.end());

        // Build the BWT result by taking the last character of each sorted rotation.
        std::string bwt;
        bwt.reserve(n);
        for (const auto &rotation : rotations)
        {
            bwt.push_back(rotation.back());
        }

        // Remove the terminator ('$').
        bwt.erase(std::remove(bwt.begin(), bwt.end(), '$'), bwt.end());

        return bwt;
    }

    // Converts the given string into its run-length encoded form
    // represented as a vector of (count, base) pairs.
    std::vector<std::pair<int, char>> runLengthEncode(const std::string &input)
    {
        std::vector<std::pair<int, char>> encoded;
        if (input.empty())
            return encoded;

        char currentChar = input[0];
        int count = 1;

        // Loop over the input starting from the second character.
        for (size_t i = 1; i < input.size(); ++i)
        {
            if (input[i] == currentChar)
            {
                ++count;
            }
            else
            {
                encoded.push_back({count, currentChar});
                currentChar = input[i];
                count = 1;
            }
        }
        // Append the final run.
        encoded.push_back({count, currentChar});
        return encoded;
    }
};