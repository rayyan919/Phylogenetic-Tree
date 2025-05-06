#pragma once
#include "SpeciesRecord.h"
#include "Octree.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
class CSVParser
{
public:
    CSVParser(const std::string &filename_);

    std::unique_ptr<PhylogeneticTree> createPhyloTree();

private:
    std::string filename;
    std::vector<SpeciesRecord> records;

    void parse();
};
