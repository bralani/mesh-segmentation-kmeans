#include "geometry/metrics/EuclideanMetric.hpp"

// Costruttore
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

template<typename PT, std::size_t PD>
std::vector<Point<PT, PD>>& EuclideanMetric<PT, PD>::getPoints(){
    return this->data;
}



// Calcola la distanza euclidea tra due punti
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

// Metodo setup (non fa nulla per questa metrica)
template <typename PT, std::size_t PD>
void EuclideanMetric<PT, PD>::setup() {
    // Metodo vuoto
}

// Esegue il clustering su CPU
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
}

#ifdef USE_CUDA
// Esegue il clustering su GPU (se CUDA è abilitato)
template <typename PT, std::size_t PD>
void EuclideanMetric<PT, PD>::fit_gpu() {
    int numClusters = this->centroids->size();

    float *data_flat = new float[data.size() * PD];
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < data->size(); i++) {
        for (int j = 0; j < PD; j++) {
            data_flat[i * PD + j] = data->at(i).coordinates[j];
        }
    }

    float *centroids_flat = new float[numClusters * PD];
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < numClusters; i++) {
        for (int j = 0; j < PD; j++) {
            centroids_flat[i * PD + j] = centroids->at(i).coordinates[j];
        }
    }

    int *cluster_assignment = new int[data.size()];

    kmeans_cuda(numClusters, PD, data.size(), data_flat, centroids_flat, cluster_assignment, (float)treshold);

    #pragma omp parallel for
    for (int i = 0; i < data.size(); i++) {
        std::shared_ptr<CentroidPoint<PT, PD>> centroid_ptr = std::make_shared<CentroidPoint<PT, PD>>(this->centroids->at(cluster_assignment[i]));
        data.at(i).setCentroid(centroid_ptr);
    }

    delete[] data_flat;
    delete[] centroids_flat;
    delete[] cluster_assignment;
}
#endif

// Filtra i dati
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

// Metodo ricorsivo per il filtraggio
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

// Trova il candidato più vicino
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

// Verifica se un punto è più lontano di un altro
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

// Assegna un centroid ai nodi foglia
template <typename PT, std::size_t PD>
void EuclideanMetric<PT, PD>::assignCentroid(std::unique_ptr<KdNode<PT, PD>> &node, const std::shared_ptr<CentroidPoint<PT, PD>> &centroid) {
    if (!node->left && !node->right) {
        node->myPoint->setCentroid(centroid);
        return;
    }

    assignCentroid(node->left, centroid);
    assignCentroid(node->right, centroid);
}

// Controlla la convergenza
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
    //Do nothings 
}

// Istanze esplicite
template class EuclideanMetric<double, 2>;
template class EuclideanMetric<double, 3>;