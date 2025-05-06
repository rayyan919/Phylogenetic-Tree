// backend_bridge.cpp
// This file creates Python bindings for your C++ backend

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "backend.h" // Include your C++ backend header

namespace py = pybind11;

// Assuming you have these classes in your C++ backend
// Modify according to your actual implementation

PYBIND11_MODULE(backend_bridge, m)
{
    m.doc() = "Python bindings for Genetic Tree backend";

    // Expose DataManager class
    py::class_<DataManager>(m, "DataManager")
        .def(py::init<const std::string &>())
        .def("load_csv", &DataManager::load_csv)
        .def("save_in_memory_record", &DataManager::save_in_memory_record)
        .def("delete_family", &DataManager::delete_family)
        .def("find_by_genetic_sequence", &DataManager::find_by_genetic_sequence)
        .def("name_exists_with_diff_sequence", &DataManager::name_exists_with_diff_sequence)
        .def("get_family_dict", &DataManager::get_family_dict)
        .def("get_family_list", &DataManager::get_family_list)
        .def("get_species_by_family", &DataManager::get_species_by_family);

    // Define Record type for Python
    py::class_<Record>(m, "Record")
        .def(py::init<>())
        .def_readwrite("NodeType", &Record::NodeType)
        .def_readwrite("Name", &Record::Name)
        .def_readwrite("Parent", &Record::Parent)
        .def_readwrite("GeneticSequence", &Record::GeneticSequence)
        .def_readwrite("Weight", &Record::Weight);
}