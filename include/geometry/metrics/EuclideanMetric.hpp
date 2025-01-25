#ifndef EUCLIDEANMETRIC_HPP
#define EUCLIDEANMETRIC_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include "geometry/point/CentroidPoint.hpp"
#include "clustering/KDTree.hpp"


#ifdef USE_CUDA
  // Declaration of the CUDA kernel function (defined in kmeans.cu)
  void kmeans_cuda(int K, int dim, int numPoints, float *points, float *centroids, int *cluster_assignment, float threshold);
#endif

#define MIN_NUM_POINTS_CUDA 10000
#define MAX_ITERATIONS 100

/**
 * Euclidean metric: calculates straight-line distance.
 */
template <typename PT, std::size_t PD>
class EuclideanMetric : public Metric<PT, PD>
{
public:
    EuclideanMetric(std::vector<Point<PT, PD>> &data, double threshold) 
    {
        this->data = &data;
        this->treshold = threshold;

        #ifdef USE_CUDA
            if (data.size() > MIN_NUM_POINTS_CUDA) {
                // In cuda mode, the kdtree is not used
                kdtree = nullptr;
            } else {
                kdtree = new KdTree<PT, PD>(data);
            }
        #else
            kdtree = new KdTree<PT, PD>(data);
        #endif
    }

    static PT distanceTo(const Point<PT, PD> &a, const Point<PT, PD> &b)
    {
        if (a.coordinates.size() != b.coordinates.size())
        {
            throw std::invalid_argument("Points must have the same dimensionality");
        }
        PT sum = 0;
        for (size_t i = 0; i < a.coordinates.size(); ++i)
        {
            sum += std::pow(a.coordinates[i] - b.coordinates[i], 2);
        }
        return std::sqrt(sum);
    }

    void setup() override
    {
        // Do nothing
    }
    
    void fit_cpu() override {

        bool convergence = false;
        int iter = 0;
        while (!convergence)
        { 
            filter();
            convergence = checkConvergence(iter);
            this->oldCentroids = *this->centroids;
            setup();
            iter++;
        }
    }

    #ifdef USE_CUDA
    /** Kmeans on GPU */
    void fit_gpu() override {

        int numClusters = this->centroids->size();
    
        // Convert the data to a flat array
        float *data_flat = new float[data->size() * PD];
        #pragma omp parallel for collapse(2)
        for (int i = 0; i < data->size(); i++)
        {
            for (int j = 0; j < PD; j++)
            {
            data_flat[i * PD + j] = data->at(i).coordinates[j];
            }
        }

        float *centroids_flat = new float[numClusters * PD];
        #pragma omp parallel for collapse(2)
        for (int i = 0; i < numClusters; i++)
        {
            for (int j = 0; j < PD; j++)
            {
                centroids_flat[i * PD + j] = centroids->at(i).coordinates[j];
            }
        }

        int *cluster_assignment = new int[data->size()];

        // Call the CUDA kernel function
        kmeans_cuda(numClusters, PD, data->size(), data_flat, centroids_flat, cluster_assignment, (float)treshold);

        // Convert the centroids back to the original format
        #pragma omp parallel for collapse(2)
        for (int i = 0; i < numClusters; i++)
        {
            for (int j = 0; j < PD; j++)
            {
                centroids->at(i).coordinates[j] = centroids_flat[i * PD + j];
            }
        #pragma omp parallel for
        for (int i = 0; i < data->size(); i++) 
        {
            std::shared_ptr<CentroidPoint<PT, PD>> centroid_ptr = std::make_shared<CentroidPoint<PT, PD>>(this->centroids->at(cluster_assignment[i]));
            data->at(i).setCentroid(centroid_ptr);
        }
    }
    #endif

private:
    std::vector<Point<PT, PD>> *data;
    double treshold;
    KdTree<PT, PD> *kdtree;                          // Kd-Tree structure

    void filter()
    {
        // Convert centers to shared pointers. During filterRecursive their values will be modified
        std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> centersPointers;
        for (CentroidPoint<PT, PD> &z : *this->centroids)
        {
            z.resetCount();
            centersPointers.push_back(std::shared_ptr<CentroidPoint<PT, PD>>(&z, [](CentroidPoint<PT, PD> *) {}));
        }

        // Start the recursive filtering process
        #pragma omp parallel
        {
            #pragma omp single
            filterRecursive(kdtree->getRoot(), centersPointers, 0);
        }

        /** We need to divide the wgtCent of the centroid for the number of points which has it as centroid */
        for (CentroidPoint<PT, PD> &c : *this->centroids)
        {
            c.normalize();
        }
    }

    /**  Recursive function for filtering. Follow exactly the paper */
    void filterRecursive(std::unique_ptr<KdNode<PT, PD>> &node, const std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> &candidates, int depth)
    {
        if (!node)
            return;

        if (!node->left && !node->right)
        {
            // Leaf node: find the closest candidate and update it
            std::shared_ptr<CentroidPoint<PT, PD>> zStar_ptr = findClosestCandidate(candidates, node->wgtCent);
            *zStar_ptr = *zStar_ptr + *node;

            // The leaf nodes are directly linked to the data we provided. Here we assign each Point its centroid
            node->myPoint->setCentroid(zStar_ptr);
            return;
        }
        else
        {
            // Internal node: compute the midpoint of the cell
            Point<PT, PD> cellMidpoint;
            for (std::size_t i = 0; i < PD; ++i)
            {
                cellMidpoint.setValue((node->cellMin[i] + node->cellMax[i]) / PT(2), i);
            }

            // Find the closest candidate to the cell midpoint
            std::shared_ptr<CentroidPoint<PT, PD>> zStar_ptr = findClosestCandidate(candidates, cellMidpoint);

            // Filter candidates based on proximity to zStar
            std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> filteredCandidates;
            filteredCandidates.reserve(candidates.size());
            for (auto &z : candidates)
            {
                if (z == zStar_ptr)
                {
                    filteredCandidates.push_back(z);
                }
                else if (!isFarther(*z, *zStar_ptr, *node))
                {
                    filteredCandidates.push_back(z);
                }
            }

            int max_threads = omp_get_max_threads();
            bool can_parallelize = (depth < std::log2(max_threads) + 1);

            if (filteredCandidates.size() == 1)
            {
                // If an internal node has a single candidate, just update it and spread the candidate among the subtree
                *filteredCandidates[0] = *filteredCandidates[0] + *node;

                if (can_parallelize)
                {
                    #pragma omp parallel
                    {
                        #pragma omp single
                        {
                            // Create tasks to assign centroids to the left and right subtrees
                            #pragma omp task shared(node, filteredCandidates)
                            assignCentroid(node->left, filteredCandidates[0]);

                            #pragma omp task shared(node, filteredCandidates)
                            assignCentroid(node->right, filteredCandidates[0]);
                        }
                    }
                }
                else
                {
                    assignCentroid(node->left, filteredCandidates[0]);
                    assignCentroid(node->right, filteredCandidates[0]);
                }
            }
            else
            {
                if (can_parallelize)
                {
                    #pragma omp parallel
                    {
                        #pragma omp single
                        {
                            // Create tasks to filter the left and right subtrees recursively
                            #pragma omp task shared(filteredCandidates)
                            filterRecursive(node->left, filteredCandidates, depth + 1);

                            #pragma omp task shared(filteredCandidates)
                            filterRecursive(node->right, filteredCandidates, depth + 1);
                        }
                    }
                }
                else
                {
                    filterRecursive(node->left, filteredCandidates, depth + 1);
                    filterRecursive(node->right, filteredCandidates, depth + 1);
                }
            }
        }
    }

    
    std::shared_ptr<CentroidPoint<PT, PD>> findClosestCandidate(const std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> &candidates, const Point<PT, PD> &target)
    {
        auto closest = candidates[0];
        double minDist = this->distanceTo(*closest, target);

        for (const auto &candidate : candidates)
        {
            double dist = this->distanceTo(*candidate, target);
            if (dist < minDist)
            {
                minDist = dist;
                closest = candidate;
            }
        }
        return closest;
    }

    /** Implements the isFarther fuction discussed in the paper.
     *  Checks if a point z is farther from a bounding box than zStar
     */
    bool isFarther(const Point<PT, PD> &z, const Point<PT, PD> &zStar, const KdNode<PT, PD> &node)
    {
        Point<PT, PD> u = z - zStar;

        Point<PT, PD> vH;
        for (std::size_t i = 0; i < PD; ++i)
        {
            vH.setValue((u.getValues()[i] >= 0) ? node.cellMax[i] : node.cellMin[i], i);
        }

        double distZ = this->distanceTo(z, vH);
        double distZStar = this->distanceTo(zStar, vH);

        return distZ > distZStar;
    }

    /**
     * Assign the centroid to the leaf nodes of the subtree
     */
    void assignCentroid(std::unique_ptr<KdNode<PT, PD>> &node, const std::shared_ptr<CentroidPoint<PT, PD>> &centroid)
    {
        if (!node->left && !node->right)
        {
            node->myPoint->setCentroid(centroid);
            return;
        }
        else
        {
            assignCentroid(node->left, centroid);
            assignCentroid(node->right, centroid);
        }
    }

    bool checkConvergence(int iter)
    {
        if (iter > MAX_ITERATIONS)
            return true;

        if (this->oldCentroids.empty())
            return false;
        else
        {
            PT dist = 0;
            for (int i = 0; i < this->centroids->size(); i++)
            {
                dist += this->distanceTo((*this->centroids)[i], this->oldCentroids[i]);
            }
            dist = dist / this->centroids->size();
            if (dist > treshold)
                return false;
            return true;
        }
    }
};

#endif
