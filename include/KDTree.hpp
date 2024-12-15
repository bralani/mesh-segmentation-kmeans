#ifndef KDTREE_HPP
#define KDTREE_HPP

#include "KDNode.hpp"
#include "Point.hpp"
#include "CentroidPoint.hpp"
#include <vector>
#include <algorithm>
#include <memory>
#include <limits>

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
    KdTree(const std::vector<Point<PT, PD>>& points);

    // Destructor: deallocates the tree
    ~KdTree() = default;

    // Returns the root node of the KD-tree
    std::unique_ptr<KdNode<PT, PD>>& getRoot() {
        return root;
    }

private:
    std::unique_ptr<KdNode<PT, PD>> root = nullptr; // Root node of the KD-tree

    // Recursively builds the KD-tree from a set of points
    std::unique_ptr<KdNode<PT, PD>> buildTree(std::vector<Point<PT, PD>>& points, int depth);
};

// Constructor: initializes the KD-tree by building it
template <typename PT, std::size_t PD>
KdTree<PT, PD>::KdTree(const std::vector<Point<PT, PD>>& points) {
    std::vector<Point<PT, PD>> pointsCopy = points; // Create a local copy of the points
    root = buildTree(pointsCopy, 0);               // Build the tree starting at depth 0
}

// Recursively builds the KD-tree
template <typename PT, std::size_t PD>
std::unique_ptr<KdNode<PT, PD>> KdTree<PT, PD>::buildTree(std::vector<Point<PT, PD>>& points, int depth) {
    if (points.empty()) return nullptr;

    // Allocate a new KD-tree node
    auto node = std::unique_ptr<KdNode<PT, PD>>(new KdNode<PT, PD>());
    node->count = points.size();
    node->wgtCent = Point<PT, PD>::vectorSum(points); 
    node->wgtCent = node->wgtCent / node->count; 

    // Initialize cell bounds
    for (std::size_t i = 0; i < PD; ++i) {
        node->cellMin[i] = std::numeric_limits<PT>::max();
        node->cellMax[i] = std::numeric_limits<PT>::lowest();
        for (const Point<PT, PD>& p : points) {
            node->cellMin[i] = std::min(node->cellMin[i], p.getValues()[i]);
            node->cellMax[i] = std::max(node->cellMax[i], p.getValues()[i]);
        }
    }

    // If there is only one point, store it in the node
    if (points.size() == 1) {
        return node;
    }

    // Choose the splitting axis based on the depth
    int axis = depth % PD;

    // Sort points along the chosen axis
    std::sort(points.begin(), points.end(),
              [axis](const Point<PT, PD>& a, const Point<PT, PD>& b) {
                  return a.getValues()[axis] < b.getValues()[axis];
              });

    // Split points into left and right subsets
    size_t medianIndex = points.size() / 2;
    std::vector<Point<PT, PD>> leftPoints(points.begin(), points.begin() + medianIndex);
    std::vector<Point<PT, PD>> rightPoints(points.begin() + medianIndex, points.end());

    // Recursively build the left and right subtrees
    node->left = std::move(buildTree(leftPoints, depth + 1));
    node->right = std::move(buildTree(rightPoints, depth + 1));

    return node;
}

#endif

