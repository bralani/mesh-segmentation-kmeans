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
    int index = 1;
    centroids.emplace_back(this->m_data[0]);  // Primo centroide
    centroids.back().setID(0);

    while (centroids.size() < static_cast<size_t>(this->m_k)) {
        Point<double, PD> farthestPoint;
        double maxDistance = -std::numeric_limits<double>::infinity();

        #pragma omp parallel
        {
            Point<double, PD> localFarthestPoint;
            double localMaxDistance = -std::numeric_limits<double>::infinity();

            #pragma omp for nowait reduction(max:maxDistance)
            for (size_t i = 0; i < this->m_data.size(); ++i) {
                const auto& point = this->m_data[i];
                double minDistance = std::numeric_limits<double>::infinity();

                for (const auto& centroid : centroids) {
                    double distance = EuclideanMetric<double, PD>::distanceTo(point, centroid);
                    if (distance < minDistance) {
                        minDistance = distance;
                    }
                }

                if (minDistance > localMaxDistance) {
                    localMaxDistance = minDistance;
                    localFarthestPoint = point;
                }
            }

            #pragma omp critical
            {
                if (localMaxDistance > maxDistance) {
                    maxDistance = localMaxDistance;
                    farthestPoint = localFarthestPoint;
                }
            }
        }

        centroids.emplace_back(farthestPoint);
        centroids.back().setID(index++);
    }

    this->exportedMesh(this->m_data, "Mesh");
    this->exportedMesh(centroids, "Centroids");
}


template class MostDistanceClass<2>;
template class MostDistanceClass<3>;