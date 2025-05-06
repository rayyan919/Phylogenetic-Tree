# Genetic Tree Visualization

This project visualizes genetic families using two tree structures:

1. **Octree** - For visualizing family hierarchies
2. **BK-Tree** - For visualizing species within a family

## Architecture

The application follows a hybrid architecture:

- **Backend**: C++ code that handles data management
- **Frontend**: PyQt6-based Python code for visualization

## Setup Instructions

### Prerequisites

- Python 3.8+
- C++ compiler compatible with C++11
- PyQt6
- pyqtgraph
- pybind11

### Installation

1. **Install Python Dependencies**:

   ```bash
   pip install PyQt6 pyqtgraph pybind11
   ```

2. **Build the C++ Backend**:

   ```bash
   python setup.py build_ext --inplace
   ```

3. **Run the Application**:
   ```bash
   python modified_frontend.py
   ```

## Files Description

### C++ Backend

- `backend.h` - Header file defining the data structures and classes
- `backend.cpp` - Implementation of backend functionality
- `backend_bridge.cpp` - Python bindings for C++ backend using pybind11
- `setup.py` - Script to build the Python extension

### Python Frontend

- `modified_frontend.py` - PyQt6-based frontend that visualizes the data structures

### Data

- `data.csv` - CSV file containing family and species data

## Usage

1. Launch the application to view the Octree (family hierarchy).
2. Click on a family node to:
   - Delete the family and all its species
   - View the BK-tree of species within that family
3. In the BK-tree view:
   - Click on species nodes to delete or mutate them
   - Click on the family node to delete the entire family

## Integration Details

The integration between Python and C++ is achieved using pybind11, which creates Python bindings for the C++ backend. This allows the Python frontend to call C++ functions directly.

Key integration points:

- DataManager class is exposed from C++ to Python
- Record structure is mapped between C++ and Python
- All CSV data operations are handled by the C++ backend
- Visualization and UI logic is handled by the Python frontend
