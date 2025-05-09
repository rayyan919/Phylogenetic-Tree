#🧬 Phylogenetic Tree using Octree + BK-Tree Hybrid (Data Structures II Project)

This project implements a **high-performance phylogenetic tree** for managing species based on **genetic similarity**, using a combination of **Octree spatial partitioning** and **BK-tree indexing** under Hamming distance.

📘 Built as a final project for **Data Structures II (Spring 2025)**.

---

## 🚀 Features

- ✅ DNA Sequence Compression using 2-bit encoding.
- ✅ Fast genetic similarity comparison with **Simhash**.
- ✅ Dynamic **Octree** for coarse 3D clustering.
- ✅ **BK-tree** indexing within Octree leaves for precise matching.
- ✅ Insertion, deletion, mutation, and search fully supported.
- ✅ Clean, modular, header-based C++ implementation.
- ✅ Terminal-based output for testing and structure validation.

---

## 🧠 Motivation

Real-world datasets such as:
- Biodiversity gene banks
- Pathogen mutation trackers
- Forensic DNA matching

...require scalable and **search-optimized** representations of genetic sequences. Our project shows how **hierarchical spatial + metric indexing** can accelerate this task.

---

## 🧬 Data Pipeline Overview

1. **Read species data** from a CSV (`name`, `DNA sequence`).
2. **Compress** DNA into bit-encoded format (2 bits per base).
3. **Generate Simhash** to summarize sequence into 64-bit signature.
4. **Map Simhash to 3D coordinates** → Insert into Octree.
5. **Insert into BK-Tree** inside the appropriate Octree leaf.
6. Support **search, mutation, insert, delete** operations.

> Each BK-tree is limited to ~20 species per leaf for efficiency. Overflow triggers Octree subdivision.

---
## 📊 Performance Summary

| Operation     | Time Complexity |
|---------------|------------------|
| Insert        | `O(log N + L)`   |
| Delete        | `O(log N + L)`   |
| Mutate        | `O(log N + L)`   |
| Search        | `O(log N + L)`   |
| Space (Total) | `O(N × L)`       |

Where:
- `N` = total number of species
- `L` = sequence length (fixed)
- `M` = max species in a BK-tree (≤ 20)

 ## 🤔 Why Not Just One Tree?
We evaluated other structures:

❌ Single massive BK-Tree: Doesn’t scale well with large datasets.

❌ Graph database: Too slow for similarity search; overkill for our needs.

✅ Octree + BK-Tree hybrid: Best of both worlds — spatial hierarchy + fast Hamming queries.
--

## 📚 Learnings
Hybrid trees can simplify search logic at large scale.

Frontend integration needs early planning.

DNA compression and simhashing are powerful tools for high-dimensional data.
--

## 📄 License
MIT License — feel free to use or modify this for your own projects!
--

## 👥 Authors
Rayyan Shah

Amn Naqvi

Tanzeel Shahid

Fatima Aijaz

Course: Data Structures II – Spring 2025
--

## 🙌 Acknowledgments
[Simhash GitHub Library](https://github.com/seomoz/simhash-cpp) used for similarity hashing

Academic discussions with DS2 Faculty

