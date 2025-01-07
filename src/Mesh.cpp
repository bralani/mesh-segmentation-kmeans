#include "reader_obj.h"
#include "geometry/mesh/Mesh.hpp"

Mesh::Mesh(const std::string path)
{
  
	// Initialize Loader
	objl::Loader Loader;

	bool loadout = Loader.LoadFile(path);


	if (loadout)
	{
		for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = Loader.LoadedMeshes[i];

			for (int j = 0; j < curMesh.Vertices.size(); j++)
			{
        std::array<double, 3> coords = {curMesh.Vertices[j].Position.X, curMesh.Vertices[j].Position.Y, curMesh.Vertices[j].Position.Z};
        Point<double, 3> point(coords, j);
        meshVertices.push_back(point);
			}


			for (int j = 0; j < curMesh.Indices.size(); j += 3)
			{
        FaceId faceId(j / 3);
        Face face = Face({curMesh.Indices[j], curMesh.Indices[j + 1], curMesh.Indices[j + 2]}, meshVertices, faceId);
        
        meshFaces.push_back(face);
			}
		}
	} else {
    throw std::runtime_error("Failed to load file");
	}
}

std::ostream &operator<<(std::ostream &os, const Mesh &graph)
{
  os << "Vertices: " << std::endl;
  for (const auto &vertex : graph.meshVertices)
  {
    os << vertex << std::endl;
  }
  return os;
}

int Mesh::createSegmentationFromSegFile(const std::filesystem::path& path)
{
  // check if the file has .seg extension
  if (path.extension() != ".seg")
  {
    throw std::invalid_argument("File must have .seg extension");
  }

  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file");
  } 

  std::string line;
  int faceId = 0;
  int maxCluster = 0;
  while (std::getline(file, line))
  {
    std::istringstream iss(line);
    int cluster;
    if (!(iss >> cluster)) { break; } // error
    
    this->setFaceCluster(FaceId(faceId), cluster);

    if (cluster > maxCluster)
    {
      maxCluster = cluster;
    }
    faceId++;
  }

  return maxCluster + 1;
}