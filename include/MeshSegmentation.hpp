#ifndef MESH_SEGMENTATION_HPP
#define MESH_SEGMENTATION_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <random>
#include <unordered_map>

// Include required header files
#include "Mesh.hpp"
#include "metrics.hpp"
#include "KMeans.hpp"

template <class M>
class MeshSegmentation : public KMeans<double, 3, M>  // Fixed PD to 3 for 3D points
{
public:
    // Constructor: Inherit from KMeans and pass the parameters
    MeshSegmentation(Mesh* mesh, int clusters, double threshold)
        : KMeans<double, 3, M>(clusters, mesh->getMeshFacesPoints(), threshold)
    {
        this->mesh = mesh;
    }

    // Method to perform mesh segmentation (clustering on a 3D mesh)
    void fit();

    void separateMeshes();

private:
    Mesh* mesh;  // Pointer to the mesh
};

/** Segment the 3D mesh by assigning points to the nearest centroids */
template <class M>
void MeshSegmentation<M>::fit()
{
    // Perform the KMeans clustering from the base class
    KMeans<double, 3, M>::fit();

    // Separate the meshes based on the clusters
    separateMeshes();
}

template <class M>
void MeshSegmentation<M>::separateMeshes() {

    for(int k = 0; k < 5; k++) {

        Mesh cur_mesh("/Users/matteobalice/Desktop/16-kmeans-16-kmeans/resources/meshes/stl/75.stl");

        std::vector<Point<double, 3>>& points = KMeans<double, 3, M>::getPoints();

        for (auto& point : points) {
            // Get the centroid of the point
            std::shared_ptr<Point<double, 3>> centroid = point.centroid;

            FaceId face = FaceId(point.id);
            int cluster = centroid->id;

            if (cluster != k) {
                cur_mesh.getMeshTopology().deleteFace(face);
            }
        }

        // Save the mesh
        MR::MeshSave::toAnySupportedFormat(cur_mesh.getMesh(), "output" + std::to_string(k) + ".stl");
    }
}


#endif // MESH_SEGMENTATION_HPP