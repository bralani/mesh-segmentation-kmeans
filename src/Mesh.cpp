#include "Mesh.hpp"

Mesh::Mesh(const std::filesystem::path& path)
{
  MR::Mesh mesh = *MR::MeshLoad::fromAnySupportedFormat(path);
  this->mesh = mesh;

  buildGraph();
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

std::ostream &operator<<(std::ostream &os, const Mesh &graph)
{
  for (const auto &[vertex, neighbors] : graph.adjacencyList)
  {
    os << "Vertex " << vertex << " is connected to: ";
    for (const auto &neighbor : neighbors)
    {
      os << neighbor << " ";
    }
    os << std::endl;
  }
  return os;
}

const std::unordered_map<MR::VertId, std::vector<MR::VertId>>& Mesh::getGraph() const
{
  return adjacencyList;
}

void Mesh::buildGraph()
{
  std::unordered_map<MR::VertId, std::set<MR::VertId>> tempAdjacencyList;

  for (MR::EdgeId edge(0); edge < mesh.topology.edgeSize(); ++edge)
  {
    if (!mesh.topology.isLoneEdge(edge))
    {
      MR::VertId org = mesh.topology.org(edge);
      MR::VertId dest = mesh.topology.dest(edge);
      tempAdjacencyList[org].insert(dest);
      tempAdjacencyList[dest].insert(org);
    }
  }

  // Convert sets to vectors for adjacencyList
  for (const auto &[vertex, neighbors] : tempAdjacencyList)
  {
    adjacencyList[vertex] = std::vector<MR::VertId>(neighbors.begin(), neighbors.end());
  }

  for (FaceId face(0); face < mesh.topology.faceSize(); ++face) {
    auto coords = mesh.triCenter(face);
    std::array<double, 3> arr = {coords.x, coords.y, coords.z};

    Point<double, 3> point(arr, face);
    meshFacesPoints.push_back(point);
  }
}
