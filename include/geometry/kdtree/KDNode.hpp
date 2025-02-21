#ifndef KDNODE_HPP
#define KDNODE_HPP

#include <vector>
#include <array>
#include <memory>
#include "geometry/point/Point.hpp"
#include "geometry/point/HasWgtCent.hpp"

/**
 * \class KdNode
 * \brief Represents a node in a kd-tree, including its bounding box and child pointers.
 * 
 * A kd-tree (k-dimensional tree) is a space-partitioning data structure used 
 * for organizing points in a k-dimensional space. Each node represents a 
 * partitioning of space with a bounding box and may contain a single point 
 * (in leaf nodes).
 * 
 * \tparam PT Type of the point coordinates.
 * \tparam PD Dimensionality of the points.
 */
template <typename PT, std::size_t PD>
class KdNode : public HasWgtCent<PT, PD> {
public:
    /**
     * \brief Minimum coordinates of the node's bounding box.
     * 
     * Defines the lower corner of the bounding box for this kd-tree node.
     */
    std::array<PT, PD> cellMin;

    /**
     * \brief Maximum coordinates of the node's bounding box.
     * 
     * Defines the upper corner of the bounding box for this kd-tree node.
     */
    std::array<PT, PD> cellMax;

    /**
     * \brief Pointer to the left child node.
     * 
     * If this node is not a leaf, it has a left child representing 
     * one of the partitions.
     */
    std::unique_ptr<KdNode<PT, PD>> left = nullptr;

    /**
     * \brief Pointer to the right child node.
     * 
     * If this node is not a leaf, it has a right child representing 
     * the other partition.
     */
    std::unique_ptr<KdNode<PT, PD>> right = nullptr;

    /**
     * \brief Pointer to the single point of this node (only for leaf nodes).
     * 
     * If this node is a leaf, it stores a pointer to the associated point.
     */
    std::unique_ptr<Point<PT, PD>> myPoint = nullptr;

    /**
     * @brief Default constructor.
     * 
     * Initializes an empty kd-tree node without assigned bounds or children.
     */
    KdNode();

    /**
     * \brief Constructor with bounding box.
     * 
     * Initializes the node with given bounding box minimum and maximum coordinates.
     * 
     * \param min The minimum coordinates of the bounding box.
     * \param max The maximum coordinates of the bounding box.
     */
    KdNode(const std::array<PT, PD>& min, const std::array<PT, PD>& max);

    /**
     * \brief Virtual destructor.
     * 
     * Ensures proper cleanup of dynamically allocated child nodes.
     */
    virtual ~KdNode() override;
};

#endif // KDNODE_HPP
