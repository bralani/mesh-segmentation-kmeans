#ifndef KDTREE_HPP
#define KDTREE_HPP

#include <vector>
#include <algorithm>
#include <memory>
#include <limits>
#include <omp.h>

#include "geometry/kdtree/KDNode.hpp"

/**
 * \class KdTree
 * \brief A k-dimensional tree (kd-tree) for organizing and searching spatial data.
 * 
 * The kd-tree is a space-partitioning data structure used for organizing points in a k-dimensional space.
 * It is useful for efficient nearest-neighbor searches, range searches, and clustering.
 * 
 * \tparam PT The type of the coordinate values (e.g., double, int).
 * \tparam PD The number of dimensions of the points (e.g., 2 for 2D, 3 for 3D).
 */
template <typename PT, std::size_t PD>
class KdTree {
public:
    /**
     * \brief Constructs a KD-tree from a given set of points.
     * 
     * This constructor initializes the tree by recursively partitioning the input points.
     * 
     * \param points A reference to a vector of points to be organized into the tree.
     */
    KdTree(std::vector<Point<PT, PD>>& points);

    /**
     * \brief Destructor.
     * 
     * Ensures proper cleanup of dynamically allocated nodes when the KdTree object is destroyed.
     */
    ~KdTree();

    /**
     * \brief Returns a reference to the root node of the kd-tree.
     * 
     * The root node provides access to the entire tree structure.
     * 
     * \return A unique pointer to the root KdNode.
     */
    std::unique_ptr<KdNode<PT, PD>>& getRoot();

private:
    std::unique_ptr<KdNode<PT, PD>> root = nullptr; ///< Root node of the KD-tree.

    /**
     * \brief Recursively builds the KD-tree from a subset of points.
     * 
     * The function partitions the points along a selected dimension and creates child nodes recursively.
     * 
     * \param begin Iterator pointing to the beginning of the subset.
     * \param end Iterator pointing to the end of the subset.
     * \param depth Current depth in the tree (used to determine the splitting dimension).
     * \return A unique pointer to the constructed KdNode.
     */
    std::unique_ptr<KdNode<PT, PD>> buildTree(typename std::vector<Point<PT, PD>>::iterator begin,
                                              typename std::vector<Point<PT, PD>>::iterator end,
                                              int depth);

    /**
     * \brief Clears the KD-tree by recursively deleting all nodes.
     * 
     * The function traverses the tree in post-order and deletes each node.
     * 
     * \param node Reference to the root node of the subtree to be cleared.
     */
    void clearTree(std::unique_ptr<KdNode<PT, PD>>& node);

};

#endif // KDTREE_HPP
