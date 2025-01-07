#include <vector>
#include <filesystem>
#include <iostream>

#include "mesh_segmentation/MeshSegmentation.hpp"
#include "mesh_segmentation/evaluation/Segmentations.hpp"
#include "mesh_segmentation/evaluation/ConsistencyError.hpp"
#include "mesh_segmentation/evaluation/HammingDistance.hpp"
#include "mesh_segmentation/evaluation/RandIndex.hpp"
#include "geometry/metrics/EuclideanMetric.hpp"

namespace plt = matplotlibcpp;
namespace fs = std::filesystem;

#define DIMENSION 2

using namespace std;

int main()
{
  try
  {
    string id;
    cout << "Enter the ID of the mesh file: ";
    cin >> id;

    std::cout << std::endl;

    string file_name = "/Users/matteobalice/Desktop/16-kmeans-16-kmeans/resources/meshes/obj/" + id + ".obj";
    Mesh mesh(file_name);
    Mesh mesh2(file_name);

    // read all segmentations from
    string folderPath = "/Users/matteobalice/Desktop/16-kmeans-16-kmeans/resources/meshes/seg/" + id + "/";
    try
    {
      if (fs::exists(folderPath) && fs::is_directory(folderPath))
      {
        Entry_CE entry_ce;
        Entry_HD entry_hd;
        Entry_RI entry_ri;

        int count = 0;

        for (const auto &entry : fs::directory_iterator(folderPath))
        {
          if (fs::is_regular_file(entry.status()))
          {
            int num_clusters = mesh2.createSegmentationFromSegFile(entry.path());

            MeshSegmentation<EuclideanMetric<double, 3>> segmentation(&mesh, num_clusters, 1e-4);

            segmentation.fit();

            segmentation.assignClustersToMesh();

            Segmentation s1(&mesh, num_clusters);
            Segmentation s2(&mesh2, num_clusters);

            // Call evaluation methods
            struct Entry_CE* tempConsistency = EvaluateConsistencyError(&s1, &s2);
            struct Entry_HD* tempHamming = EvaluateHammingDistance(&s1, &s2);
            struct Entry_RI* tempRI = EvaluateRandIndex(&s1, &s2);

            entry_ce.GCE += tempConsistency->GCE;
            entry_ce.LCE += tempConsistency->LCE;
            entry_ce.GCEa += tempConsistency->GCEa;
            entry_ce.LCEa += tempConsistency->LCEa;

            entry_hd.distance += tempHamming->distance;
            entry_hd.missingRate += tempHamming->missingRate;
            entry_hd.falseAlarmRate += tempHamming->falseAlarmRate;

            entry_ri.RI += tempRI->RI;
            count++;
          }
        }

        entry_ce.GCE /= count;
        entry_ce.LCE /= count;
        entry_ce.GCEa /= count;
        entry_ce.LCEa /= count;

        entry_hd.distance /= count;
        entry_hd.missingRate /= count;
        entry_hd.falseAlarmRate /= count;

        entry_ri.RI /= count;

        cout << entry_ce << std::endl;
        cout << entry_hd << std::endl;
        cout << entry_ri << std::endl;

      }
      else
      {
        std::cout << "Provided path is not a directory or does not exist!" << std::endl;
      }
    }
    catch (const std::exception &e)
    {
      std::cout << "Error: " << e.what() << std::endl;
    }

    return 0;
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
}
