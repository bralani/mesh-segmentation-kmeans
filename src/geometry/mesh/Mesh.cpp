#include "objload.h"
#include "geometry/mesh/Mesh.hpp"
#include <fstream> // For file output
#include <sstream> // For stringstream

Mesh::Mesh(const std::string path)
{
  try
  {
    std::ifstream in(path.c_str());
    auto model = obj::parseObjModel(in);

    meshVertices.reserve(model.vertex.size() / 3);
    std::vector<Point<double, 3>> localMeshVertices(model.vertex.size() / 3);
    #pragma omp parallel for
    for (int i = 0; i < model.vertex.size(); i += 3)
    {
      std::array<double, 3> coords = {model.vertex[i], model.vertex[i + 1], model.vertex[i + 2]};
      localMeshVertices[i / 3] = Point<double, 3>(coords, i / 3);
    }
    meshVertices = std::move(localMeshVertices);

    const auto faces = model.faces.at("default");
    const auto &faceVertices = faces.first;

    meshFaces.reserve(faces.first.size() / 3);
    std::vector<Face> localMeshFaces(faceVertices.size() / 3);
    #pragma omp parallel for
    for (int j = 0; j < faceVertices.size(); j = j + 3)
    {
      FaceId faceId(j / 3);
      Face face = Face({VertId(faceVertices[j].v), VertId(faceVertices[j + 1].v), VertId(faceVertices[j + 2].v)}, meshVertices, faceId);

      localMeshFaces[j / 3] = face;
    }
    meshFaces = std::move(localMeshFaces);
  }
  catch (const std::exception &e)
  {
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

int Mesh::createSegmentationFromSegFile(const std::filesystem::path &path)
{
  // check if the file has .seg extension
  if (path.extension() != ".seg")
  {
    throw std::invalid_argument("File must have .seg extension");
  }

  std::ifstream file(path);
  if (!file.is_open())
  {
    throw std::runtime_error("Failed to open file");
  }

  std::string line;
  int faceId = 0;
  int maxCluster = 0;
  while (std::getline(file, line))
  {
    std::istringstream iss(line);
    int cluster;
    if (!(iss >> cluster))
    {
      break;
    } // error

    this->setFaceCluster(FaceId(faceId), cluster);

    if (cluster > maxCluster)
    {
      maxCluster = cluster;
    }
    faceId++;
  }

  return maxCluster + 1;
}

void Mesh::buildFaceAdjacency()
{
    faceAdjacency.clear();

    std::unordered_map<VertId, std::set<FaceId>> vertexToFaces;

    #pragma omp parallel
    {
        std::unordered_map<VertId, std::set<FaceId>> vertexToFacesThreadLocal;

        #pragma omp for nowait
        for (size_t i = 0; i < meshFaces.size(); i++)
        {
            const auto &face = meshFaces[i];
            for (VertId vertex : face.vertices)
            {
                vertexToFacesThreadLocal[vertex].insert(face.baricenter.id);
            }
        }

        #pragma omp critical
        {
            for (const auto &[vertex, faces] : vertexToFacesThreadLocal)
            {
                vertexToFaces[vertex].insert(faces.begin(), faces.end());
            }
        }
    }

    std::vector<std::vector<FaceId>> tempFaceAdjacency(meshFaces.size());

    #pragma omp parallel for
    for (size_t i = 0; i < meshFaces.size(); i++)
    {
        const auto &face = meshFaces[i];
        std::set<FaceId> adjacentFacesSet;

        for (VertId vertex : face.vertices)
        {
            const auto &connectedFaces = vertexToFaces[vertex];
            adjacentFacesSet.insert(connectedFaces.begin(), connectedFaces.end());
        }

        adjacentFacesSet.erase(face.baricenter.id);
        tempFaceAdjacency[i] = std::vector<FaceId>(adjacentFacesSet.begin(), adjacentFacesSet.end());
    }

    for (size_t i = 0; i < meshFaces.size(); i++)
    {
        faceAdjacency[meshFaces[i].baricenter.id] = std::move(tempFaceAdjacency[i]);
    }
}

void Mesh::exportToObj(const std::string &filepath, int cluster)
{
  std::ofstream objFile(filepath);

  if (!objFile.is_open())
  {
    std::cerr << "Failed to open file: " << filepath << std::endl;
    return;
  }

  // Write the vertices to the file (in .obj format)
  for (const auto &vertex : meshVertices)
  {
    objFile << "v " << vertex.coordinates[0] << " " << vertex.coordinates[1] << " " << vertex.coordinates[2] << std::endl;
  }

  for (FaceId faceId = 0; faceId < meshFaces.size(); ++faceId)
  {
    if (getFaceCluster(faceId) == cluster)
    { // Check if the cluster ID is 0
      const Face &face = meshFaces[faceId];

      // Write the face in OBJ format (note that OBJ uses 1-based indexing)
      objFile << "f";
      for (const auto &vertId : face.vertices)
      {
        objFile << " " << (vertId + 1); // OBJ indices are 1-based
      }
      objFile << std::endl;
    }
  }

  objFile.close();
  std::cout << "Exported mesh to " << filepath << std::endl;
}

const int Mesh::getFaceCluster(FaceId face) const
{
  try
  {
      return faceClusters.at(face);
  }
  catch (const std::out_of_range &e)
  {
      return -1;
  }
}

std::vector<Point<double, 3>> Mesh::getMeshFacesPoints()
{
    std::vector<Point<double, 3>> faces;
    for (const auto &face : meshFaces)
    {
        faces.push_back(face.baricenter);
    }
    return faces;
}

void Mesh::exportToGroupedObj(const std::string &filepath) const
{
  std::ofstream objFile(filepath);

  if (!objFile.is_open())
  {
    std::cerr << "Failed to open file: " << filepath << std::endl;
    return;
  }

  // Write the vertices to the file (in .obj format)
  objFile << "# Vertices\n";
  for (const auto &vertex : meshVertices)
  {
    objFile << "v " << vertex.coordinates[0] << " "
            << vertex.coordinates[1] << " "
            << vertex.coordinates[2] << std::endl;
  }

  // Initialize current cluster and group
  int currentCluster = -1;

  objFile << "# Faces grouped by clusters\n";
  for (FaceId faceId = 0; faceId < meshFaces.size(); ++faceId)
  {
    int cluster = getFaceCluster(faceId);

    // If cluster changes, write a new group
    if (cluster != currentCluster)
    {
      currentCluster = cluster;
      objFile << "\ng cluster_" << currentCluster << std::endl;
    }

    // Write the face in OBJ format (note that OBJ uses 1-based indexing)
    const Face &face = meshFaces[faceId];
    objFile << "f";
    for (const auto &vertId : face.vertices)
    {
      objFile << " " << (vertId + 1); // OBJ indices are 1-based
    }
    objFile << std::endl;
  }

  objFile.close();
  std::cout << "Exported grouped mesh to " << filepath << std::endl;
}

void Mesh::addVertex(const Point<double, 3> &vertex)
{
  meshVertices.push_back(vertex);
}

void Mesh::addFace(const Face &face)
{
  meshFaces.push_back(face);
}
