#ifndef KDTREE_PP
#define KDTREE_HPP

#include "KDNode.hpp"
#include "Point2.hpp"
#include "CentroidPoint.hpp"
#include <vector>
#include <algorithm>

/**
 * Points to discuss:
 * - Should Point be templated with TYPE + DIMENSION?
 */


/**
 * Point deve implementare:
 * Somma
 * Sottrazione
 * un metodo statico per fare la vectorSum di un vettore di punti
 * Avere una classa figlia CentroidPoint 
 */

/**
 * CentroidPoint deve ereditara da Point implementare:
 * un attributo Count
 * un costruttore che prenda in ingresso un Point
 */

class KdTree {
public:
    // Constructor: builds the tree from a set of points and a given dimensionality
    KdTree(const std::vector<Point>& points, int dimension);

    // Destructor: deallocates the tree
    ~KdTree();

    // Filters the given centers based on the KD-tree structure
    void filter(std::vector<CentroidPoint>& centers);

private:
    KdNode* root = nullptr; // Root node of the KD-tree
    int dimension;          // Dimensionality of the points

    // Recursively builds the KD-tree from a set of points
    KdNode* buildTree(std::vector<Point>& points, int depth);

    // Recursive helper for the filter function
    void filterRecursive(KdNode* node, std::vector<CentroidPoint*> &candidates);

    // Finds the closest point to a target from a set of candidates
    Point findClosestPoint(const std::vector<Point>& candidates, const Point& target);

    // Determines if a point `z` is farther from a bounding box than `zStar`
    bool isFarther(const Point& z, const Point& zStar, const KdNode& node);
};

// Constructor: initializes the KD-tree by building it
KdTree::KdTree(const std::vector<Point>& points, int dimension) : dimension(dimension) {
    std::vector<Point> pointsCopy = points; // Create a local copy of the points
    root = buildTree(pointsCopy, 0);        // Build the tree starting at depth 0
}

// Recursively builds the KD-tree
KdNode* KdTree::buildTree(std::vector<Point>& points, int depth) {
    if (points.empty()) return nullptr;

    // Allocate a new KD-tree node
    KdNode* node = new KdNode(dimension);
    node->count = points.size();
    node->wgtCent = Point::VectorSum(points); // Compute weighted centroid, c.wgtCent

    // Initialize cell bounds
    for (int i = 0; i < dimension; ++i) {
        node->cellMin[i] = std::numeric_limits<double>::max();
        node->cellMax[i] = std::numeric_limits<double>::lowest();
        for (const Point& p : points) {
            node->cellMin[i] = std::min(node->cellMin[i], p.coordinates[i]);
            node->cellMax[i] = std::max(node->cellMax[i], p.coordinates[i]);
        }
    }

    // If there is only one point, store it in the node
    if (points.size() == 1) {
        node->points = points;
        node->wgtCent = points[0]; //z.wgtCent
        return node;
    }

    // Choose the splitting axis based on the depth
    int axis = depth % dimension;

    // Sort points along the chosen axis
    std::sort(points.begin(), points.end(),
              [axis](const Point& a, const Point& b) {
                  return a.coordinates[axis] < b.coordinates[axis];
              });

    // Split points into left and right subsets
    // New vectors with copies of the points 
    size_t medianIndex = points.size() / 2;
    std::vector<Point> leftPoints(points.begin(), points.begin() + medianIndex);
    std::vector<Point> rightPoints(points.begin() + medianIndex, points.end());

    // Recursively build the left and right subtrees
    node->left = buildTree(leftPoints, depth + 1);
    node->right = buildTree(rightPoints, depth + 1);

    return node;
}

// Destructor: deallocate the KD-tree nodes
KdTree::~KdTree() {
    delete root;
}

// Filters the given set of centers using the KD-tree
void KdTree::filter(std::vector<CentroidPoint>& centers) {
    std::vector<CentroidPoint*> centersPointers;

    // Convert centers to pointers for in-place modification
    for (CentroidPoint& z : centers) {
        centersPointers.push_back(&z); //Take the address of each candidate centers with &
    }

    // Start the recursive filtering process
    filterRecursive(root, centersPointers);
}

// Recursive function for filtering. References to a vector of pointers of CentroidPoint
void KdTree::filterRecursive(KdNode* node, std::vector<CentroidPoint*> &candidates) {
    if (!node) return;

    // Leaf node: find the closest candidate and update it, if (|Z| = 1)
    if (!node->left && !node->right) {
        // z* ← the closest point in Z to u.point;
        // z*.wgtCent ← z*.wgtCent + u.point;
        // z*.count ← z*.count + 1;
        Point zStar = *candidates[0];  //Select first of remain candidates
        //Find the closest 
        for (int i = 1; i < candidates.size(); i++) {
            double minDist = zStar.distanceTo(node->wgtCent);
            double dist = (*candidates[i]).distanceTo(node->wgtCent);
            if (dist < minDist) {
                minDist = dist;
                zStar = *candidates[i];
            }
        }
        *candidates[0] = *candidates[0] + node->wgtCent;
        candidates[0]->count = candidates[0]->count + node->count;

    } else {
        // z* ← the closest point in Z to C’s midpoint;
        // for each (z ∈ Z \ {z*})
        //      if (z.isFarther(z*, C)) Z ← Z \ {z};

        // Internal node: compute the midpoint of the cell
        Point cellMidpoint(dimension);
        for (int i = 0; i < dimension; ++i)
            cellMidpoint.coordinates[i] = (node->cellMin[i] + node->cellMax[i]) / 2.0;

        // Find the closest candidate to the cell midpoint
        Point zStar = *candidates[0];
        for (int i = 1; i < candidates.size(); i++) {
            double minDist = zStar.distanceTo(cellMidpoint);
            double dist = (*candidates[i]).distanceTo(cellMidpoint);
            if (dist < minDist) {
                minDist = dist;
                zStar = *candidates[i];
            }
        }

        // Filter candidates based on proximity to zStar
        // New vector with address of candidates
        std::vector<CentroidPoint*> filteredCandidates;
        for (CentroidPoint*& z : candidates) {
            if (!isFarther(*z, zStar, *node)) {
                filteredCandidates.push_back(z);
            }
        }

        // Recursively filter left and right subtrees
        if (filteredCandidates.size() == 1) {
            // z*.wgtCent ← z*.wgtCent + u.wgtCent;
            // z*.count ← z*.count + u.count;
            *filteredCandidates[0] = *filteredCandidates[0] + node->wgtCent;
            filteredCandidates[0]->count = filteredCandidates[0]->count + node->count;
        } else {
            // Filter(u.left, Z);
            // Filter(u.right, Z);
            filterRecursive(node->left, filteredCandidates);
            filterRecursive(node->right, filteredCandidates);
        }
    }
}

// Finds the closest point to the target among a set of candidates
Point KdTree::findClosestPoint(const std::vector<Point>& candidates, const Point& target) {
    Point closest = candidates[0];
    double minDist = closest.distanceTo(target);

    for (const Point& z : candidates) {
        double dist = z.distanceTo(target);
        if (dist < minDist) {
            minDist = dist;
            closest = z;
        }
    }

    return closest;
}

// Checks if a point z is farther from a bounding box than zStar
bool KdTree::isFarther(const Point& z, const Point& zStar, const KdNode& node) {
    Point u(dimension);
    u = z - zStar;

    Point vH(dimension);
    for (size_t i = 0; i < dimension; ++i) {
        double coordinate = (u.coordinates[i] > 0) ? node.cellMax[i] : node.cellMin[i];
        vH.coordinates[i] = coordinate;
    }

    double distZ = z.distanceTo(vH);
    double distZStar = zStar.distanceTo(vH);

    return distZ > distZStar;
}

#endif
