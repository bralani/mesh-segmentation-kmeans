#ifndef KDTREE_HPP
#define KDTREE_HPP

#include "Point2.hpp"
#include <memory>
#include <vector>   
#include <ctime>   
#include <unordered_set>
#include <algorithm>
#include <iostream>
//PT = Point type
//PD = Point Dimensions

template <typename PT, std::size_t PD>
class KDTree {
public:
    KDTree() : root_(nullptr), size_(0) {}
    KDTree(std::vector<Point<PT, PD>> points){
        root_ = buildTree(points.begin(), points.end(), 0, NULL);
        size_ = points.size();      

        root_->max_dim_coordinates_ = find_max_coordinates(points.begin(), points.end());
        root_->min_dim_coordinates_ = find_min_coordinates(points.begin(), points.end());
        setup_dimensions(root_,0);
    }

    void testFindNode(const Point<PT, PD>& pt){
        void* pp;
        pp = this->findNode(pt);
        std::cout<<"Get it"<<std::endl;
        pp = NULL;
    }

    std::size_t countNodes(const typename KDTree<PT, PD>::KDNode *currNode) const ;
    std::size_t countNodes() const ;

    size_t getSize_t();
    void setSize_t(size_t tmp);

    std::size_t dimension() const;

private:

    struct KDNode{
        KDNode(){}

        KDNode(const Point<PT, PD>& point, int level_index)
            : point_(point), level_(level_index) {}


        Point<PT, PD> point_; 
        /** Cordinates to keep track of the limits of the C cell of the node */
        std::array<PT, PD> min_dim_coordinates_;
        std::array<PT, PD> max_dim_coordinates_;

        KDNode* left_node_;
        KDNode* right_node_;
        KDNode* parent_node_;

        int level_;

        /** Candidate centers */
        std::vector<Point<PT,PD>*> candidates_;

        const Point<PT, PD>& getPoint() const;
        void setPoint(const Point<PT, PD>& point);

        KDNode* getLeftNode() const;
        void setLeftNode(KDNode* leftNode);

        KDNode* getRightNode() const;
        void setRightNode(KDNode* rightNode);

    };

    KDNode* root_;
    size_t size_;  

    /**
     * Creates a KdTree recursively
     */
    KDNode*
    buildTree(const std::vector<Point<PT, PD>>::iterator& start,
              const std::vector<Point<PT, PD>>::iterator& end, 
              int level_index,
              KDNode* parent)
    {
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


        KDNode* new_node = new KDNode(*middle, level_index);
        new_node->parent_node_ = parent;
        new_node->left_node_ = buildTree(start, middle, level_index + 1, new_node);
        new_node->right_node_ = buildTree(middle + 1, end, level_index + 1, new_node);
        return new_node;
    }
    
    /**
     * Traverse the tree and sets up the C cells limits
     */
    void setup_dimensions(KDNode* node, int axis) {
        if (node == nullptr) {
            return;
        }
        else if (node->parent_node_ != nullptr) {

            // Copies the C cells coordinates of the parent
            node->max_dim_coordinates_ = node->parent_node_->max_dim_coordinates_;
            node->min_dim_coordinates_ = node->parent_node_->min_dim_coordinates_;

            // Change the single coordinates due to the split
            int previous_axis = (axis - 1 + PD) % PD;
            if (node == node->parent_node_->left_node_) {
                node->max_dim_coordinates_[previous_axis] = node->parent_node_->point_[previous_axis];
            } else if (node == node->parent_node_->right_node_) {
                node->min_dim_coordinates_[previous_axis] = node->parent_node_->point_[previous_axis];
            }
        }

        // Chiamate ricorsive per i figli
        if (node->left_node_ != nullptr) {
            setup_dimensions(node->left_node_, (axis + 1) % PD);
        }
        if (node->right_node_ != nullptr) {
            setup_dimensions(node->right_node_, (axis + 1) % PD);
        }
    }


    /*
    
     */

    std::array<PT, PD> find_max_coordinates(const typename std::vector<Point<PT, PD>>::iterator& start,
                                            const typename std::vector<Point<PT, PD>>::iterator& end) {
        // Array per memorizzare le coordinate massime
        std::array<PT, PD> max_coordinates;

        // Inizializza il massimo con i valori del primo punto nell'intervallo
        for (std::size_t dim = 0; dim < PD; ++dim) {
            max_coordinates[dim] = (*start)[dim];
        }

        // Scansiona i punti nell'intervallo
        for (auto it = start; it != end; ++it) {
            for (std::size_t dim = 0; dim < PD; ++dim) {
                if ((*it)[dim] > max_coordinates[dim]) {
                    max_coordinates[dim] = (*it)[dim];
                }
            }
        }

        return max_coordinates;
    }

    std::array<PT, PD> find_min_coordinates(const typename std::vector<Point<PT, PD>>::iterator& start,
                                            const typename std::vector<Point<PT, PD>>::iterator& end) {
        // Array per memorizzare le coordinate minime
        std::array<PT, PD> min_coordinates;

        // Inizializza il minimo con i valori del primo punto nell'intervallo
        for (std::size_t dim = 0; dim < PD; ++dim) {
            min_coordinates[dim] = (*start)[dim];
        }

        // Scansiona i punti nell'intervallo
        for (auto it = start; it != end; ++it) {
            for (std::size_t dim = 0; dim < PD; ++dim) {
                if ((*it)[dim] < min_coordinates[dim]) {
                    min_coordinates[dim] = (*it)[dim];
                }
            }
        }

        return min_coordinates;
    }


    std::vector<Point<PT, PD>*> to_vector() {
        std::vector<Point<PT, PD>*> points;
        points.reserve(size_);
        collectPoints(root_, points); 
        return points;
    }
    
    void collectPoints(KDNode* node, std::vector<Point<PT, PD>*>& points) {
        if (node == nullptr) return; 
        points.push_back(&(node->point_)); 
        collectPoints(node->left_node_, points); 
        collectPoints(node->right_node_, points); 
    }

    std::vector<Point<PT, PD>*> extract_k_random_points(int k) {
        if (k > size_) {
            throw std::out_of_range("k è maggiore della dimensione dell'albero.");
        }

        std::vector<Point<PT, PD>*> all_points = to_vector(); 
        std::vector<Point<PT, PD>*> random_points;

        std::srand(std::time(nullptr));  
        std::unordered_set<int> selected_indices;

        while (random_points.size() < k) {
            int random_index = std::rand() % all_points.size();
            if (selected_indices.find(random_index) == selected_indices.end()) {
                random_points.push_back(all_points[random_index]);
                selected_indices.insert(random_index); 
            }
        }
        return random_points;
    }
    

    KDNode* findNode(KDNode* currNode, const Point<PT, PD>& pt) const {
        if (currNode == nullptr || currNode->point_ == pt) {
            return currNode;
        }

        const Point<PT, PD>& currPoint = currNode->point_;
        int currLevel = currNode->level_;

        if (pt[currLevel % PD] < currPoint[currLevel % PD]) {
            return (currNode->left_node_ != nullptr) ? findNode(currNode->left_node_, pt) : currNode;
        } else {
            return (currNode->right_node_ != nullptr) ? findNode(currNode->right_node_, pt) : currNode;
        }
    }


    KDNode* findNode(const Point<PT, PD>& pt) const {
        return findNode(root_, pt);
    }


};

template <typename PT, std::size_t PD>
void KDTree<PT, PD>::setSize_t(size_t tmp){
    size_ = tmp;
}

template <typename PT, std::size_t PD>
size_t KDTree<PT, PD>::getSize_t(){
    return size_;
}

template <typename PT, std::size_t PD>
inline std::size_t KDTree<PT, PD>::countNodes(const typename KDTree<PT, PD>::KDNode *currNode) const{
    if(currNode != nullptr)
        return 1 + countNodes(currNode->left_node_) + countNodes(currNode->right_node_) ;
    return 0;
}

template <typename PT, std::size_t PD>
inline std::size_t KDTree<PT, PD>::countNodes() const{
    return countNodes(root_);
}

template <typename PT, std::size_t PD>
std::size_t KDTree<PT, PD>::dimension() const {
        return PD;
}

// Getter per `point_`
template <typename PT, std::size_t PD>
const Point<PT, PD>& KDTree<PT, PD>::KDNode::getPoint() const {
    return point_;
}

// Setter per `point_`
template <typename PT, std::size_t PD>
void KDTree<PT, PD>::KDNode::setPoint(const Point<PT, PD>& point) {
    point_ = point;
}

// Getter per `left_node_`
template <typename PT, std::size_t PD>
typename KDTree<PT, PD>::KDNode* KDTree<PT, PD>::KDNode::getLeftNode() const {
    return left_node_;
}

// Setter per `left_node_`
template <typename PT, std::size_t PD>
void KDTree<PT, PD>::KDNode::setLeftNode(KDNode* leftNode) {
    left_node_ = leftNode;
}

// Getter per `right_node_`
template <typename PT, std::size_t PD>
typename KDTree<PT, PD>::KDNode* KDTree<PT, PD>::KDNode::getRightNode() const {
    return right_node_;
}

// Setter per `right_node_`
template <typename PT, std::size_t PD>
void KDTree<PT, PD>::KDNode::setRightNode(KDNode* rightNode) {
    right_node_ = rightNode;
}

#endif