#ifndef GEODETICMETRIC_HPP
#define GEODETICMETRIC_HPP

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
    GeodesicMetric(const Mesh &mesh)
        : mesh(mesh) {}

    void setCentroids(std::vector<CentroidPoint<PT, PD>> &centroids) override
    {
        this->centroids = std::ref(centroids);
    }

    void setup() override
    {
        std::unordered_map<int, std::unordered_map<FaceId, PT>> distances;
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
            for (FaceId faceId = 0; faceId < mesh.numFaces(); ++faceId)
            {
                PT distance = computeDijkstraDistance(startFace, faceId);
                distances[centroidId][faceId] = distance;
            }
        }

        this->distances = std::move(distances);
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
        return distances.at(a.id).at(b.id);
    }

private:
    Mesh mesh;
    std::optional<std::reference_wrapper<std::vector<CentroidPoint<PT, PD>>>> centroids;
    std::unordered_map<int, std::unordered_map<FaceId, PT>> distances;

    double computeEuclideanDistance(const Point<PT, PD>& a, const Point<PT, PD>& b) const
    {
        double sum = 0.0;
        for (std::size_t i = 0; i < PD; ++i)
        {
            sum += std::pow(a.coordinates[i] - b.coordinates[i], 2);
        }
        return std::sqrt(sum);
    }

    PT computeDijkstraDistance(FaceId startFace, FaceId endFace) const
    {
        // Check if the points are in the same face
        if (startFace == endFace)
        {
            return 0;
        }

        cout << "Computing distance between " << startFace << " and " << endFace << endl;

        // Initialize Dijkstra's algorithm
        std::unordered_map<FaceId, PT> distances; // Minimum distance from startFace
        std::unordered_map<FaceId, bool> visited; // Keep track of visited faces
        std::priority_queue<std::pair<PT, FaceId>, std::vector<std::pair<PT, FaceId>>, std::greater<>> pq;

        // Initialize distances and visited flags
        for (const auto &pair : mesh.getFaceAdjacency())
        {
            distances[pair.first] = std::numeric_limits<PT>::max();
            visited[pair.first] = false;
        }

        distances[startFace] = 0;
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
            for (const auto &neighbor : mesh.getFaceAdjacency()[currentFace])
            {
                const auto &neighborFace = mesh.getFace(neighbor);

                // Compute distance between baricenters
                const auto &currentBaricenter = mesh.getFace(currentFace).baricenter;
                const auto &neighborBaricenter = neighborFace.baricenter;
                PT weight = computeEuclideanDistance(currentBaricenter, neighborBaricenter);

                // Update the distance if a shorter path is found
                if (distances[currentFace] + weight < distances[neighbor])
                {
                    distances[neighbor] = distances[currentFace] + weight;
                    pq.push({distances[neighbor], neighbor});
                }
            }
        }

        if (distances[endFace] == std::numeric_limits<PT>::max())
        {
            throw std::runtime_error("No path found between the two points.");
        }

        return distances[endFace];
    }
};

#endif