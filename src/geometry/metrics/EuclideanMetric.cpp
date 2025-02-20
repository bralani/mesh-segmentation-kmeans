#include "geometry/metrics/EuclideanMetric.hpp"

// Constructor
template <typename PT, std::size_t PD>
EuclideanMetric<PT, PD>::EuclideanMetric(std::vector<Point<PT, PD>> data, double threshold) {
    this->data = data;
    this->treshold = threshold;

    #ifdef USE_CUDA
        if (this->data.size() > MIN_NUM_POINTS_CUDA) {
            kdtree = nullptr;
        } else {
            kdtree = new KdTree<PT, PD>(this->data);
        }
    #else
        kdtree = new KdTree<PT, PD>(this->data);
    #endif
}

template <typename PT, std::size_t PD>
EuclideanMetric<PT, PD>::EuclideanMetric(Mesh &mesh, double percentage_threshold, std::vector<Point<PT, PD>> data)
: mesh(&mesh)
{
    this->treshold = percentage_threshold;
    this->data = data;
    
    #ifdef USE_CUDA
        if (this->data.size() > MIN_NUM_POINTS_CUDA) {
            kdtree = nullptr;
        } else {
            kdtree = new KdTree<PT, PD>(this->data);
        }
    #else
        kdtree = new KdTree<PT, PD>(this->data);
    #endif
}

template<typename PT, std::size_t PD>
std::vector<Point<PT, PD>>& EuclideanMetric<PT, PD>::getPoints(){
    return this->data;
}

// Calculating the Euclidean distance between two points
template <typename PT, std::size_t PD>
PT EuclideanMetric<PT, PD>::distanceTo(const Point<PT, PD> &a, const Point<PT, PD> &b) {
    if (a.coordinates.size() != b.coordinates.size()) {
        throw std::invalid_argument("Points must have the same dimensionality");
    }
    PT sum = 0;
    for (size_t i = 0; i < a.coordinates.size(); ++i) {
        sum += std::pow(a.coordinates[i] - b.coordinates[i], 2);
    }
    return std::sqrt(sum);
}

// Setup method (does nothing for this metric)
template <typename PT, std::size_t PD>
void EuclideanMetric<PT, PD>::setup() {}

// Execute clustering on CPU
template <typename PT, std::size_t PD>
void EuclideanMetric<PT, PD>::fit_cpu() {
    bool convergence = false;
    int iter = 0;
    while (!convergence) {
        filter();
        convergence = checkConvergence(iter);
        this->oldCentroids = *this->centroids;
        setup();
        iter++;
    }

    updateFaceClusters();
    storeCentroids();
}

#ifdef USE_CUDA
// Execute clustering on GPU (if CUDA is enabled)
template <typename PT, std::size_t PD>
void EuclideanMetric<PT, PD>::fit_gpu() {
    int numClusters = this->centroids->size();

    float *data_flat = new float[this->data.size() * PD];
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < this->data.size(); i++) {
        for (int j = 0; j < PD; j++) {
            data_flat[i * PD + j] = this->data.at(i).coordinates[j];
        }
    }

    float *centroids_flat = new float[numClusters * PD];
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < numClusters; i++) {
        for (int j = 0; j < PD; j++) {
            centroids_flat[i * PD + j] = this->centroids->at(i).coordinates[j];
        }
    }

    int *cluster_assignment = new int[this->data.size()];

    kmeans_cuda(numClusters, PD, this->data.size(), data_flat, centroids_flat, cluster_assignment, (float)treshold);

    #pragma omp parallel for
    for (int i = 0; i < this->data.size(); i++) {
        std::shared_ptr<CentroidPoint<PT, PD>> centroid_ptr = std::make_shared<CentroidPoint<PT, PD>>(this->centroids->at(cluster_assignment[i]));
        this->data.at(i).setCentroid(centroid_ptr);
    }

    if (mesh != nullptr) {
        updateFaceClusters();
        storeCentroids();
    }

    delete[] data_flat;
    delete[] centroids_flat;
    delete[] cluster_assignment;
}
#endif

// Filter the data
template <typename PT, std::size_t PD>
void EuclideanMetric<PT, PD>::filter() {
    std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> centersPointers;
    for (CentroidPoint<PT, PD> &z : *this->centroids) {
        z.resetCount();
        centersPointers.push_back(std::shared_ptr<CentroidPoint<PT, PD>>(&z, [](CentroidPoint<PT, PD> *) {}));
    }

    #pragma omp parallel
    {
        #pragma omp single
        filterRecursive(kdtree->getRoot(), centersPointers, 0);
    }

    for (CentroidPoint<PT, PD> &c : *this->centroids) {
        c.normalize();
    }
}

// Recursively filter the data
template <typename PT, std::size_t PD>
void EuclideanMetric<PT, PD>::filterRecursive(std::unique_ptr<KdNode<PT, PD>> &node, const std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> &candidates, int depth) {
    if (!node) return;

    if (!node->left && !node->right) {
        auto zStar_ptr = findClosestCandidate(candidates, node->wgtCent);
        *zStar_ptr = *zStar_ptr + *node;
        node->myPoint->setCentroid(zStar_ptr);
        return;
    }

    Point<PT, PD> cellMidpoint;
    for (std::size_t i = 0; i < PD; ++i) {
        cellMidpoint.setValue((node->cellMin[i] + node->cellMax[i]) / PT(2), i);
    }

    auto zStar_ptr = findClosestCandidate(candidates, cellMidpoint);
    std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> filteredCandidates;

    for (auto &z : candidates) {
        if (z == zStar_ptr || !isFarther(*z, *zStar_ptr, *node)) {
            filteredCandidates.push_back(z);
        }
    }

    if (filteredCandidates.size() == 1) {
        *filteredCandidates[0] = *filteredCandidates[0] + *node;
        assignCentroid(node->left, filteredCandidates[0]);
        assignCentroid(node->right, filteredCandidates[0]);
    } else {
        filterRecursive(node->left, filteredCandidates, depth + 1);
        filterRecursive(node->right, filteredCandidates, depth + 1);
    }
}

// Find the closest candidate
template <typename PT, std::size_t PD>
std::shared_ptr<CentroidPoint<PT, PD>> EuclideanMetric<PT, PD>::findClosestCandidate(const std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> &candidates, const Point<PT, PD> &target) {
    auto closest = candidates[0];
    double minDist = this->distanceTo(*closest, target);

    for (const auto &candidate : candidates) {
        double dist = this->distanceTo(*candidate, target);
        if (dist < minDist) {
            minDist = dist;
            closest = candidate;
        }
    }
    return closest;
}

// Check if a point is farther than another
template <typename PT, std::size_t PD>
bool EuclideanMetric<PT, PD>::isFarther(const Point<PT, PD> &z, const Point<PT, PD> &zStar, const KdNode<PT, PD> &node) {
    Point<PT, PD> u = z - zStar;
    Point<PT, PD> vH;

    for (std::size_t i = 0; i < PD; ++i) {
        vH.setValue((u.getValues()[i] >= 0) ? node.cellMax[i] : node.cellMin[i], i);
    }

    double distZ = this->distanceTo(z, vH);
    double distZStar = this->distanceTo(zStar, vH);

    return distZ > distZStar;
}

// Assign a centroid to the leaf nodes
template <typename PT, std::size_t PD>
void EuclideanMetric<PT, PD>::assignCentroid(std::unique_ptr<KdNode<PT, PD>> &node, const std::shared_ptr<CentroidPoint<PT, PD>> &centroid) {
    if (!node->left && !node->right) {
        node->myPoint->setCentroid(centroid);
        return;
    }

    assignCentroid(node->left, centroid);
    assignCentroid(node->right, centroid);
}

// Check if the centroids have converged
template <typename PT, std::size_t PD>
bool EuclideanMetric<PT, PD>::checkConvergence(int iter) {
    if (iter > MAX_ITERATIONS) return true;

    if (this->oldCentroids.empty()) return false;

    PT dist = 0;
    for (size_t i = 0; i < this->centroids->size(); i++) {
        dist += this->distanceTo((*this->centroids)[i], this->oldCentroids[i]);
    }
    dist = dist / this->centroids->size();
    return dist <= treshold;
}

template <typename PT, std::size_t PD>
void EuclideanMetric<PT, PD>::storeCentroids() {
    if (mesh == nullptr) return;
    
    const size_t numFaces = mesh->numFaces();
    for (FaceId faceId = 0; faceId < numFaces; ++faceId)
    {
        int centroidIndex = mesh->getFaceCluster(faceId);
        // Check if the cluster is valid: greater or equal to 0 and less than the size of the centroids vector
        if (centroidIndex < 0 || static_cast<size_t>(centroidIndex) >= this->centroids->size()) {
            std::cerr << "Warning: Face " << faceId << " has not a valid cluster (" << centroidIndex << "). Skipping." << std::endl;
            continue;
        }
        Point<PT, PD>& baricenter = mesh->getFace(faceId).baricenter;
        CentroidPoint<PT, PD>& c = (this->centroids)->at(centroidIndex);
        baricenter.setCentroid(c);
    }

}

template <>
void EuclideanMetric<double, 2>::storeCentroids() {}

template <typename PT, std::size_t PD>
void EuclideanMetric<PT, PD>::updateFaceClusters() {
    const size_t numFaces = mesh->numFaces();
    const size_t numCentroids = this->centroids->size();
    
    // For each face, find the closest centroid based on the euclidean distance
    for (FaceId faceId = 0; faceId < numFaces; ++faceId) {
        double minDistance = std::numeric_limits<double>::max();
        int closestCentroid = -1;
        const Point<PT, PD>& faceCenter = mesh->getFace(faceId).baricenter;
        
        for (size_t i = 0; i < numCentroids; ++i) {
            double distance = this->distanceTo(faceCenter, (*this->centroids)[i]);
            if (distance < minDistance) {
                minDistance = distance;
                closestCentroid = static_cast<int>(i);
            }
        }
        
        mesh->setFaceCluster(faceId, closestCentroid);
    }
}

template <>
void EuclideanMetric<double, 2>::updateFaceClusters() {}

// Explicit template instantiations
template class EuclideanMetric<double, 2>;
template class EuclideanMetric<double, 3>;