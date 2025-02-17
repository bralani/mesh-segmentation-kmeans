#include <vector>

#include <iostream>
#include <render.hpp>
#include <filesystem>

#include "mesh_segmentation/MeshSegmentation.hpp"
#include "geometry/metrics/EuclideanMetric.hpp"
#include "geometry/metrics/GeodesicMetric.hpp"
#include "geometry/metrics/GeodesicHeatMetric.hpp"

#define DIMENSION 2

using namespace std;
namespace fs = std::filesystem;

void segmentationCallback(Render &render, const std::string &fileName, int num_initialization_method, int num_k_init_method, int num_clusters, double threshold)
{
  try
  {
    std::cout << "Selected model path: " << fileName << std::endl;
    std::cout << "Number of clusters: " << num_clusters << std::endl;
    std::cout << "Initialisation Method: " << num_initialization_method << std::endl;
    std::cout << "Key Init method: " << num_k_init_method << std::endl;
    std::cout << "Threshold value: " << threshold << std::endl;

    Mesh mesh(fileName);
    MeshSegmentation<GeodesicHeatMetric<double, 3>> segmentation(&mesh, num_clusters, threshold, num_initialization_method, num_k_init_method);
    segmentation.fit();

    // Extract filename without extension
    size_t lastDot = fileName.find_last_of('.');
    std::string baseName = (lastDot == std::string::npos) ? fileName : fileName.substr(0, lastDot);
    std::string extension = (lastDot == std::string::npos) ? "" : fileName.substr(lastDot);

    std::string suffix = "_segmented";
    if (baseName.size() >= suffix.size() && baseName.substr(baseName.size() - suffix.size()) == suffix)
    {
      std::cout << "File already contains '_segmented', keeping original name." << std::endl;
    }
    else
    {
      baseName += suffix;
    }

    std::string outputFile = baseName + extension;

    try
    {
      if (fs::exists(outputFile))
      {
        fs::remove(outputFile);
        std::cout << "Previous segmented file deleted: " << outputFile << std::endl;
      }
    }
    catch (const fs::filesystem_error &fsErr)
    {
      std::cerr << "Filesystem error while deleting: " << fsErr.what() << std::endl;
    }

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
  Render render(segmentationCallback);
  render.start();
  return 0;
}
