#ifndef MD_HPP
#define MD_HPP

#include "geometry/point/Point.hpp"
#include "geometry/metrics/EuclideanMetric.hpp"
#include "geometry/point/CentroidPoint.hpp"
#include "clustering/CentroidInitializationMethods/CentroidInitMethods.hpp"
#include <random> 
#include <limits> 

#define LIMIT_NUM_CENTROIDS 10

template<std::size_t PD>
class MostDistanceClass : public CentroidInitMethod<double, PD> {
public:
    MostDistanceClass(std::vector<Point<double, PD>>& data, int k) : CentroidInitMethod<double, PD>(data, k) {
        //Do nothing
    }

    MostDistanceClass(std::vector<Point<double, PD>>& data) : CentroidInitMethod<double, PD>(data) {
        //Do nothing
    }

    void findCentroid(std::vector<CentroidPoint<double, PD>>& centroids) override {
        EuclideanMetric<double, PD> metric(this->m_data, 1e-4);
        int limit = LIMIT_NUM_CENTROIDS;
        int index = 1;

        //Generate a random number of centroids if necessary
        if (this->m_k == 0)
            this->set_k(casualNumber(limit));

        // Generate a random index to select the first point
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, this->m_data.size() - 1);

        // Select the first point randomly
        CentroidPoint<double, PD> tmpInitialCentroids( (this->m_data)[casualNumber(limit)] );
        tmpInitialCentroids.setID(0);
        centroids.push_back(tmpInitialCentroids);

        while (centroids.size() < this->m_k) {
            // Find the point farthest from the current centroids
            Point<double, PD> farthestPoint;
            double maxDistance = -std::numeric_limits<double>::infinity();

            for (const auto& point : this->m_data) {
                // Compute the minimum distance between the point and all centroids
                double minDistance = std::numeric_limits<double>::infinity();
                for (const auto& centroid : centroids) {
                    double distance = metric.distanceTo(point, centroid);
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
            CentroidPoint<double, PD> c(farthestPoint);
            c.setID(index);
            index++;
            centroids.push_back(c);
        }
    }

    int casualNumber(int limit) {
        if (limit <= 0) {
            throw std::invalid_argument("Limit must be positive");
        }

        static std::random_device rd;  
        static std::mt19937 gen(rd()); 

        std::uniform_int_distribution<> dis(0, limit - 1);

        return dis(gen);
    }
};


#endif