#include "clustering/KDNode.hpp"

// Default constructor
template <typename PT, std::size_t PD>
KdNode<PT, PD>::KdNode()
    : HasWgtCent<PT, PD>(), cellMin(), cellMax()
{
    cellMin.fill(0); // Initialize the cellMin array to zero
    cellMax.fill(0); // Initialize the cellMax array to zero
}

// Constructor with bounding box
template <typename PT, std::size_t PD>
KdNode<PT, PD>::KdNode(const std::array<PT, PD> &min, const std::array<PT, PD> &max)
    : HasWgtCent<PT, PD>(), cellMin(min), cellMax(max) {}

// Virtual destructor (defaulted)
template <typename PT, std::size_t PD>
KdNode<PT, PD>::~KdNode() = default;

// Explicit instantiations
template class KdNode<double, 2>; // 2D nodes with double coordinates
template class KdNode<double, 3>; // 3D nodes with double coordinates
