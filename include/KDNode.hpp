#ifndef KDNODE_HPP
#define KDNODE_HPP

#include <memory> 
#include "Point.hpp"

template <typename POINT_TYPE, std::size_t POINT_DIMENSIONS>
class KDNode {
public:
    KDNode() : left_node_(nullptr), right_node_(nullptr) {}

    KDNode(const Point<POINT_TYPE, POINT_DIMENSIONS>& point)
        : point_(point), left_node_(nullptr), right_node_(nullptr) {}


    Point<POINT_TYPE, POINT_DIMENSIONS> point_; 
    std::unique_ptr<KDNode<POINT_TYPE, POINT_DIMENSIONS>> left_node_;
    std::unique_ptr<KDNode<POINT_TYPE, POINT_DIMENSIONS>> right_node_;


    // Getter
    const Point<POINT_TYPE, POINT_DIMENSIONS>& getPoint() const;
    KDNode* getLeftNode() const;
    KDNode* getRightNode() const;

    // Setter
    void setPoint(const Point<POINT_TYPE, POINT_DIMENSIONS>& point);
    void setLeftNode(std::unique_ptr<KDNode<POINT_TYPE, POINT_DIMENSIONS>> leftNode);
    void setRightNode(std::unique_ptr<KDNode<POINT_TYPE, POINT_DIMENSIONS>> rightNode);
    
};

#endif // KDNODE_HPP
