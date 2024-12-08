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

    void insert(const Point<N>& pt, const ElemType& value);

    std::optional<KDNode&> findNode(std::optional<KDNode&> currNode, const Point<PT, PD>& pt) const;
    std::optional<KDNode&> findNode(const Point<PT, PD>& pt) const;

    void removeRecursively(KDNode* currNode);
    void removeNode(KDNode* currNode);

    std::size_t KDTree<PT, PD>::dimension() const; 

    std::size_t countNodes(KDNode* currNode) const;
    std::size_t countNodes() const;

    size_t getSize_t();
    void setSize_t(size_t tmp);

    ~KDTree();

private:
    std::unique_ptr<KDNode<PT, PD>> root_;
    size_t size_;  

    KDNode<PT, PD>*
    buildTree(std::vector<Point<PT, PD>>::iterator start,
              std::vector<Point<PT, PD>>::iterator end, 
              int level_index);

    void removeRecursively_internalCall(std::optional<KDNode&> currNode)

};

#endif