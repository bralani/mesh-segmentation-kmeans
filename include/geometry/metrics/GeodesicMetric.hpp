#ifndef GEODESICMETRIC_HPP
#define GEODESICMETRIC_HPP

#include <queue>
#include <utility>
#include <optional>
#include <functional> 
#include "geometry/mesh/Mesh.hpp"
#include "geometry/point/CentroidPoint.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>


template <typename PT, std::size_t PD>
class GeodesicMetric : public Metric<PT, PD>
{
public:

    GeodesicMetric(const Mesh &mesh) : mesh(mesh) {}

    void setMesh(const Mesh &mesh)
    {
        this->mesh = mesh;
    }

    void setCentroids(std::vector<CentroidPoint<PT, PD>> &centroids) override
    {
        this->centroids = std::ref(centroids);
    }

    void setup() override
    {
        std::unordered_map<int, FaceId> centroidToFaceMap;

        // Map each centroid to the closest face
        for (const auto &centroid : centroids->get())
        {
            FaceId closestFaceId = findClosestFace(centroid);
            centroidToFaceMap[centroid.id] = closestFaceId;
        }

        // Compute the distance between each pair of faces
        for (const auto &[centroidId, startFace] : centroidToFaceMap)
        {
            std::vector<PT> current_distances = computeDijkstraDistances(startFace);
            this->distances[FaceId(centroidId)] = current_distances;
        }
    }

    void initialSetup() override
    {
        if (!centroids.has_value())
        {
            throw std::runtime_error("Centroids not set!");
        }

        mesh.buildFaceAdjacency();
        setup();
    }

    FaceId findClosestFace(const Point<PT, PD> &centroid) const
    {
        double minDistance = std::numeric_limits<double>::max();
        FaceId closestFaceId = 0;

        // Iterate over all faces
        for (FaceId faceId = 0; faceId < mesh.numFaces(); ++faceId)
        {
            const auto &face = mesh.getFace(faceId);
            const auto &baricenter = face.baricenter;

            // Compute euclidean distance between the centroid and the baricenter
            double distance = computeEuclideanDistance(centroid, baricenter);

            // Update the closest FaceId
            if (distance < minDistance)
            {
                minDistance = distance;
                closestFaceId = faceId;
            }
        }

        return closestFaceId;
    }

    PT distanceTo(const Point<PT, PD> &a, const Point<PT, PD> &b) const override
    {
        
        FaceId closest = findClosestFace(b);
        auto dist = this->distances.at(FaceId(a.id));

        return dist.at(closest);
    }

private:
    Mesh mesh;
    std::optional<std::reference_wrapper<std::vector<CentroidPoint<PT, PD>>>> centroids;
    std::unordered_map<FaceId, std::vector<PT>> distances;

    double computeEuclideanDistance(const Point<PT, PD>& a, const Point<PT, PD>& b) const
    {
        double sum = 0.0;
        for (std::size_t i = 0; i < PD; ++i)
        {
            sum += std::pow(a.coordinates[i] - b.coordinates[i], 2);
        }
        return std::sqrt(sum);
    }

    std::vector<PT> computeDijkstraDistances(FaceId startFace) const
    {
        // Initialize Dijkstra's algorithm
        std::vector<PT> curr_distances(mesh.numFaces()); // Minimum distance from startFace
        std::unordered_map<FaceId, bool> visited; // Keep track of visited faces
        std::priority_queue<std::pair<PT, FaceId>, std::vector<std::pair<PT, FaceId>>, std::greater<>> pq;

        // Initialize curr_distances and visited flags
        for (int i = 0; i < mesh.numFaces(); ++i) {
            curr_distances[i] = std::numeric_limits<PT>::max();
            visited[i] = false;
        }

        curr_distances[startFace] = 0;
        pq.push({0, startFace});

        // Execute Dijkstra
        while (!pq.empty())
        {
            auto [currentDistance, currentFace] = pq.top();
            pq.pop();

            // Check if the face has already been visited
            if (visited[currentFace])
                continue;
            visited[currentFace] = true;

            // Iterate over the neighbors of the current face
            for (const auto &neighbor : mesh.getFaceAdjacencyAt(currentFace))
            {
                const auto &neighborFace = mesh.getFace(neighbor);

                // Compute distance between baricenters
                const auto &currentBaricenter = mesh.getFace(currentFace).baricenter;
                const auto &neighborBaricenter = neighborFace.baricenter;
                PT weight = computeEuclideanDistance(currentBaricenter, neighborBaricenter);

                // Update the distance if a shorter path is found
                if (curr_distances[currentFace] + weight < curr_distances[neighbor])
                {
                    curr_distances[neighbor] = curr_distances[currentFace] + weight;
                    pq.push({curr_distances[neighbor], neighbor});
                }
            }
        }

        return curr_distances;
    }
};

#endif // GEODESICMETRIC_HPP