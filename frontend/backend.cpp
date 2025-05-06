// backend.cpp
// This is a simplified implementation of your C++ backend

#include "backend.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

// Helper functions
std::string trim(const std::string &str)
{
    auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char c)
                                  { return std::isspace(c); });
    auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char c)
                                { return std::isspace(c); })
                   .base();
    return (start < end) ? std::string(start, end) : std::string();
}

std::string to_title_case(const std::string &str)
{
    if (str.empty())
        return str;

    std::string result = str;
    bool make_upper = true;

    for (auto &c : result)
    {
        if (std::isalpha(c))
        {
            if (make_upper)
                c = std::toupper(c);
            else
                c = std::tolower(c);
            make_upper = false;
        }
        else if (std::isspace(c) || c == '-' || c == '_')
        {
            make_upper = true;
        }
    }

    return result;
}

// DataManager implementation
DataManager::DataManager(const std::string &csv_file) : csv_file(csv_file)
{
    load_csv();
}

void DataManager::load_csv()
{
    records.clear();

    std::ifstream file(csv_file);
    if (!file.is_open())
    {
        return; // Handle error in a real implementation
    }

    std::string line;
    std::getline(file, line); // Read header

    std::vector<std::string> headers;
    std::stringstream ss(line);
    std::string header;

    while (std::getline(ss, header, ','))
    {
        headers.push_back(trim(header));
    }

    while (std::getline(file, line))
    {
        std::stringstream row_ss(line);
        std::string cell;
        std::vector<std::string> row_values;

        while (std::getline(row_ss, cell, ','))
        {
            row_values.push_back(trim(cell));
        }

        if (row_values.size() >= headers.size())
        {
            Record record;

            for (size_t i = 0; i < headers.size(); ++i)
            {
                if (headers[i] == "NodeType")
                {
                    record.NodeType = row_values[i];
                }
                else if (headers[i] == "Name")
                {
                    record.Name = to_title_case(trim(row_values[i]));
                }
                else if (headers[i] == "Parent")
                {
                    record.Parent = to_title_case(trim(row_values[i]));
                }
                else if (headers[i] == "GeneticSequence")
                {
                    record.GeneticSequence = row_values[i];
                }
                else if (headers[i] == "Weight")
                {
                    try
                    {
                        record.Weight = std::stoi(trim(row_values[i]));
                    }
                    catch (...)
                    {
                        record.Weight = 0;
                    }
                }
            }

            records.push_back(record);
        }
    }
}

bool DataManager::save_in_memory_record(const Record &record)
{
    records.push_back(record);
    return true;
}

bool DataManager::delete_family(const std::string &family_name)
{
    if (to_title_case(family_name) == "Sentinel")
    {
        return false;
    }

    std::string titled_name = to_title_case(family_name);

    auto new_end = std::remove_if(records.begin(), records.end(),
                                  [&titled_name](const Record &r)
                                  {
                                      return (r.NodeType == "Family" && r.Name == titled_name) ||
                                             (r.NodeType == "Species" && r.Parent == titled_name);
                                  });

    if (new_end != records.end())
    {
        records.erase(new_end, records.end());
        return true;
    }

    return false;
}

Record DataManager::find_by_genetic_sequence(const std::string &sequence)
{
    for (const auto &record : records)
    {
        if (record.NodeType == "Species" && record.GeneticSequence == sequence)
        {
            return record;
        }
    }
    return Record(); // Return empty record if not found
}

bool DataManager::name_exists_with_diff_sequence(const std::string &node_type,
                                                 const std::string &name,
                                                 const std::string &sequence)
{
    std::string lower_name = name;
    std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(),
                   [](unsigned char c)
                   { return std::tolower(c); });

    for (const auto &record : records)
    {
        std::string record_name = record.Name;
        std::transform(record_name.begin(), record_name.end(), record_name.begin(),
                       [](unsigned char c)
                       { return std::tolower(c); });

        if (record_name == lower_name)
        {
            if (node_type == "Species")
            {
                if (record.GeneticSequence != sequence)
                {
                    return true;
                }
            }
            else
            {
                return true;
            }
        }
    }

    return false;
}

std::vector<Record> DataManager::get_family_dict()
{
    std::vector<Record> families;

    for (const auto &record : records)
    {
        if (record.NodeType == "Family")
        {
            families.push_back(record);
        }
    }

    return families;
}

std::vector<std::string> DataManager::get_family_list()
{
    std::vector<std::string> families;

    for (const auto &record : records)
    {
        if (record.NodeType == "Family")
        {
            families.push_back(record.Name);
        }
    }

    return families;
}

std::vector<Record> DataManager::get_species_by_family(const std::string &family_name)
{
    std::vector<Record> species;
    std::string titled_name = to_title_case(family_name);

    for (const auto &record : records)
    {
        if (record.NodeType == "Species" && record.Parent == titled_name)
        {
            species.push_back(record);
        }
    }

    return species;
}