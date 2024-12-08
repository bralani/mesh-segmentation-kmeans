#include "KDNode.hpp"


template <typename POINT_TYPE, std::size_t POINT_DIMENSIONS>
const Point<POINT_TYPE, POINT_DIMENSIONS>& KDNode<POINT_TYPE, POINT_DIMENSIONS>::getPoint() const {
    return point_;
}

template <typename POINT_TYPE, std::size_t POINT_DIMENSIONS>
KDNode<POINT_TYPE, POINT_DIMENSIONS>* KDNode<POINT_TYPE, POINT_DIMENSIONS>::getLeftNode() const {
    return left_node_.get();
}

template <typename POINT_TYPE, std::size_t POINT_DIMENSIONS>
KDNode<POINT_TYPE, POINT_DIMENSIONS>* KDNode<POINT_TYPE, POINT_DIMENSIONS>::getRightNode() const {
    return right_node_.get();
}


template <typename POINT_TYPE, std::size_t POINT_DIMENSIONS>
void KDNode<POINT_TYPE, POINT_DIMENSIONS>::setPoint(const Point<POINT_TYPE, POINT_DIMENSIONS>& point) {
    point_ = point;
}

template <typename POINT_TYPE, std::size_t POINT_DIMENSIONS>
void KDNode<POINT_TYPE, POINT_DIMENSIONS>::setLeftNode(std::unique_ptr<KDNode<POINT_TYPE, POINT_DIMENSIONS>> leftNode) {
    left_node_ = std::move(leftNode);
}

template <typename POINT_TYPE, std::size_t POINT_DIMENSIONS>
void KDNode<POINT_TYPE, POINT_DIMENSIONS>::setRightNode(std::unique_ptr<KDNode<POINT_TYPE, POINT_DIMENSIONS>> rightNode) {
    right_node_ = std::move(rightNode);
}
