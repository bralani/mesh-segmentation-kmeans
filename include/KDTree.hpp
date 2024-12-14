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

    // Filters the given centers based on the KD-tree structure
    void filter(std::vector<CentroidPoint<PT, PD>>& centers);

private:
    std::unique_ptr<KdNode<PT, PD>> root = nullptr; // Root node of the KD-tree

    // Recursively builds the KD-tree from a set of points
    std::unique_ptr<KdNode<PT, PD>> buildTree(std::vector<Point<PT, PD>>& points, int depth);

    /*
    // Recursive helper for the filter function
    void filterRecursive(std::unique_ptr<KdNode<PT, PD>>& node, std::vector<std::shared_ptr<CentroidPoint<PT, PD>>>& candidates);

    // Finds the closest point to a target from a set of candidates
    Point<PT, PD> findClosestPoint(const std::vector<Point<PT, PD>>& candidates, const Point<PT, PD>& target);

    // Determines if a point `z` is farther from a bounding box than `zStar`
    bool isFarther(const Point<PT, PD>& z, const Point<PT, PD>& zStar, const KdNode<PT, PD>& node);

    // Finds the closest candidate (centroid) to a given point
    CentroidPoint<PT, PD> findClosestCandidate(const std::vector<std::shared_ptr<CentroidPoint<PT, PD>>>& candidates, const Point<PT, PD>& target);
    */
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
    auto node = std::make_unique<KdNode<PT, PD>>();
    node->count = points.size();
    node->wgtCent = Point<PT, PD>::vectorSum(points); // Compute weighted centroid

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
        node->points = points;
        node->wgtCent = points[0];
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


/**
// Filters the given set of centers using the KD-tree
template <typename PT, std::size_t PD>
void KdTree<PT, PD>::filter(std::vector<CentroidPoint<PT, PD>>& centers) {
    std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> centersPointers;

    // Convert centers to shared pointers for in-place modification
    for (CentroidPoint<PT, PD>& z : centers) {
        centersPointers.push_back(std::make_shared<CentroidPoint<PT, PD>>(z));
    }

    // Start the recursive filtering process
    filterRecursive(root, centersPointers);
}

// Finds the closest candidate to a target point
template <typename PT, std::size_t PD>
CentroidPoint<PT, PD> KdTree<PT, PD>::findClosestCandidate(const std::vector<std::shared_ptr<CentroidPoint<PT, PD>>>& candidates, const Point<PT, PD>& target) {
    auto closest = candidates[0];
    double minDist = closest->distanceTo(target);

    for (const auto& candidate : candidates) {
        double dist = candidate->distanceTo(target);
        if (dist < minDist) {
            minDist = dist;
            closest = candidate;
        }
    }
    return *closest;
}

// Recursive function for filtering
template <typename PT, std::size_t PD>
void KdTree<PT, PD>::filterRecursive(std::unique_ptr<KdNode<PT, PD>>& node, std::vector<std::shared_ptr<CentroidPoint<PT, PD>>>& candidates) {
    if (!node) return;

    // Leaf node: find the closest candidate and update it
    if (!node->left && !node->right) {
        CentroidPoint<PT, PD> zStar = findClosestCandidate(candidates, node->wgtCent);
        zStar = zStar + node->wgtCent;
        zStar.count += node->count;
        return;
    } else {
        // Internal node: compute the midpoint of the cell
        Point<PT, PD> cellMidpoint;
        for (std::size_t i = 0; i < PD; ++i) {
            cellMidpoint.getValues()[i] = (node->cellMin[i] + node->cellMax[i]) / PT(2);
        }

        // Find the closest candidate to the cell midpoint
        CentroidPoint<PT, PD> zStar = findClosestCandidate(candidates, cellMidpoint);

        // Filter candidates based on proximity to zStar
        std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> filteredCandidates;
        filteredCandidates.reserve(candidates.size());

        // Recursively filter left and right subtrees
        if (filteredCandidates.size() == 1) {
            *filteredCandidates[0] = *filteredCandidates[0] + node->wgtCent;
            filteredCandidates[0]->count = filteredCandidates[0]->count + node->count;
        } else {
            filterRecursive(node->left, filteredCandidates);
            filterRecursive(node->right, filteredCandidates);
        }
    }
}


// Finds the closest point to the target among a set of candidates
template <typename PT, std::size_t PD>
Point<PT, PD> KdTree<PT, PD>::findClosestPoint(const std::vector<Point<PT, PD>>& candidates, const Point<PT, PD>& target) {
    Point<PT, PD> closest = candidates[0];
    double minDist = closest.distanceTo(target);

    for (const Point<PT, PD>& z : candidates) {
        double dist = z.distanceTo(target);
        if (dist < minDist) {
            minDist = dist;
            closest = z;
        }
    }

    return closest;
}


// Checks if a point z is farther from a bounding box than zStar
template <typename PT, std::size_t PD>
bool KdTree<PT, PD>::isFarther(const Point<PT, PD>& z, const Point<PT, PD>& zStar, const KdNode<PT, PD>& node) {
    Point<PT, PD> u = z - zStar;

    Point<PT, PD> vH;
    for (std::size_t i = 0; i < PD; ++i) {
        vH.getValues()[i] = (u.getValues()[i] > 0) ? node.cellMax[i] : node.cellMin[i];
    }

    double distZ = z.distanceTo(vH);
    double distZStar = zStar.distanceTo(vH);

    return distZ > distZStar;
}
*/


#endif

