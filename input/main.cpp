#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <iterator>
#include <iomanip> // For formatting percentages

double getDifference(const std::string &seq1, const std::string &seq2)
{
    int diff = 0;
    for (size_t i = 0; i < seq1.size(); ++i)
    {
        if (seq1[i] != seq2[i])
        {
            ++diff;
        }
    }
    double raw = static_cast<double>(diff) / seq1.size() * 100.0; // Convert to percentage
    double truncated = std::floor(raw * 10.0) / 10.0;             // Keep one decimal place
    return truncated;
}

using namespace std;

// Function to read from a CSV, double the sequence, and write to a new CSV file
void doubleSequencesInCSV(const string &inputFile, const string &outputFile)
{
    ifstream inFile(inputFile);   // Open the input CSV file
    ofstream outFile(outputFile); // Open the output CSV file

    if (!inFile.is_open())
    {
        cerr << "Error: Could not open input file." << endl;
        return;
    }

    if (!outFile.is_open())
    {
        cerr << "Error: Could not open output file." << endl;
        return;
    }

    string line;
    vector<string> rows;

    // Skip the header line if present
    getline(inFile, line);

    // Read each line from the input file
    while (getline(inFile, line))
    {
        stringstream ss(line);
        string speciesName, geneSequence;

        // Split the line into species name and gene sequence
        getline(ss, speciesName, ',');
        getline(ss, geneSequence, ',');

        // Double the gene sequence
        geneSequence += geneSequence;

        // Write the new line to the output file
        outFile << speciesName << "," << geneSequence << endl;
    }

    inFile.close();  // Close the input file
    outFile.close(); // Close the output file

    cout << "Processing complete. New file saved to: " << outputFile << endl;
}

int main()
{

    std::string inputFilename = "MediumData.csv";             // Updated to match your CSV filename
    std::string outputFilename = "MediumDataDifferences.txt"; // Output file name

    std::ifstream file(inputFilename);
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open file: " + inputFilename);
    }

    std::ofstream outFile(outputFilename);
    if (!outFile.is_open())
    {
        throw std::runtime_error("Could not create output file: " + outputFilename);
    }

    std::string line;
    std::unordered_map<std::string, std::string> speciesMap;

    // Skip the header line
    std::getline(file, line);

    // Read species and sequences
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string speciesName, seq;
        if (std::getline(ss, speciesName, ',') && std::getline(ss, seq))
        {
            speciesMap[speciesName] = seq;
        }
    }
    file.close();

    // Calculate and write differences in the requested format
    for (const auto &species1 : speciesMap)
    {
        outFile << species1.first << ": {";

        bool firstDifference = true;
        for (const auto &species2 : speciesMap)
        {
            if (species1.first != species2.first)
            {
                if (!firstDifference)
                {
                    outFile << ", ";
                }

                double diff = getDifference(species1.second, species2.second);
                outFile << species2.first << ": " << diff << "%";
                firstDifference = false;
            }
        }

        outFile << "}" << std::endl;
    }
    outFile.close();

    std::cout << "Differences calculated and written to " << outputFilename << std::endl;

    return 0;
}