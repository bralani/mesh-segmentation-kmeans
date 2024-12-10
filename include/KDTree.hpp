#ifndef KDTREE_HPP
#define KDTREE_HPP

#include "Point.hpp"
#include <memory>
#include <vector>   
#include <ctime>   
#include <unordered_set>
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

    /*void insert(const Point<N>& pt, const ElemType& value);

    std::optional<KDNode&> findNode(std::optional<KDNode&> currNode, const Point<PT, PD>& pt) const;
    std::optional<KDNode&> findNode(const Point<PT, PD>& pt) const;

    void removeRecursively(KDNode* currNode);
    void removeNode(KDNode* currNode);

    std::size_t KDTree<PT, PD>::dimension() const; 

    std::size_t countNodes(KDNode* currNode) const;
    std::size_t countNodes() const;

    size_t getSize_t();
    void setSize_t(size_t tmp);

    ~KDTree();*/

private:

    struct KDNode{
        KDNode(){}

        KDNode(const Point<PT, PD>& point)
            : point_(point) {}


        Point<PT, PD> point_; 
        /** Cordinates to keep track of the limits of the C cell of the node */
        std::array<PT, PD> min_dim_coordinates_;
        std::array<PT, PD> max_dim_coordinates_;

        KDNode* left_node_;
        KDNode* right_node_;
        KDNode* parent_node_;

        /** Candidate centers */
        std::vector<Point<PT,PD>*> candidates_;

        /*// Getter
        const Point<POINT_TYPE, POINT_DIMENSIONS>& getPoint() const {
            return point_;
        }
        KDNode* getLeftNode() const{
            return left_node_.get();
        }
        KDNode* getRightNode() const{
            return right_node_.get();
        }

        // Setter
        void setPoint(const Point<PT, PD>& point){
            point_ = point;
        }
        void setLeftNode(std::unique_ptr<KDNode<PT, PD>> leftNode){
            left_node_ = std::move(leftNode);
        }
        void setRightNode(std::unique_ptr<KDNode<PT, PD>> rightNode){
            right_node_ = std::move(rightNode);
        }*/
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


        KDNode* new_node = new KDNode(*middle);
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


    //void removeRecursively_internalCall(std::optional<KDNode&> currNode)
    /*
    
    template <typename PT, std::size_t PD>
    std::optional<KDNode&> KDTree<PT, PD>::findNode(std::optional<KDNode&> currNode, const Point<PT, PD>& pt) const {
        if (!currNode.has_value() || currNode->point == pt) 
            return currNode;

        const Point<PT, PD>& currPoint = currNode->point;
        int currLevel = currNode->level;

        if (pt[currLevel % PD] < currPoint[currLevel % PD]) {
            return currNode->left.has_value() ? findNode(currNode->left, pt) : currNode;
        } else {
            return currNode->right.has_value() ? findNode(currNode->right, pt) : currNode;
        }
    }

    template <typename PT, std::size_t PD>
    std::optional<KDNode&> KDTree<PT, PD>::findNode(const Point<PT, PD>& pt) const {
        return findNode(root_, pt);
    }

    template <typename PT, std::size_t PD>
    void KDTree<PT, PD>::removeRecursively_internalCall(KDNode* currNode) {
        if (currNode == nullptr) 
            return;

        removeRecursively_internalCall(currNode->left);
        removeRecursively_internalCall(currNode->right);

        delete currNode;
    }


    template <typename PT, std::size_t PD>
    void KDTree<PT, PD>::removeRecursively(KDNode* currNode) {
        removeRecursively_internalCall(currNode);
        size_t tmp = countNodes(root_); 
        if (getSize_t() != tmp)
            setSize_t(tmp);
    }



    template <typename PT, std::size_t PD>
    void KDTree<PT, PD>::removeNode(std::optional<KDNode&> currNode){
        //Non so bene come svilupparlo, se rimuovo un singolo nodo dovrei ricreare tutti i piani
        
    }

    template <typename PT, std::size_t PD>
    KDTree<PT, PD>::~KDTree() {
        freeResource(root_);
    }

    template <typename PT, std::size_t PD>
    std::size_t KDTree<PT, PD>::dimension() const {
        return PD;
    }

    template <typename PT, std::size_t PD>
    std::size_t KDTree<PT, PD>::countNodes(const KDNode* currNode) const {
        if (currNode == nullptr) {
            return 0;
        }
        return 1 + countNodes(currNode->left) + countNodes(currNode->right);
    }


    template <typename PT, std::size_t PD>
    std::size_t KDTree<PT, PD>::countNodes() const {
        return countNodes(root_);
    }

    template <typename PT, std::size_t PD>
    void KDTree<PT, PD>::setSize_t(size_t tmp){
        size_ = tmp;
    }

    template <typename PT, std::size_t PD>
    size_t KDTree<PT, PD>::getSize_t(){
        return size_;
    }

    template <std::size_t N, typename ElemType>
    void KDTree<N, ElemType>::insert(const Point<N>& pt, const ElemType& value) {
        // Caso 1: L'albero è vuoto
        if (root_ == nullptr) { 
            root_ = new KDNode(pt, value, 0); 
            size_ = 1;
            return;
        }

        // Caso 2: Cerca la posizione corretta per il nodo
        KDNode* current = root_;
        int level = 0;

        while (true) {
            if (current->point == pt) {
                current->value = value;
                return;
            }

            // Determina l'asse corrente
            int axis = level % N;

            if (pt[axis] < current->point[axis]) {
                // Inserisci a sinistra
                if (current->left == nullptr) {
                    current->left = new KDNode(pt, value, level + 1);
                    ++size_;
                    return;
                } else {
                    current = current->left;
                }
            } else {
                // Inserisci a destra
                if (current->right == nullptr) {
                    current->right = new KDNode(pt, value, level + 1);
                    ++size_;
                    return;
                } else {
                    current = current->right;
                }
            }

            ++level;
        }
    }

    */
};

#endif