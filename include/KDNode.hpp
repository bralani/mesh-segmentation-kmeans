#ifndef KDNODE_HPP
#define KDNODE_HPP

#include "Point.hpp"
#include <vector>
#include <array>
#include <memory>

/**
 * Class template to represent a node in a KD-Tree
 * Template parameters:
 * - PT: The type of the coordinate values (e.g., double, float)
 * - PD: The number of dimensions of the points (e.g., 2 for 2D, 3 for 3D)
 */
template <typename PT, std::size_t PD>
struct KdNode {
    Point<PT, PD> wgtCent;               // wgtCent for the cell (see paper)
    int count = 0;                       // Number of points in the cell
    std::array<PT, PD> cellMin;          // Bounding box minimum coordinates
    std::array<PT, PD> cellMax;          // Bounding box maximum coordinates
    std::unique_ptr<KdNode<PT, PD>> left = nullptr;              // Left child
    std::unique_ptr<KdNode<PT, PD>> right = nullptr;             // Right child


    /**
     * Constructor
     * Initializes the bounding box and the weighted centroid.
     */
    KdNode()
        : cellMin(), cellMax() {
        cellMin.fill(PT(0));
        cellMax.fill(PT(0));
    }

    /**
     * Constructor with initial bounding box values
     * @param min Initial minimum bounding box coordinates
     * @param max Initial maximum bounding box coordinates
     */
    KdNode(const std::array<PT, PD>& min, const std::array<PT, PD>& max)
        :  cellMin(min), cellMax(max) {}

    /**
     * Destructor
     * Recursively deletes child nodes.
     */
    ~KdNode() = default;  // No need for manual deletion with unique_ptr
};

#endif
