#include "../include/Testcases.h"

void Testcases::createRecords()
{
    std::ifstream file("../input/TestData.csv");
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open file");
    }
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line))
    {
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

Testcases::Testcases(const std::string &filePath) : testFilePath(filePath)
{
    if (filePath == "../input/MediumData.csv")
    {
        std::cout << "Test file path is set to: " << testFilePath << std::endl;
        csvParser = std::make_unique<CSVParser>(testFilePath);
        phyloTree = csvParser->createPhyloTree();
        len = phyloTree->getLength();
        type = 1;
    }
    else if (filePath == "../input/SmallData.csv")
    {
        std::cout << "Test file path is set to: " << testFilePath << std::endl;
        csvParser = std::make_unique<CSVParser>(testFilePath);
        phyloTree = csvParser->createPhyloTree();
        len = phyloTree->getLength();
        type = 0;
    }
    else if (filePath == "../input/LargeData.csv")
    {
        std::cout << "Test file path is set to: " << testFilePath << std::endl;
        csvParser = std::make_unique<CSVParser>(testFilePath);
        phyloTree = csvParser->createPhyloTree();
        len = phyloTree->getLength();
        type = 2;
    }
    else
    {
        throw std::invalid_argument("Invalid test file path.");
    }
}

void Testcases::testInsert()
{
    std::cout << "Running testInsert..." << std::endl;

    for (const auto &record : records)
    {
        phyloTree->insert(record);
    }
    std::cout << "Inserted " << records.size() << " records into the phylogenetic tree." << std::endl;
}

void Testcases::testRemove(const std::string &name)
{
    if (name != "hold")
    {
        std::cout << "Running testRemove on " << name << std::endl;
        phyloTree->remove(name);
        std::cout << "Removed record: " << name << std::endl;
    }
    else // If no name is provided, remove a random species
    {
        std::cout << "Running testRemove..." << std::endl;
        std::string randName = csvParser->getRandomSpeciesName();
        bool removed = phyloTree->remove(randName);
        if (!removed)
        {
            std::cout << "Failed to remove " << randName << " from tree." << std::endl;
        }
        else
        {
            std::cout << "Successfully removed " << randName << " from tree." << std::endl;
        }
    }
}

void Testcases::testMutate(const std::string &name)
{
    if (name != "hold")
    {
        std::cout << "Running testMutate on " << name << std::endl;
        double mutationRatio = phyloTree->mutate(name);
        std::cout << "Mutation ratio for " << name << ": " << mutationRatio << std::endl;
    }
    else // If no name is provided, mutate a random species
    {
        std::cout << "Running testMutate..." << std::endl;
        std::string randName = csvParser->getRandomSpeciesName();
        double mutationRatio = phyloTree->mutate(randName);
        if (mutationRatio == 0.0)
        {
            std::cout << "No mutation occurred on " << randName << std::endl;
        }
        else
        {
            std::cout << "Successfully mutated" << randName << " (" << mutationRatio << ')' << std::endl;
        }
    }
}

void Testcases::testSearchAndPrint()
{
    std::cout << "Running testSearchAndPrint..." << std::endl;
    std::string randName = csvParser->getRandomSpeciesName();
    phyloTree->searchAndPrint(randName);
    std::cout << "Searched and printed record: " << randName << std::endl;
}

void Testcases::testPrint()
{
    std::cout << "Running testPrint on entire tree..." << std::endl;
    phyloTree->print();
}

void Testcases::testAll()
{
    std::string hold = "hold";
    createRecords();
    testInsert();
    testMutate(hold);
    testSearchAndPrint();
    testRemove(hold);
    testPrint();
}