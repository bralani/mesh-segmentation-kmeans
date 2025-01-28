#include "clustering/CentroidInitializationMethods/MostDistantCentroids.hpp"
#include "geometry/metrics/EuclideanMetric.hpp"

template<std::size_t PD>
MostDistanceClass<PD>::MostDistanceClass(std::vector<Point<double, PD>>& data, int k)
    : CentroidInitMethod<double, PD>(data, k) {
    // Costruttore
}

template<std::size_t PD>
MostDistanceClass<PD>::MostDistanceClass(std::vector<Point<double, PD>>& data)
    : CentroidInitMethod<double, PD>(data) {
    // Costruttore
}

template<std::size_t PD>
void MostDistanceClass<PD>::findCentroid(std::vector<CentroidPoint<double, PD>>& centroids) {
    EuclideanMetric<double, PD> metric(this->m_data, 1e-4);
    int limit = LIMIT_NUM_CENTROIDS;
    int index = 1;

    if (this->m_k == 0)
        this->set_k(casualNumber(limit));

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, this->m_data.size() - 1);

    CentroidPoint<double, PD> tmpInitialCentroids((this->m_data)[casualNumber(limit)]);
    tmpInitialCentroids.setID(0);
    centroids.push_back(tmpInitialCentroids);

    while (centroids.size() < this->m_k) {
        Point<double, PD> farthestPoint;
        double maxDistance = -std::numeric_limits<double>::infinity();

        for (const auto& point : this->m_data) {
            double minDistance = std::numeric_limits<double>::infinity();
            for (const auto& centroid : centroids) {
                double distance = metric.distanceTo(point, centroid);
                if (distance < minDistance) {
                    minDistance = distance;
                }
            }

            if (minDistance > maxDistance) {
                maxDistance = minDistance;
                farthestPoint = point;
            }
        }

        CentroidPoint<double, PD> c(farthestPoint);
        c.setID(index);
        index++;
        centroids.push_back(c);
    }
    this->exportedMesh(this->m_data, "Mesh");
    this->exportedMesh(centroids, "Centroids");
}

template<std::size_t PD>
int MostDistanceClass<PD>::casualNumber(int limit) {
    if (limit <= 0) {
        throw std::invalid_argument("Limit must be positive");
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, limit - 1);

    return dis(gen);
}

template class MostDistanceClass<2>;
template class MostDistanceClass<3>;