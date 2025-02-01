#include <vector>

#include <iostream>

#include "mesh_segmentation/MeshSegmentation.hpp"
#include "geometry/metrics/EuclideanMetric.hpp"
#include "geometry/metrics/GeodesicMetric.hpp"

namespace plt = matplotlibcpp;

#define DIMENSION 2

using namespace std;

int main()
{
  try
  {
    string file_name;
    cout << "Enter the name of the mesh file: ";
    cin >> file_name;
    file_name = "../../resources/meshes/obj/" + file_name;

    int num_clusters;
    cout << "Enter the number of clusters (parameter k, 0 if unknow): ";
    cin >> num_clusters;

    int num_initialization_method;
    std::cout << "Enter the number of initialization method for centroids \n (0: random, 1: kernel density estimator, 2: most distant, 3: Static KDE - 3D point )" << std::endl;
    cin >> num_initialization_method;

    Mesh mesh(file_name);


    int num_k_init_method=0;
    if(num_clusters == 0){
      std::cout<<"Enter the method for k initialization (0: elbow, 1: kde, 2: Silhoulette): ";
      cin >> num_k_init_method;
    }

    MeshSegmentation<GeodesicHeatMetric<double, 3>> segmentation(&mesh, num_clusters, 1e-4, num_initialization_method, num_k_init_method) ;

    segmentation.fit();

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
