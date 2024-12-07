#include "KDTree.hpp"

template <typename PT, std::size_t PD>
KDNode<PT, PD>* KDTree<PT, PD>::buildTree(typename std::vector<Point<PT, PD>>::iterator start,
                                           typename std::vector<Point<PT, PD>>::iterator end, 
                                           int level_index) {
    if (start >= end) 
        return nullptr;

    int axis = level_index % PD;

    auto points_lenght = end - start;
    auto middle = start + points_lenght / 2;

    auto compare_lambda = [axis](const Point<PT, PD> &p1,
                                 const Point<PT, PD> &p2) {
        return p1[axis] < p2[axis];
    };

    std::nth_element(start, middle, end, compare_lambda); 
    
    //...//

    KDNode<PT, PD>* new_node = new KDNode<PT, PD>(*middle);
    new_node->left_node_ = buildTree(start, middle, level_index + 1);
    new_node->right_node_ = buildTree(middle + 1, end, level_index + 1);
    return new_node;        
}

template <typename PT, std::size_t PD>
KDTree<PT, PD>::KDTree(const std::vector<Point<PT, PD>> &points) {
    root_ = buildTree(points.begin(), points.end(), 0);
    size_ = points.size();       
}
