#include <vector>
#include <filesystem>
#include <iostream>

#include "mesh_segmentation/MeshSegmentation.hpp"
#include "mesh_segmentation/evaluation/Segmentations.hpp"
#include "mesh_segmentation/evaluation/ConsistencyError.hpp"
#include "mesh_segmentation/evaluation/HammingDistance.hpp"
#include "mesh_segmentation/evaluation/RandIndex.hpp"
#include "geometry/metrics/EuclideanMetric.hpp"

namespace fs = std::filesystem;

using namespace std;

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <num_initialization_method> <metric>" << endl;
        std::cout << "  <num_initialization_method> : Initialization method for centroids (0: random, 1: KDE, 2: most distant, 3: Static KDE - 3D point)" << endl;
        std::cout << "  <metric>                     : Distance metric (0: Euclidean, 1: Dijkstra, 2: Heat)" << endl;
        return 1;
    }
    
    try
    {
        int num_initialization_method = stoi(argv[1]);
        int metric = stoi(argv[2]);
        int count = 0;
        Entry_CE entry_ce;
        Entry_HD entry_hd;
        Entry_RI entry_ri;

        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 1; i < 401; i++) {
            string file_name = "D:/16-kmeans-16-kmeans/resources/meshes/obj/" + to_string(i) + ".obj";
            string folderPath = "D:/16-kmeans-16-kmeans/resources/meshes/seg/" + to_string(i) + "/";
            
            if (fs::exists(folderPath) && fs::is_directory(folderPath))
            {
                Mesh mesh(file_name);
                Mesh mesh2(file_name);
                
                for (const auto &entry : fs::directory_iterator(folderPath))
                {
                    if (fs::is_regular_file(entry.status()))
                    {
                        int num_clusters = mesh2.createSegmentationFromSegFile(entry.path());

                        if(metric == 0) {
                            MeshSegmentation<EuclideanMetric<double, 3>> segmentation(&mesh, num_clusters, 1e-4, num_initialization_method, 0);
                            segmentation.fit();
                        }
                        else if(metric == 1) {
                            MeshSegmentation<GeodesicDijkstraMetric<double, 3>> segmentation(&mesh, num_clusters, 0.05, num_initialization_method, 0);
                            segmentation.fit();
                        }
                        else if(metric == 2) {
                            MeshSegmentation<GeodesicHeatMetric<double, 3>> segmentation(&mesh, num_clusters, 0.1, num_initialization_method, 0);
                            segmentation.fit();
                        }

                        Segmentation s1(&mesh, num_clusters);
                        Segmentation s2(&mesh2, num_clusters);

                        Entry_CE* tempConsistency = EvaluateConsistencyError(&s1, &s2);
                        Entry_HD* tempHamming = EvaluateHammingDistance(&s1, &s2);
                        Entry_RI* tempRI = EvaluateRandIndex(&s1, &s2);

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
            }
            else
            {
                cerr << "Provided path is not a directory or does not exist!" << endl;
            }

            std::cout << "Mesh " << i << "/" << 400 << " done!" << std::endl;

            // Compute partial results
            if (count > 0) {
                Entry_CE entry_ce_temp;
                Entry_HD entry_hd_temp;
                Entry_RI entry_ri_temp;

                entry_ce_temp.GCE = entry_ce.GCE / count;
                entry_ce_temp.LCE = entry_ce.LCE / count;
                entry_ce_temp.GCEa = entry_ce.GCEa / count;
                entry_ce_temp.LCEa = entry_ce.LCEa / count;

                entry_hd_temp.distance = entry_hd.distance / count;
                entry_hd_temp.missingRate = entry_hd.missingRate / count;
                entry_hd_temp.falseAlarmRate = entry_hd.falseAlarmRate / count;

                entry_ri_temp.RI = entry_ri.RI / count;
                
                cout << entry_ce_temp << endl;
                cout << entry_hd_temp << endl;
                cout << entry_ri_temp << endl;
            }

        }

        if (count > 0) {
            entry_ce.GCE /= count;
            entry_ce.LCE /= count;
            entry_ce.GCEa /= count;
            entry_ce.LCEa /= count;

            entry_hd.distance /= count;
            entry_hd.missingRate /= count;
            entry_hd.falseAlarmRate /= count;

            entry_ri.RI /= count;
        }

        cout << entry_ce << endl;
        cout << entry_hd << endl;
        cout << entry_ri << endl;

        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;
        cout << "Elapsed time: " << elapsed.count() << " s" << endl;
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
