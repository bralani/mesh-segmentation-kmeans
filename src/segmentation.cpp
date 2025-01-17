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

    int num_clusters;
    cout << "Enter the number of clusters (parameter k): ";
    cin >> num_clusters;

    Mesh mesh(file_name);

    std::cout << mesh << std::endl;

    MeshSegmentation segmentation(&mesh, num_clusters, 1e-4);

    segmentation.fit();

    return 0;
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
}
