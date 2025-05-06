// backend.h
// This is a simplified header for your C++ backend

#pragma once
#include <string>
#include <vector>
#include <map>

// Simple record structure to match your Python dict structure
struct Record
{
    std::string NodeType; // "Family" or "Species"
    std::string Name;
    std::string Parent;
    std::string GeneticSequence;
    int Weight;
};

// C++ implementation of your DataManager class
class DataManager
{
public:
    DataManager(const std::string &csv_file);

    void load_csv();
    bool save_in_memory_record(const Record &record);
    bool delete_family(const std::string &family_name);
    Record find_by_genetic_sequence(const std::string &sequence);
    bool name_exists_with_diff_sequence(const std::string &node_type,
                                        const std::string &name,
                                        const std::string &sequence);
    std::vector<Record> get_family_dict();
    std::vector<std::string> get_family_list();
    std::vector<Record> get_species_by_family(const std::string &family_name);

    // Additional methods for your BKTree and Octree implementations
    // Add as needed based on your C++ implementation

private:
    std::string csv_file;
    std::vector<Record> records;
};

// Your C++ Octree and BKTree implementations would be declared here
// But they're not needed for the Python binding since we're using PyQt for visualization