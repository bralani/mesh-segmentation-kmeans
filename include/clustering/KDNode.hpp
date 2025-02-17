#ifndef KDNODE_HPP
#define KDNODE_HPP

#include <vector>
#include <array>
#include <memory>
#include "geometry/point/Point.hpp"
#include "geometry/point/HasWgtCent.hpp"

/**
 * @class KdNode
 * @brief Represents a node in a kd tree, including bounding box and child pointers.
 * 
 * @tparam PT Type of the point coordinates.
 * @tparam PD Dimensionality of the points.
 */
template <typename PT, std::size_t PD>
class KdNode : public HasWgtCent<PT, PD> {
public:
    std::array<PT, PD> cellMin;         ///< Minimum coordinates of the node's bounding box
    std::array<PT, PD> cellMax;         ///< Maximum coordinates of the node's bounding box
    std::unique_ptr<KdNode<PT, PD>> left = nullptr;  ///< Pointer to the left child node
    std::unique_ptr<KdNode<PT, PD>> right = nullptr; ///< Pointer to the right child node
    std::unique_ptr<Point<PT, PD>> myPoint = nullptr; ///< For the leaves: a pointer to the single point of the node.

    // Default constructor
    KdNode();

    // Constructor with bounding box
    KdNode(const std::array<PT, PD>& min, const std::array<PT, PD>& max);

    // Virtual destructor
    virtual ~KdNode() override;
};

#endif // KDNODE_HPP
