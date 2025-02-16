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
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <mesh_id> <num_initialization_method> <num_k_init_method>" << endl;
        return 1;
    }
    
    try
    {
        string id = argv[1];
        int num_initialization_method = stoi(argv[2]);
        int num_k_init_method = stoi(argv[3]);

        string file_name = "/Users/matteobalice/Desktop/16-kmeans-16-kmeans/resources/meshes/obj/" + id + ".obj";
        string folderPath = "/Users/matteobalice/Desktop/16-kmeans-16-kmeans/resources/meshes/seg/" + id + "/";
        
        if (fs::exists(folderPath) && fs::is_directory(folderPath))
        {
            Mesh mesh(file_name);
            Mesh mesh2(file_name);

            Entry_CE entry_ce;
            Entry_HD entry_hd;
            Entry_RI entry_ri;
            int count = 0;
            
            auto start = std::chrono::high_resolution_clock::now();
            for (const auto &entry : fs::directory_iterator(folderPath))
            {
                if (fs::is_regular_file(entry.status()))
                {
                    int num_clusters = mesh2.createSegmentationFromSegFile(entry.path());
                    MeshSegmentation<GeodesicHeatMetric<double, 3>> segmentation(&mesh, num_clusters, 0.05, num_initialization_method, num_k_init_method);

                    segmentation.fit();

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
        else
        {
            cerr << "Provided path is not a directory or does not exist!" << endl;
        }
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
