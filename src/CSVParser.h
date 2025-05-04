#include "SpeciesRecord.h"
#include "Octree.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

class CSVParser
{
public:
    CSVParser(const std::string &filename_) : filename(filename_) {}

    PhylogeneticTree &createPhyloTree()
    {
        parse();
        auto root = std::make_unique<PhylogeneticTree>(0, 0, 0, 4095, 4095, 4095);
        for (const auto &record : records)
        {
            auto species = std::make_shared<SpeciesRecord>(record.speciesName, record.seq->blocks());
            root->insert(*species);
        }
        return *root;
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
        while (std::getline(file, line))
        {
            std::stringstream ss(line);
            std::string speciesName, seq;
            if (std::getline(ss, speciesName, ',') && std::getline(ss, seq, ','))
            {
                records.emplace_back(speciesName, seq);
            }
        }
    }
};
