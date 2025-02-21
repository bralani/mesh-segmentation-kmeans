#ifndef KD_NODE_HPP
#define KD_NODE_HPP

#include <vector>
#include "geometry/point/Point.hpp"

/**
 * \class KDNode
 * \brief Represents a node in a k-d tree.
 *
 * This class defines a node structure used in a k-dimensional tree (k-d tree) for spatial partitioning.
 * Each node contains a point, left and right children, and the split dimension.
 *
 * \tparam PT Type of the point coordinates (e.g., float, double, etc.).
 * \tparam PD Dimension of the data points.
 */
template <typename PT, std::size_t PD>
class KDNode {
public:
    /**
     * \brief Constructor for KDNode.
     * \param point The point stored in this node.
     * \param splitDimension The dimension used for splitting at this node.
     */
    KDNode(const Point<PT, PD>& point, std::size_t splitDimension);

    /**
     * \brief Destructor for KDNode.
     */
    ~KDNode();

    /**
     * \brief Gets the left child node.
     * \return Pointer to the left child node.
     */
    KDNode* getLeft() const;

    /**
     * \brief Gets the right child node.
     * \return Pointer to the right child node.
     */
    KDNode* getRight() const;

    /**
     * \brief Sets the left child node.
     * \param left Pointer to the left child node.
     */
    void setLeft(KDNode* left);

    /**
     * \brief Sets the right child node.
     * \param right Pointer to the right child node.
     */
    void setRight(KDNode* right);

    /**
     * \brief Gets the point stored in this node.
     * \return Reference to the point stored in the node.
     */
    const Point<PT, PD>& getPoint() const;

    /**
     * \brief Gets the dimension used for splitting at this node.
     * \return The split dimension.
     */
    std::size_t getSplitDimension() const;

private:
    Point<PT, PD> m_point; ///< The point stored in this node.
    std::size_t m_splitDimension; ///< The dimension used for splitting.
    KDNode* m_left = nullptr; ///< Pointer to the left child node.
    KDNode* m_right = nullptr; ///< Pointer to the right child node.
};

#endif // KD_NODE_HPP
