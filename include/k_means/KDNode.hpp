#ifndef KDNODE_HPP
#define KDNODE_HPP

#include <vector>
#include <array>
#include <memory>

#include "point/Point.hpp"
#include "point/HasWgtCent.hpp"

// The KdNode class represents a node in a k-d tree
// It inherits from HasWgtCent, which provides weighted centroid functionality
// Each KdNode contains the following: 
template <typename PT, std::size_t PD>
class KdNode : public HasWgtCent<PT, PD> {
public:
    std::array<PT, PD> cellMin;         // Minimum coordinates of the node's bounding box
    std::array<PT, PD> cellMax;         // Maximum coordinates of the node's bounding box
    std::unique_ptr<KdNode<PT, PD>> left = nullptr;  // Pointer to the left child node
    std::unique_ptr<KdNode<PT, PD>> right = nullptr; // Pointer to the right child node
    std::unique_ptr<Point<PT, PD>> myPoint = nullptr; // For the leaves: a pointer to the single point of the node.

    // Default constructor initializes the bounding box to zero and the centroid data to defaults
    KdNode() : HasWgtCent<PT, PD>(), cellMin(), cellMax() {
        cellMin.fill(0);  // Initialize the cellMin array to zero
        cellMax.fill(0);  // Initialize the cellMax array to zero
    }

    // Constructor that takes minimum and maximum bounds for the node
    // Initializes the KdNode with a given bounding box (min and max coordinates)
    KdNode(const std::array<PT, PD>& min, const std::array<PT, PD>& max)
        : HasWgtCent<PT, PD>(), cellMin(min), cellMax(max) {}

    // Virtual destructor ensures proper cleanup of the KdNode, including the dynamic children
    // The destructor is defaulted as the class uses smart pointers for automatic memory management
    virtual ~KdNode() override = default;
};

#endif
