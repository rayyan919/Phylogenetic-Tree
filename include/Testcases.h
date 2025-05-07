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
    std::unique_ptr<CSVParser> csvParser;
    std::vector<SpeciesRecord> records;
    std::string randint;
    int len;
    int type; // 0: SmallData; 1: MediumData; 2: LargeData

    void createRecords();

public:
    Testcases(const std::string &filePath);

    void testInsert();

    void testRemove(const std::string &name);

    void testMutate(const std::string &name);
    void testSearchAndPrint();
    void testPrint();
    void testAll();
};