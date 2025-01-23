#ifndef KDTREE_HPP
#define KDTREE_HPP

#include <vector>
#include <algorithm>
#include <memory>
#include <limits>
#include <omp.h>

#include "geometry/point/Point.hpp"
#include "geometry/point/CentroidPoint.hpp"
#include "clustering/KDNode.hpp"
#include "geometry/metrics/EuclideanMetric.hpp"
#include "geometry/metrics/Metric.hpp"

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
    std::unique_ptr<KdNode<PT, PD>>& getRoot() {
        return root;
    }

    Point<PT, PD>& mostDistantFrom(Point<PT, PD> point,const std::unique_ptr<KdNode<PT, PD>> root, int depth);

private:
    std::unique_ptr<KdNode<PT, PD>> root = nullptr; // Root node of the KD-tree

    // Recursively builds the KD-tree using iterators
    std::unique_ptr<KdNode<PT, PD>> buildTree(typename std::vector<Point<PT, PD>>::iterator begin,
                                              typename std::vector<Point<PT, PD>>::iterator end,
                                              int depth);

    

    Point<PT, PD> findCenter(const std::array<PT, PD>& cellMin, const std::array<PT, PD>& cellMax);
    Point<PT, PD> &checkAlsoOtherSide(Point<PT, PD> point, double actualDistance, std::unique_ptr<KdNode<PT, PD>> root, int depth, Point<PT, PD>& actualPointMostDistant);
    Point<PT, PD> mostDistantPointInNode(Point<PT, PD> point, std::unique_ptr<KdNode<PT, PD>> root);
    PT distanceTo( Point<PT, PD> &a, Point<PT, PD> &b);
};

// Constructor: initializes the KD-tree by building it
template <typename PT, std::size_t PD>
KdTree<PT, PD>::KdTree(std::vector<Point<PT, PD>>& points) {
    #pragma omp parallel
    {
        #pragma omp single
        root = buildTree(points.begin(), points.end(), 0); 
    }
}

// Recursively builds the KD-tree
template <typename PT, std::size_t PD>
std::unique_ptr<KdNode<PT, PD>> KdTree<PT,PD>::buildTree(typename std::vector<Point<PT, PD>>::iterator begin,
                                              typename std::vector<Point<PT, PD>>::iterator end,
                                              int depth){
    if (begin == end) return nullptr; // Base case: no points

    // Allocate a new KD-tree node
    auto node = std::unique_ptr<KdNode<PT, PD>>(new KdNode<PT, PD>());
    size_t count = std::distance(begin, end);
    node->count = count;
    node->wgtCent = Point<PT, PD>::vectorSum(begin, end).coordinates;  

    // Initialize cell bounds
    for (std::size_t i = 0; i < PD; ++i) {
        node->cellMin[i] = std::numeric_limits<PT>::max();
        node->cellMax[i] = std::numeric_limits<PT>::lowest();
        for (auto it = begin; it != end; ++it) {
            node->cellMin[i] = std::min(node->cellMin[i], it->getValues()[i]);
            node->cellMax[i] = std::max(node->cellMax[i], it->getValues()[i]);
        }
    }

    // If there is only one point, store it in the node
    if (count == 1) {
        node->myPoint = std::unique_ptr<Point<PT, PD>>(&(*begin));
        return node;
    }

    // Choose splitting axis
    int axis = depth % PD;

    // Sort points along the chosen axis
    std::sort(begin, end, [axis](const Point<PT, PD>& a, const Point<PT, PD>& b) {
        return a.getValues()[axis] < b.getValues()[axis];
    });

    // Find the median
    auto median = begin + count / 2;


    int max_threads = omp_get_max_threads();
    bool can_parallelize = (depth < std::log2(max_threads) + 1);

    if (can_parallelize) {
        #pragma omp parallel sections
        {
            #pragma omp section 
            node->left = buildTree(begin, median, depth + 1);

            #pragma omp section
            node->right = buildTree(median, end, depth + 1);
        }
    } else {
        node->left = buildTree(begin, median, depth + 1);
        node->right = buildTree(median, end, depth + 1); 
    }

    return node;
}

//Finds the most distance point in the kdtree from point in input
template <typename PT, std::size_t PD>
Point<PT, PD>& KdTree<PT,PD>::mostDistantFrom(Point<PT, PD> point,const std::unique_ptr<KdNode<PT, PD>> root, int depth){
    if (root->myPoint != nullptr) {
        return *(root->myPoint);
    }

    int axis = depth % PD; 
    Point<PT, PD>* tmpMostDistant = nullptr; 
    double realDistance;
    Point<PT, PD> centerA = findCenter(root->left->cellMin, root->left->cellMax);
    Point<PT, PD> centerB = findCenter(root->right->cellMin, root->right->cellMax);
    
    double distanceA = distanceTo(point, centerA);
    double distanceB =distanceTo(point, centerB);

    if (distanceA > distanceB) {
        tmpMostDistant = mostDistantFrom(point, root->left, depth + 1);
        realDistance = distanceTo(*tmpMostDistant, point);
        checkAlsoOtherSide(point, realDistance, root->right, depth, tmpMostDistant);
    } else {
        tmpMostDistant = mostDistantFrom(point, root->right, depth + 1);
        realDistance = distanceTo(*tmpMostDistant, point);
        checkAlsoOtherSide(point, realDistance, root->left, depth, tmpMostDistant);
    }

    return *tmpMostDistant;
}

template <typename PT, std::size_t PD>
Point<PT, PD>& KdTree<PT,PD>::checkAlsoOtherSide(
                    Point<PT, PD> point, 
                    double actualDistance,
                    std::unique_ptr<KdNode<PT, PD>> root, 
                    int depth,
                    Point<PT, PD>& actualPointMostDistant){

    Point<PT,PD> mostDistantPoint = mostDistantPointInNode(point, root);
    double distMax = distanceTo(point, mostDistantPoint);
    
    if(distMax > actualDistance){
        Point<PT,PD>& tmpNewMostDistance = mostDistantFrom(point, root, depth);
        if(actualDistance < distanceTo(tmpNewMostDistance, point))
            (*actualPointMostDistant) = tmpNewMostDistance;
    }
}


template <typename PT, std::size_t PD>
Point<PT, PD> KdTree<PT,PD>::mostDistantPointInNode(Point<PT, PD> point, std::unique_ptr<KdNode<PT, PD>> root){
    Point<PT, PD> mostDistantPoint;

    for (std::size_t d = 0; d < PD; ++d) {
        if( abs((root.cellMin[d] - point.coordinates[d])) < abs((root.cellMax[d] - point.coordinates[d])) )
            mostDistantPoint.coordinates[d] = root.cellMax[d]; 
        else
            mostDistantPoint.coordinates[d] = root.cellMin[d];
    }

    return mostDistantPoint;
}

template <typename PT, std::size_t PD>
Point<PT, PD> KdTree<PT, PD>::findCenter(const std::array<PT, PD>& cellMin, const std::array<PT, PD>& cellMax) {
    Point<PT, PD> point;

    for (std::size_t i = 0; i < PD; i++) {
        point.coordinates[i] = cellMin[i] + (cellMax[i] - cellMin[i]) / 2.0;
    }

    return point;
}


template <typename PT, std::size_t PD>
PT KdTree<PT, PD>::distanceTo( Point<PT, PD>& a,  Point<PT, PD>& b) {
    if (a.coordinates.size() != b.coordinates.size()) {
        throw std::invalid_argument("Points must have the same dimensionality");
    }
    PT sum = 0;
    for (std::size_t i = 0; i < a.coordinates.size(); ++i) {
        sum += std::pow(a.coordinates[i] - b.coordinates[i], 2);
    }
    return std::sqrt(sum);
}


#endif

