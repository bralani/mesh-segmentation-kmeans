#ifndef KDTREE_HPP
#define KDTREE_HPP

#include <vector>
#include <algorithm>
#include <memory>
#include <limits>
#include <omp.h>

#include "clustering/KDNode.hpp"

/**
 * Class template to represent a KD-Tree
 * Template parameters:
 * - PT: The type of the coordinate values (e.g., double, int)
 * - PD: The number of dimensions of the points (e.g., 2 for 2D, 3 for 3D)
 */
template <typename PT, std::size_t PD>
class KdTree {
public:
    // Constructor: builds the tree from a set of points and a given dimensionality
    KdTree(std::vector<Point<PT, PD>>& points);

    // Destructor: deallocates the tree
    virtual ~KdTree() = default;

    // Returns the root node of the KD-tree
    std::unique_ptr<KdNode<PT, PD>>& getRoot();

private:
    std::unique_ptr<KdNode<PT, PD>> root = nullptr; // Root node of the KD-tree

    // Recursively builds the KD-tree using iterators
    std::unique_ptr<KdNode<PT, PD>> buildTree(typename std::vector<Point<PT, PD>>::iterator begin,
                                              typename std::vector<Point<PT, PD>>::iterator end,
                                              int depth);

};


#endif

