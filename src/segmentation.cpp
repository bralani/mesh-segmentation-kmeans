#include <vector>
#include <iostream>
#include <cstdlib>

#include "mesh_segmentation/MeshSegmentation.hpp"
#include "geometry/metrics/EuclideanMetric.hpp"
#include "geometry/metrics/GeodesicMetric.hpp"
#include "geometry/metrics/GeodesicHeatMetric.hpp"
#include "clustering/CentroidInitializationMethods/SharedEnum.hpp"

using namespace std;

#define DIM 3

int main(int argc, char *argv[])
{
    try
    {
        if (argc < 5)
        {
            std::cerr << "Usage: " << argv[0] << " <mesh_file> <num_clusters> <init_method> <metric> [k_init_method]" << std::endl;
            std::cerr << "  <mesh_file>       : Name of the mesh file in /resources/meshes/obj folder (without extension)" << std::endl;
            std::cerr << "  <num_clusters>    : Number of clusters (0 if unknown)" << std::endl;
            std::cerr << "  <init_method>     : Initialization method for centroids (0: random, 1: KDE, 2: most distant, 3: Static KDE - 3D point)" << std::endl;
            std::cerr << "  <metric>          : Distance metric (0: Euclidean, 1: Dijkstra, 2: Heat)" << std::endl;
            std::cerr << "  [k_init_method]   : (Optional) Method for k initialization (0: elbow, 1: KDE, 2: Silhouette) if <num_clusters> is 0" << std::endl;
            return 1;
        }

        string file_name = std::string(ROOT_FOLDER) + "/resources/meshes/obj/" + string(argv[1]) + ".obj";

        int num_clusters = std::stoi(argv[2]);
        int num_initialization_method = std::stoi(argv[3]);
        int metric = std::stoi(argv[4]);

        int num_k_init_method = 0;
        if (num_clusters == 0)
        {
            if (argc < 6)
            {
                std::cerr << "Error: You must specify the k initialization method when num_clusters is 0." << std::endl;
                return 1;
            }
            num_k_init_method = std::stoi(argv[5]);
        }

        Mesh mesh(file_name);

        if (metric == Enums::MetricMethod::EUCLIDEAN)
        {
            MeshSegmentation<EuclideanMetric<double, DIM>> segmentation(&mesh, num_clusters, 1e-4, num_initialization_method, num_k_init_method);
            segmentation.fit();
        }
        else if (metric == Enums::MetricMethod::DIJKSTRA)
        {
            MeshSegmentation<GeodesicMetric<double, DIM>> segmentation(&mesh, num_clusters, 0.05, num_initialization_method, num_k_init_method);
            segmentation.fit();
        }
        else if (metric == Enums::MetricMethod::HEAT)
        {
            MeshSegmentation<GeodesicHeatMetric<double, DIM>> segmentation(&mesh, num_clusters, 0.05, num_initialization_method, num_k_init_method);
            segmentation.fit();
        }
        else
        {
            std::cerr << "Error: Invalid metric option. Use 0 (Euclidean), 1 (Dijkstra), or 2 (Heat)." << std::endl;
            return 1;
        }

        // Generate the output file path
        std::string output_file = file_name.substr(0, file_name.find_last_of('.')) + "_segmented.obj";

        // Export the mesh grouped by clusters
        mesh.exportToGroupedObj(output_file);

        std::cout << "Segmented mesh saved to: " << output_file << std::endl;

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
