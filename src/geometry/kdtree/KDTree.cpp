#include "geometry/kdtree/KDTree.hpp"

// Constructor: initializes the KD-tree by building it
template <typename PT, std::size_t PD>
KdTree<PT, PD>::KdTree(std::vector<Point<PT, PD>> &points)
{
    root = buildTree(points.begin(), points.end(), 0);
}

// Recursively builds the KD-tree
template <typename PT, std::size_t PD>
std::unique_ptr<KdNode<PT, PD>> KdTree<PT, PD>::buildTree(typename std::vector<Point<PT, PD>>::iterator begin,
                                                          typename std::vector<Point<PT, PD>>::iterator end,
                                                          int depth)
{
    if (begin == end)
        return nullptr; // Base case: no points

    // Allocate a new KD-tree node safely
    auto node = std::make_unique<KdNode<PT, PD>>();
    size_t count = std::distance(begin, end);
    node->count = count;
    node->wgtCent = Point<PT, PD>::vectorSum(begin, end).coordinates;

    // Initialize cell bounds
    for (std::size_t i = 0; i < PD; ++i)
    {
        node->cellMin[i] = std::numeric_limits<PT>::max();
        node->cellMax[i] = std::numeric_limits<PT>::lowest();
        for (auto it = begin; it != end; ++it)
        {
            node->cellMin[i] = std::min(node->cellMin[i], it->getValues()[i]);
            node->cellMax[i] = std::max(node->cellMax[i], it->getValues()[i]);
        }
    }

    // If there is only one point, store it in the node
    if (count == 1)
    {
        node->myPoint = std::make_unique<Point<PT, PD>>(*begin);
        return node;
    }

    // Choose splitting axis
    int axis = depth % PD;

    // Efficiently find the median
    auto median = begin + count / 2;
    std::nth_element(begin, median, end, [axis](const Point<PT, PD> &a, const Point<PT, PD> &b)
                     { return a.getValues()[axis] < b.getValues()[axis]; });

    // Determine if parallel execution is possible
    int max_threads = omp_get_max_threads();
    bool can_parallelize = (depth < std::log2(max_threads));

    if (can_parallelize)
    {
#pragma omp parallel sections
        {
#pragma omp section
            node->left = buildTree(begin, median, depth + 1);

#pragma omp section
            node->right = buildTree(median, end, depth + 1);
        }
    }
    else
    {
        node->left = buildTree(begin, median, depth + 1);
        node->right = buildTree(median, end, depth + 1);
    }

    return node;
}

template <typename PT, std::size_t PD>
std::unique_ptr<KdNode<PT, PD>> &KdTree<PT, PD>::getRoot()
{
    return root;
}

// Explicit instantiation for supported types
template class KdTree<double, 2>;
template class KdTree<double, 3>;