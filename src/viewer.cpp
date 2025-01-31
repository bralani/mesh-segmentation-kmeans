#include <vector>

#include <iostream>
#include <render.hpp>

#include "mesh_segmentation/MeshSegmentation.hpp"
#include "geometry/metrics/EuclideanMetric.hpp"
#include "geometry/metrics/GeodesicMetric.hpp"

#define DIMENSION 2

using namespace std;

void segmentationCallback(Render &render, const std::string &fileName, int num_initialization_method, int num_k_init_method, int num_clusters)
{
  try
  {
    std::cout << "Selected model path: " << fileName << std::endl;
    std::cout << "Number of clusters: " << num_clusters << std::endl;
    std::cout << "Initialisation Methos: " << num_initialization_method << std::endl;
    std::cout << "Key Init method: " << num_k_init_method << std::endl;

    Mesh mesh(fileName);
    MeshSegmentation<GeodesicHeatMetric<double, 3>> segmentation(&mesh, num_clusters, 1e-4, num_initialization_method, num_k_init_method);
    segmentation.fit();

    std::string outputFile = fileName.substr(0, fileName.find_last_of('.')) + "_segmented.obj";
    mesh.exportToGroupedObj(outputFile);
    std::cout << "Segmented mesh saved to: " << outputFile << std::endl;

    // Notify render to display the segmented file
    render.renderFile(outputFile);
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error during segmentation: " << e.what() << std::endl;
  }
}

int main()
{

  string file_name = "../../resources/meshes/obj_test/1.obj";

  Render render(segmentationCallback);
  render.start();
  return 0;
}
