#pragma once
#include "SpeciesRecord.h"
#include "Octree.h"
#include "CSVParser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <tuple>
#include <cmath>
class Testcases
{
private:
    std::string testFilePath;
    std::unique_ptr<PhylogeneticTree> phyloTree;
    std::vector<SpeciesRecord> records;
    std::string randint;
    int len;

public:
    Testcases(const std::string &filePath) : testFilePath(filePath)
    {
        if (filePath == "../input/MediumData.csv")
        {
            std::cout << "Test file path is set to: " << testFilePath << std::endl;
            CSVParser parser(testFilePath);
            phyloTree = parser.createPhyloTree();
            len = phyloTree->getLength();
        }
        else if (filePath == "../input/SmallData.csv")
        {
            std::cout << "Test file path is set to: " << testFilePath << std::endl;
            CSVParser parser(testFilePath);
            phyloTree = parser.createPhyloTree();
            len = phyloTree->getLength();
        }
        else if (filePath == "../input/LargeData.csv")
        {
            std::cout << "Test file path is set to: " << testFilePath << std::endl;
            CSVParser parser(testFilePath);
            phyloTree = parser.createPhyloTree();
            len = phyloTree->getLength();
        }
        else
        {
            throw std::invalid_argument("Invalid test file path.");
        }
    }

    void createRecords()
    {
        std::ifstream file("../input/TestData.csv");
        if (!file.is_open())
        {
            throw std::runtime_error("Could not open file");
        }
        std::string line;
        std::getline(file, line);
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(1, 10);
        int randNum = distrib(gen);
        while (std::getline(file, line))
        {
            std::cout << randNum << std::endl;
            std::stringstream ss(line);
            std::string speciesName, seq;
            if (std::getline(ss, speciesName, ',') && std::getline(ss, seq, ','))
            {
                if (seq.size() == len)
                {
                    records.emplace_back(speciesName, seq);
                }
                else if (seq.size() > len)
                {
                    records.emplace_back(speciesName, seq.substr(0, len)); // snip longer
                }
                // sequences shorter than expected are ignored
            }
        }
    }

    void testInsert()
    {
        std::cout << "Running testInsert..." << std::endl;

        for (const auto &record : records)
        {
            phyloTree->insert(record);
        }
        std::cout << "Inserted " << records.size() << " records into the phylogenetic tree." << std::endl;
    }

    void testRemove(const std::string &name)
    {
        if (name != "hold")
        {
            std::cout << "Running testRemove..." << name << std::endl;
            phyloTree->remove(name);
            std::cout << "Removed record: " << name << std::endl;
        }
        std::cout << "Running testRemove..." << std::endl;
        for (const auto &record : records)
        {
            phyloTree->remove(record.speciesName);
        }
        std::cout << "Removed all records from the phylogenetic tree." << std::endl;
    }

    void testMutate(const std::string &name)
    {
        if (name != "hold")
        {
            std::cout << "Running testMutate..." << name << std::endl;
            double mutationRatio = phyloTree->mutate(name);
            std::cout << "Mutated ratio for " << name << ": " << mutationRatio << std::endl;
        }
        std::cout << "Running testMutate..." << std::endl;
        for (const auto &record : records)
        {
            double mutationRatio = phyloTree->mutate(record.speciesName);
            std::cout << "Mutation ratio for " << record.speciesName << ": " << mutationRatio << std::endl;
        }
    }
    void testSearchAndPrint()
    {
        std::cout << "Running testSearchAndPrint..." << std::endl;
        for (const auto &record : records)
        {
            phyloTree->searchAndPrint(record.speciesName);
        }
    }
    void testPrint()
    {
        std::cout << "Running testPrint..." << std::endl;
        phyloTree->print();
    }
    void testAll()
    {
        std::string hold = "hold";
        createRecords();
        testInsert();
        testMutate(hold);
        testSearchAndPrint();
        testRemove(hold);
        testPrint();
    }
};