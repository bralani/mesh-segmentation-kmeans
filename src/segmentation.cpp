#include <vector>

#include <iostream>

#include "segmentation/MeshSegmentation.hpp"
#include "point/metrics.hpp"

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

    MeshSegmentation<EuclideanMetric<double, 3>> segmentation(&mesh, num_clusters, 1e-4);

    segmentation.fit();

    segmentation.print();

    return 0;
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
}
