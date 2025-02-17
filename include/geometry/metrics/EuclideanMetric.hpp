#ifndef EUCLIDEANMETRIC_HPP
#define EUCLIDEANMETRIC_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>

#include "clustering/KDTree.hpp"
#include "geometry/metrics/Metric.hpp"
#include "geometry/mesh/Mesh.hpp"

#ifdef USE_CUDA
// Dichiarazione della funzione kernel CUDA (definita in kmeans.cu)
void kmeans_cuda(int K, int dim, int numPoints, float *points, float *centroids, int *cluster_assignment, float threshold);
#endif

#define MIN_NUM_POINTS_CUDA 10000
#define MAX_ITERATIONS 100

/**
 * Euclidean metric: calcola la distanza in linea retta.
 */
template <typename PT, std::size_t PD>
class EuclideanMetric : public Metric<PT, PD>
{
public:
    EuclideanMetric() = default;

    EuclideanMetric(std::vector<Point<PT, PD>> data, double threshold);

    EuclideanMetric(Mesh &mesh, double percentage_threshold, std::vector<Point<PT, PD>> data);

    static PT distanceTo(const Point<PT, PD> &a, const Point<PT, PD> &b);

    void setup() override;
    void fit_cpu() override;

#ifdef USE_CUDA
    void fit_gpu() override;
#endif

    std::vector<Point<PT, PD>> &getPoints() override;

private:
    Mesh *mesh;
    double treshold;
    KdTree<PT, PD> *kdtree;

    void filter();
    void filterRecursive(std::unique_ptr<KdNode<PT, PD>> &node, const std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> &candidates, int depth);
    std::shared_ptr<CentroidPoint<PT, PD>> findClosestCandidate(const std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> &candidates, const Point<PT, PD> &target);
    bool isFarther(const Point<PT, PD> &z, const Point<PT, PD> &zStar, const KdNode<PT, PD> &node);
    void assignCentroid(std::unique_ptr<KdNode<PT, PD>> &node, const std::shared_ptr<CentroidPoint<PT, PD>> &centroid);
    bool checkConvergence(int iter);
    void storeCentroids() override;
    void updateFaceClusters();
};

#endif
