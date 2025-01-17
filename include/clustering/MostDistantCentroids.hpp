#include "CentroidInitMethods.hpp"
#include <random> // For random index generation
#include <limits> // For std::numeric_limits

template<std::size_t PD>
class MostDistantCentroids : public CentroidInitMethods<double, PD> {
public:
    // Constructor that calls the parent constructor
    MostDistantCentroids( std::vector<Point<double, PD>>& points)
        : CentroidInitMethods<double, PD>(points) {}

    MostDistantCentroids( std::vector<Point<double, PD>>& points, int k)
        : CentroidInitMethods<double, PD>(points, k) {}


    void findCentroid(std::vector<CentroidPoint<PT, PD>>& centroids) override {
        std::vector<Point<double, PD>> centroids;
        int limit;
        if (this->m_k == 0)
            set_k(casualK(limit));

        // Generate a random index to select the first point
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, this->m_points.size() - 1);

        // Select the first point randomly
        centroids.push_back( (this->m_data)[casualNumber(limit)] );

        while (centroids.size() < k) {
            // Find the point farthest from the current centroids
            Point<double, PD> farthestPoint;
            double maxDistance = -std::numeric_limits<double>::infinity();

            for (const auto& point : this->m_data) {
                // Compute the minimum distance between the point and all centroids
                double minDistance = std::numeric_limits<double>::infinity();
                for (const auto& centroid : centroids) {
                    double distance = this->distanceTo(point, centroid);
                    if (distance < minDistance) {
                        minDistance = distance;
                    }
                }

                // Update the farthest point if necessary
                if (minDistance > maxDistance) {
                    maxDistance = minDistance;
                    farthestPoint = point;
                }
            }

            // Add the farthest point to the list of centroids
            centroids.push_back(farthestPoint);
        }

        return centroids;
    }

    int casualK(int limit){
        //DA FARE
        return 0;
    }
};
