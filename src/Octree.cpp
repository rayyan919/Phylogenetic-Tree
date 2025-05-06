#include "../include/Octree.h"

int PhylogeneticTree::length = 0;
PhylogeneticTree::PhylogeneticTree(int minX, int minY, int minZ,
                                   int maxX, int maxY, int maxZ)
    : bounds_{minX, minY, minZ, maxX, maxY, maxZ},
      root_(std::make_unique<Node>(bounds_)) {}

void PhylogeneticTree::insert(const SpeciesRecord &rec)
{
    insertRec(root_.get(), rec);
    nameToCoords_[rec.speciesName] = std::make_tuple(rec.x, rec.y, rec.z);
}

bool PhylogeneticTree::remove(const std::string &name)
{
    auto it = nameToCoords_.find(name);
    if (it == nameToCoords_.end())
        return false;
    bool removed = removeRec(root_.get(), name);
    if (removed)
        nameToCoords_.erase(it);
    return removed;
}

double PhylogeneticTree::mutate(const std::string &name)
{
    auto it = nameToCoords_.find(name);
    if (it == nameToCoords_.end())
        return -1.0;
    std::shared_ptr<SpeciesRecord> rec = findLeaf(root_.get(), std::get<0>(it->second),
                                                  std::get<1>(it->second), std::get<2>(it->second))
                                             ->family->findExact(name);
    if (!rec)
        return -1.0;
    double ratio = rec->mutate();
    if (ratio > 0.0)
    {
        remove(name);
        insert(*rec);
    }
    return ratio;
}

bool PhylogeneticTree::searchAndPrint(const std::string &name)
{
    auto it = nameToCoords_.find(name);
    if (it == nameToCoords_.end())
    {
        std::cout << "Species not found: " << name << std::endl;
        return false;
    }
    auto found = findLeaf(root_.get(), std::get<0>(it->second),
                          std::get<1>(it->second), std::get<2>(it->second))
                     ->b;
    std::cout
        << "Species found: " << name << std::endl;
    std::cout << "Coordinates: ("
              << std::get<0>(it->second) << ", "
              << std::get<1>(it->second) << ", "
              << std::get<2>(it->second) << ")" << std::endl;
    std::cout << "Found in: " << '[' << found.minX << 'x' << found.maxX << ']';
    std::cout << " x [" << found.minY << 'x' << found.maxY << ']';
    std::cout << " x [" << found.minZ << 'x' << found.maxZ << ']';
    return true;
}

void PhylogeneticTree::print() const
{
    printRec(root_.get(), 0);
}

void PhylogeneticTree::insertRec(Node *node, const SpeciesRecord &rec)
{
    if (!contains(node->b, rec))
        return;
    if (node->isLeaf && length == 0)
    {
        length = rec.seq->size(); // set length on first insert
        if (node->family->canInsert(rec))
        {
            node->family->insert(rec);
            return;
        }
        subdivideNode(node);
    }
    else if (node->isLeaf)
    {
        if (node->family->canInsert(rec))
        {
            node->family->insert(rec);
            return;
        }
        subdivideNode(node);
    }
    int idx = octantIndex(node->b, rec.x, rec.y, rec.z);
    insertRec(node->children[idx].get(), rec);
}

bool PhylogeneticTree::removeRec(Node *node, const std::string &name)
{
    auto it = nameToCoords_.find(name);
    if (it == nameToCoords_.end())
        return false;
    int x = std::get<0>(it->second);
    int y = std::get<1>(it->second);
    int z = std::get<2>(it->second);
    Node *node_ = findLeaf(node, x, y, z);
    if (!node_)
        return false;
    node_->family->remove(name);
    return true;
}

std::shared_ptr<SpeciesRecord> PhylogeneticTree::findSpeciesRec(Node *node, const std::string &name)
{
    if (!node)
        return nullptr;
    auto it = nameToCoords_.find(name);
    int x = std::get<0>(it->second);
    int y = std::get<1>(it->second);
    int z = std::get<2>(it->second);
    return findLeaf(node, x, y, z)->family->findExact(name);
}

PhylogeneticTree::Node *PhylogeneticTree::findLeaf(Node *node, int x, int y, int z)
{
    if (!node)
        return nullptr;
    if (node->isLeaf && inBounds(node->b, x, y, z))
        return node;
    int idx = octantIndex(node->b, x, y, z);
    return findLeaf(node->children[idx].get(), x, y, z);
}

void PhylogeneticTree::printRec(const Node *node, int depth) const
{
    std::string indent(depth * 2, ' ');
    if (node->isLeaf)
    {
        std::cout << indent << "Leaf ["
                  << node->b.minX << "," << node->b.maxX << "] x ["
                  << node->b.minY << "," << node->b.maxY << "] x ["
                  << node->b.minZ << "," << node->b.maxZ
                  << "] : " << node->family->size() << " members\n";
        std::cout << indent << "Species records:";
        node->family->print();
        std::cout << "\n";
    }
    else
    {
        std::cout << indent << "Branch ["
                  << node->b.minX << "," << node->b.maxX << "]\n";
        for (const auto &c : node->children)
        {
            if (c)
                printRec(c.get(), depth + 1);
        }
    }
}

void PhylogeneticTree::subdivideNode(Node *node)
{
    node->isLeaf = false;
    auto &b = node->b;
    int midX = (b.minX + b.maxX) >> 1;
    int midY = (b.minY + b.maxY) >> 1;
    int midZ = (b.minZ + b.maxZ) >> 1;
    Bounds bs[8] = {
        {b.minX, b.minY, b.minZ, midX, midY, midZ},
        {midX + 1, b.minY, b.minZ, b.maxX, midY, midZ},
        {b.minX, midY + 1, b.minZ, midX, b.maxY, midZ},
        {midX + 1, midY + 1, b.minZ, b.maxX, b.maxY, midZ},
        {b.minX, b.minY, midZ + 1, midX, midY, b.maxZ},
        {midX + 1, b.minY, midZ + 1, b.maxX, midY, b.maxZ},
        {b.minX, midY + 1, midZ + 1, midX, b.maxY, b.maxZ},
        {midX + 1, midY + 1, midZ + 1, b.maxX, b.maxY, b.maxZ},
    };
    for (int i = 0; i < 8; ++i)
    {
        node->children[i] = std::make_unique<Node>(bs[i]);
    }
    auto oldNodes = node->family->collectAll();
    node->family->reset();
    for (const auto &bkNode : oldNodes)
    {
        const auto &rec = *bkNode->species;
        int idx = octantIndex(node->b, rec.x, rec.y, rec.z);
        node->children[idx]->family->insert(rec);
    }
}

bool PhylogeneticTree::contains(const Bounds &b, const SpeciesRecord &r) const
{
    return r.x >= b.minX && r.x <= b.maxX && r.y >= b.minY && r.y <= b.maxY && r.z >= b.minZ && r.z <= b.maxZ;
}

bool PhylogeneticTree::contains(const Bounds &b, const std::tuple<int, int, int> coords) const
{
    return std::get<0>(coords) >= b.minX && std::get<0>(coords) <= b.maxX &&
           std::get<1>(coords) >= b.minY && std::get<1>(coords) <= b.maxY &&
           std::get<2>(coords) >= b.minZ && std::get<2>(coords) <= b.maxZ;
}

bool PhylogeneticTree::inBounds(const Bounds &b, int x, int y, int z) const
{
    return x >= b.minX && x <= b.maxX && y >= b.minY && y <= b.maxY && z >= b.minZ && z <= b.maxZ;
}

int PhylogeneticTree::octantIndex(const Bounds &b, int x, int y, int z) const
{
    int idx = 0;
    int midX = (b.minX + b.maxX) >> 1;
    int midY = (b.minY + b.maxY) >> 1;
    int midZ = (b.minZ + b.maxZ) >> 1;
    if (x > midX)
        idx |= 1;
    if (y > midY)
        idx |= 2;
    if (z > midZ)
        idx |= 4;
    return idx;
}