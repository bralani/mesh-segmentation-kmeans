#ifndef KDTREE_HPP
#define KDTREE_HPP

#include "KDNode.hpp"
#include "Point.hpp"
#include <memory>
#include <vector>   

//PT = Point type
//PD = Point Dimensions

template <typename PT, std::size_t PD>
class KDTree {
public:
    KDTree() : root_(nullptr), size_(0) {}
    KDTree(const std::vector<Point<PT, PD>> &points);
private:
    std::unique_ptr<KDNode<PT, PD>> root_;
    size_t size_;  

    KDNode<PT, PD>*
    buildTree(std::vector<Point<PT, PD>>::iterator start,
              std::vector<Point<PT, PD>>::iterator end, 
              int level_index);
};

#endif