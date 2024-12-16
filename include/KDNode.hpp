#ifndef KDNODE_HPP
#define KDNODE_HPP

#include "Point.hpp"
#include <vector>
#include <array>
#include <memory>

/**
 * Class template to represent a node in a KD-Tree
 * 
 * A node in this tree delines a spacial region (a Cell).
 * The spacial region is delimited by cellMin and cellMax which corresponds to the min
 * and max coordinates of any point that can be put in this spatia region-
 * WgtCent is the vector sum of all the points contained in the spatial region
 * 
 * In particular, only the leaves of such three has the attribute myPoint.
 * That's because for leaves the spatial region has cellMin = cellMax = the coordinate of
 * a single point.
 * myPoint is kept as a pointer to be able to assign it a centroid later on.
 * 
 * 
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

    std::unique_ptr<Point<PT,PD>> myPoint = nullptr;

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
