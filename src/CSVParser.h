#include "SpeciesRecord.h"
#include "Octree.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
class CSVParser
{
public:
    CSVParser(const std::string &filename_) : filename(filename_) {}

    std::unique_ptr<PhylogeneticTree> createPhyloTree()
    {
        parse();
        auto root = std::make_unique<PhylogeneticTree>(0, 0, 0, 4095, 4095, 4095, records[0].seq->size());
        std::cout << records.size() << std::endl;
        for (const auto &record : records)
        {
            std::cout << record.speciesName << " " << record.seq->size() << std::endl;
            std::cout << "Simhash: " << record.simhash << std::endl;
            std::cout << "Coords: ";
            std::cout << '[' << record.x << ", " << record.y << ", " << record.z << ']' << std::endl;

            root->insert(record);
        }
        return root;
    }

private:
    std::string filename;
    std::vector<SpeciesRecord> records;

    void parse()
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            throw std::runtime_error("Could not open file: " + filename);
        }
        std::string line;
        long long int size = -1; // initialized once
        std::getline(file, line);
        while (std::getline(file, line))
        {
            std::stringstream ss(line);
            std::string speciesName, seq;
            if (std::getline(ss, speciesName, ',') && std::getline(ss, seq, ','))
            {
                std::cout << speciesName << " " << seq.size() << std::endl;
                if (size == -1 && seq.size() > 63) // first record
                {
                    size = seq.size();
                    records.emplace_back(speciesName, seq);
                }
                else if (seq.size() == size)
                {
                    records.emplace_back(speciesName, seq);
                }
                else if (seq.size() > size)
                {
                    records.emplace_back(speciesName, seq.substr(0, size)); // snip longer
                }
                // sequences shorter than expected are ignored
            }
        }
    }
};
